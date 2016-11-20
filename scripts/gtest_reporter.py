#!/usr/bin/env python

import lxml.etree
from collections import namedtuple
import unittest
import sys

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
    for arg in sequence:
        fn(arg)


GTEST_XML = """
<?xml version="1.0" encoding="UTF-8"?>
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


class TestFailure(object):
    def __init__(self, message):
        assert_texty(message)
        self.message = message

class TestCase(object):
    def __init__(self, name, class_name, status, duration, failure=None):
        doeach(assert_texty, name, class_name)
        assert_texty(status)
        assert_numeric(duration)
        if failure is not None:
            assert isinstance(failure, TestFailure)
        self.name = name
        self.class_name = class_name
        self.status = status
        self.duration = duration
        self.failure = failure

    def succeeded(self):
        return not self.failed()

    def failed(self):
        return self.failure is not None


class TestSuiteStats(object):
    def __init__(self, n_failures, n_errors, n_skipped, duration):
        doeach(assert_numeric, n_failures, n_errors, n_skipped, duration)
        self.n_failures = n_failures
        self.n_errors = n_errors
        self.n_skipped = n_skipped
        self.duration = duration

class TestSuite(object):
    def __init__(self, name, stats, test_cases=None):
        test_cases = test_cases or []
        for item in test_cases:
            assert isinstance(item, TestCase)
        assert_texty(name)
        assert isinstance(stats, TestSuiteStats)
        self.name = name
        self.outcomes = outcomes
        self.test_cases = test_cases

    def add_test_case (self, test):
        assert isinstance(test, TestCase)
        self.test_cases.append(test)

