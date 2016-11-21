#!/usr/bin/env python
from __future__ import print_function
import os
import sys
import unittest
import lxml.etree

IS_PY2 = sys.version_info[0] == 2

if IS_PY2:
    STR_TYPES = (basestring,)
else:
    STR_TYPES = (unicode, bytes)

if IS_PY2:
    from cStringIO import StringIO
else:
    from io import StringIO


def assert_texty(x):
    assert isinstance(x, STR_TYPES)

def assert_numeric(x):
    return isinstance(x, (float, int))

def doeach(fn, *args):
    if len(args) == 0:
        return
    if len(args) == 1 and isinstance(args[0], (tuple, list)):
        args = args[0]
    for arg in args:
        fn(arg)


def assert_xml_node(node):
    assert isinstance(node, lxml.etree._Element)



class GTestFailure(object):
    def __init__(self, message):
        assert_texty(message)
        self.message = message

    @classmethod
    def from_element(cls, elem):
        assert_xml_node(elem)
        assert elem.tag.lower() == "failure"
        return cls(message=elem.attrib['message'])


class GTestCase(object):
    def __init__(self, name, class_name, status, duration, failure=None):
        doeach(assert_texty, name, class_name)
        assert_texty(status)
        assert_numeric(duration)
        if failure is not None:
            assert isinstance(failure, GTestFailure)
        self.name = name
        self.class_name = class_name
        self.status = status
        self.duration = duration
        self.failure = failure

    def succeeded(self):
        return not self.failed()

    def failed(self):
        return self.failure is not None

    @classmethod
    def from_element(cls, elem):
        assert_xml_node(elem)
        assert elem.tag.lower() == "testcase"
        case = GTestCase(
            name=elem.attrib['name'],
            duration=float(elem.attrib['time']),
            class_name=elem.attrib['classname'],
            status=elem.attrib['status']
        )
        children = elem.getchildren()
        if children:
            assert len(children) == 1
            case.failure = GTestFailure.from_element(children[0])
        return case


class GTestSuiteStats(object):
    def __init__(self, n_failures, n_errors, duration):
        doeach(assert_numeric, n_failures, n_errors, duration)
        self.n_failures = n_failures
        self.n_errors = n_errors
        self.duration = duration

    @classmethod
    def from_attr_dict(cls, attr_dict):
        return cls(
            n_failures=int(attr_dict['failures']),
            n_errors=int(attr_dict['errors']),
            duration=float(attr_dict['time'])
        )


class GTestSuite(object):
    def __init__(self, name, stats, test_cases=None):
        test_cases = test_cases or []
        for item in test_cases:
            assert isinstance(item, GTestCase)
        assert_texty(name)
        assert isinstance(stats, GTestSuiteStats)
        self.name = name
        self.stats = stats
        self.test_cases = test_cases

    def add_test_case (self, test):
        assert isinstance(test, GTestCase)
        self.test_cases.append(test)

    @classmethod
    def from_element(cls, elem):
        assert_xml_node(elem)
        assert elem.tag.lower() == "testsuite"
        suite = GTestSuite(
            name=elem.attrib['name'],
            stats=GTestSuiteStats.from_attr_dict(elem.attrib)
        )
        for child in elem.getchildren():
            suite.add_test_case(GTestCase.from_element(child))
        return suite

    def total_test_count(self):
        return len(self.test_cases)

    def iter_failed_tests(self):
        for test_case in self.test_cases:
            if test_case.failed():
                yield test_case


class GTestSuiteSet(object):
    def __init__(self, name, stats, test_suites=None):
        test_suites = test_suites or []
        for item in test_suites:
            assert isinstance(item, GTestSuite)
        assert_texty(name)
        assert isinstance(stats, GTestSuiteStats)
        self.name = name
        self.stats = stats
        self.test_suites = test_suites

    def add_test_suite(self, suite):
        assert isinstance(suite, GTestSuite)
        self.test_suites.append(suite)

    @classmethod
    def from_element(cls, elem):
        assert_xml_node(elem)
        assert elem.tag.lower() == "testsuites"
        name = elem.attrib['name']
        stats = GTestSuiteStats.from_attr_dict(elem.attrib)
        suites = GTestSuiteSet(name=name, stats=stats)
        for child in elem.getchildren():
            suites.add_test_suite(GTestSuite.from_element(child))
        return suites

    @classmethod
    def parse(cls, xml_doc):
        if isinstance(xml_doc, STR_TYPES):
            doc = lxml.etree.fromstring(xml_doc)
        else:
            doc = lxml.etree.parse(xml_doc)

        return cls.from_element(doc)

    def total_test_count(self):
        return sum([suite.total_test_count() for suite in self.test_suites])

    def iter_failed_tests(self):
        for test_suite in self.test_suites:
            for failed_test in test_suite.iter_failed_tests():
                yield test_suite, failed_test


GTEST_XML = """<?xml version="1.0" encoding="UTF-8"?>
<testsuites tests="4" failures="2" disabled="0" errors="0" timestamp="2016-11-20T15:08:47" time="0.051" name="AllTests">
  <testsuite name="TestDefaultValueMap" tests="2" failures="0" disabled="0" errors="0" time="0">
    <testcase name="Simple" status="run" time="0" classname="TestDefaultValueMap" />
    <testcase name="Modification" status="run" time="0" classname="TestDefaultValueMap" />
  </testsuite>
  <testsuite name="TestUTF8IndexView" tests="2" failures="2" disabled="0" errors="0" time="0.002">
    <testcase name="SimpleAscii" status="run" time="0.001" classname="TestUTF8IndexView">
      <failure message="/home/scott/code/score/score_core/test_unit/io/test_UTF8IndexView.cpp:25&#x0A;      Expected: expected&#x0A;      Which is: { (0, 99), (1, 97), (2, 116), (3, 45), (4, 114), (5, 101), (6, 108), (7, 97), (8, 116), (9, 101), (10, 100), (11, 32), (12, 116), (13, 101), (14, 120), (15, 116) }&#x0A;To be equal to: actual&#x0A;      Which is: { (0, 0), (1, 0), (2, 0), (3, 0), (4, 0), (5, 0), (6, 0), (7, 0), (8, 116), (9, 101), (10, 100), (11, 32), (12, 116), (13, 101), (14, 120), (15, 116) }" type=""><![CDATA[/home/scott/code/score/score_core/test_unit/io/test_UTF8IndexView.cpp:25
      Expected: expected
      Which is: { (0, 99), (1, 97), (2, 116), (3, 45), (4, 114), (5, 101), (6, 108), (7, 97), (8, 116), (9, 101), (10, 100), (11, 32), (12, 116), (13, 101), (14, 120), (15, 116) }
To be equal to: actual
      Which is: { (0, 0), (1, 0), (2, 0), (3, 0), (4, 0), (5, 0), (6, 0), (7, 0), (8, 116), (9, 101), (10, 100), (11, 32), (12, 116), (13, 101), (14, 120), (15, 116) }]]></failure>
    </testcase>
    <testcase name="SomeUnicode" status="run" time="0" classname="TestUTF8IndexView">
      <failure message="/home/scott/code/score/score_core/test_unit/io/test_UTF8IndexView.cpp:42&#x0A;      Expected: expected&#x0A;      Which is: { (0, 99), (1, 97), (2, 116), (3, 8212), (6, 114), (7, 101), (8, 108), (9, 97), (10, 116), (11, 101), (12, 100), (13, 32), (14, 116), (15, 101), (16, 120), (17, 116) }&#x0A;To be equal to: actual&#x0A;      Which is: { (0, 0), (1, 0), (2, 0), (3, 0), (4, 0), (5, 0), (6, 0), (7, 0), (8, 108), (9, 97), (10, 116), (11, 101), (12, 100), (13, 32), (14, 116), (15, 101), (16, 120), (17, 116) }" type=""><![CDATA[/home/scott/code/score/score_core/test_unit/io/test_UTF8IndexView.cpp:42
      Expected: expected
      Which is: { (0, 99), (1, 97), (2, 116), (3, 8212), (6, 114), (7, 101), (8, 108), (9, 97), (10, 116), (11, 101), (12, 100), (13, 32), (14, 116), (15, 101), (16, 120), (17, 116) }
To be equal to: actual
      Which is: { (0, 0), (1, 0), (2, 0), (3, 0), (4, 0), (5, 0), (6, 0), (7, 0), (8, 108), (9, 97), (10, 116), (11, 101), (12, 100), (13, 32), (14, 116), (15, 101), (16, 120), (17, 116) }]]></failure>
    </testcase>
  </testsuite>
</testsuites>
"""


class TestParsing(unittest.TestCase):
    def test_parsing(self):
        suites = GTestSuiteSet.parse(GTEST_XML)
        self.assertEqual("AllTests", suites.name)
        self.assertEqual(4, suites.total_test_count())
        self.assertEqual(2, suites.stats.n_failures)
        self.assertEqual(2, len(suites.test_suites))


class FailedTests(RuntimeError):
    pass


def splat(maybe_seq):
    if len(maybe_seq) == 1 and isinstance(maybe_seq, (list, tuple)):
        maybe_seq = maybe_seq[0]
    return list(maybe_seq)


class TermBuffer(object):
    def __init__(self):
        self._buffer = StringIO()

    def write(self, msg, color=None):
        to_write = [None, msg, None]
        if color is not None:
            to_write[0] = ansi_code_of_print_color(color)
            to_write[-1] = ANSICode.ENDC
        self._buffer.write(''.join(list(filter(None, to_write))))

    def seek(self, *args, **kwargs):
        self._buffer.seek(*args, **kwargs)

    def read(self, *args, **kwargs):
        return self._buffer.read(*args, **kwargs)

    def __str__(self):
        self.seek(0)
        return self.read()



class Table(object):
    def __init__(self, *column_names):
        self.column_names = splat(column_names)
        for name in self.column_names:
            assert_texty(name)
        self.body_rows = []
        self.summary_rows = []

    def add_body_row(self, *values):
        values = splat(values)
        assert len(values) == len(self.column_names)
        self.body_rows.append(values)

    def add_summary_row(self, *values):
        values = splat(values)
        assert len(values) == len(self.column_names)
        self.summary_rows.append(values)

    def to_buffer(self):
        max_lengths = list(map(len, self.column_names))
        str_body_rows = []
        str_summary_rows = []

        def _stringify_cell(idx, cell_data):
            if isinstance(cell_data, float):
                cell_data = "%3.3f" % cell_data
            if not isinstance(cell_data, STR_TYPES):
                cell_data = str(cell_data)
            if len(cell_data) > max_lengths[idx]:
                max_lengths[idx] = len(cell_data)
            return cell_data

        def _stringify_row(row):
            as_strs = []
            for i, elem in enumerate(row):
                as_strs.append(_stringify_cell(i, elem))
            return as_strs

        for row in self.body_rows:
            str_body_rows.append(_stringify_row(row))

        for row in self.summary_rows:
            str_summary_rows.append(_stringify_row(row))

        for i in range(len(max_lengths)):
            max_lengths[i] += 3

        divider_size = sum(max_lengths) + (len(self.column_names) * 2) + 2
        buff = TermBuffer()

        def _write_cell(idx, cell_data):
            buff.write("| ")
            n_pad = max_lengths[idx] - len(cell_data)
            if n_pad:
                cell_data += (" " * n_pad)
            buff.write(cell_data)

        def _write_row(row_data):
            for idx, cell in enumerate(row_data):
                _write_cell(idx, cell)
            buff.write(" |\n")

        def _write_divider():
            buff.write("-" * divider_size)
            buff.write("\n")


        _write_divider()
        _write_row(self.column_names)
        _write_divider()
        for row in str_body_rows:
            _write_row(row)
        _write_divider()
        for row in str_summary_rows:
            _write_row(row)
        _write_divider()
        buff.write("\n")
        return buff



class ANSICode(object):
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    LIGHT_GRAY = '\033[37m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


class PrintColor(object):
    RED = "RED"
    BLUE = "BLUE"
    GREEN = "GREEN"
    LIGHT_GRAY = "LIGHT_GRAY"

def ansi_code_of_print_color(color):
    return {
        PrintColor.RED: ANSICode.FAIL,
        PrintColor.GREEN: ANSICode.OKGREEN,
        PrintColor.BLUE: ANSICode.OKBLUE,
        PrintColor.LIGHT_GRAY: ANSICode.LIGHT_GRAY
    }[color]


class Reporter(object):
    def __init__(self, xml_dir):
        self.xml_dir = xml_dir

    def run(self):
        total = 0
        total_failures = 0
        total_duration = 0.0
        table = Table("name", "success", "failure", "total", "duration")
        suite_sets = []
        for fname in os.listdir(self.xml_dir):
            fpath = os.path.join(self.xml_dir, fname)
            with open(fpath) as f:
                suite_set = GTestSuiteSet.parse(f.read())
            suite_sets.append((fname, suite_set))
            suite_name = fname
            current_total = suite_set.total_test_count()
            success = current_total - suite_set.stats.n_failures
            total += current_total
            total_failures += suite_set.stats.n_failures
            total_duration += suite_set.stats.duration
            table.add_body_row(suite_name,
                success,
                suite_set.stats.n_failures,
                current_total,
                suite_set.stats.duration
            )

        table.add_summary_row("OVERALL", total - total_failures, total_failures, total, total_duration)
        buff = table.to_buffer()
        buff.seek(0)
        print(buff.read())
        if total_failures > 0:
            failed_names = []
            for fname, suite_set in suite_sets:
                fail_buff = TermBuffer()
                for suite, test in suite_set.iter_failed_tests():
                    test_name = "%s::%s::%s::%s" % (
                        suite_set.name, suite.name, test.class_name, test.name
                    )
                    fail_buff.write("FAILED: %s\n" % test_name, color=PrintColor.RED)
                    fail_buff.write("\t%s\n" % fname)
                    fail_buff.write("\n")
                    message = test.failure.message.split('\n')
                    for line in message:
                        fail_buff.write("\t%s\n" % line, color=PrintColor.LIGHT_GRAY)
                    fail_buff.write("\n")
                    failed_names.append(test_name)
                fail_buff.seek(0)
                print(fail_buff.read())
            raise FailedTests("%s failing tests." % len(failed_names))
        else:
            buff = TermBuffer()
            buff.write(
                "\n\t[ SUCCESS: %s/%s ]\n" % (total, total),
                color=PrintColor.GREEN
            )
            print(str(buff))

if __name__ == '__main__':
    if len(sys.argv) == 1:
        raise RuntimeError("usage: gtest_reporter.py /path/to/xml/output")
    Reporter(sys.argv[1]).run()
