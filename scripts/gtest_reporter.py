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



def _iter_suite_sets(xml_dir):
    for fname in os.listdir(xml_dir):
        fpath = os.path.join(xml_dir, fname)
        with open(fpath) as f:
            yield fname, GTestSuiteSet.parse(f.read())


class FailedTests(RuntimeError):
    pass


class Reporter(object):
    def __init__(self, xml_dir):
        self.xml_dir = xml_dir

    def run(self):
        total = 0
        failures = 0
        for suite_name, suite_set in _iter_suite_sets(self.xml_dir):
            msg = "%s: %s/%s"
            current_total = suite_set.total_test_count()
            success = current_total - suite_set.stats.n_failures
            total += current_total
            failures += suite_set.stats.n_failures
            print(msg % (suite_name, success, current_total))
        print("")
        print("OVERALL: %s/%s" % (total - failures, total))
        if failures > 0:
            raise FailedTests

if __name__ == '__main__':
    if len(sys.argv) == 1:
        raise RuntimeError("usage: gtest_reporter.py /path/to/xml/output")
    Reporter(sys.argv[1]).run()
