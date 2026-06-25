# Copyright (C) 2026 Texas Instruments Incorporated
#
# SPDX-License-Identifier: Apache-2.0

from __future__ import print_function
from subprocess import call
from sys import platform

import sys
import numpy as np
import csv
import argparse


def gen_ifdefs(idatFile, testCase):
    """Generate C++ ifdefs macros for a given test case

    Args:
        param1 (file object): Input file object to idat.c file
        param2 (dict object): Dict object with test case parameter

    """
    testId = testCase["ID"]
    demoCase = testCase["demoCase"]
    if "testCategory" in testCase:
        testCategory = testCase["testCategory"]

    headerFileName = "staticRefCase" + str(testId) + ".h"

    if demoCase == "true":
        idatFile.write(
            "#if (defined(ALL_TEST_CASES) || (TEST_CASE == %d) || defined(DEMO_CASE))\n"
            % (int(testId))
        )
    else:
        if "testCategory" in testCase:
            idatFile.write(
                "#if (defined(ALL_TEST_CASES) || (TEST_CASE == %d) || (TEST_CATEGORY == %d))\n"
                % (int(testId), int(testCategory))
            )
        else:
            idatFile.write(
                "#if (defined(ALL_TEST_CASES) || (TEST_CASE == %d))\n" % (int(testId))
            )

    idatFile.write('#include "test_data/%s"\n' % (headerFileName))
    idatFile.write("#endif\n\n")


def shiftAndRound(inVal, shift):
    np.right_shift(inVal, shift - 1, out=inVal, where=shift != 0)
    np.add(inVal, 1, out=inVal, where=shift != 0)
    np.right_shift(inVal, 1, out=inVal, where=shift != 0)
    return inVal


def resolve_dType(dType):
    DSPLIB_dType = ""
    if dType[0] == "i":
        if dType[3] == "8":
            DSPLIB_dType = "DSPLIB_INT" + dType[3]
        else:
            DSPLIB_dType = "DSPLIB_INT" + dType[3] + dType[4]
    elif dType[0] == "u":
        if dType[4] == "8":
            DSPLIB_dType = "DSPLIB_UINT" + dType[4]
        else:
            DSPLIB_dType = "DSPLIB_UINT" + dType[4] + dType[5]
    elif dType[0] == "f":
        DSPLIB_dType = "DSPLIB_FLOAT32"
    elif dType[0] == "d":
        DSPLIB_dType = "DSPLIB_FLOAT64"

    return DSPLIB_dType


def minMaxVal(dType):
    if dType[0] == "i" or dType[0] == "u":
        [minVal, maxVal] = [np.finfo("float32").min, np.finfo("float32").max]
    elif dType[0] == "f":
        [minVal, maxVal] = [np.finfo("float32").min, np.finfo("float32").max]
    elif dType[0] == "d":
        [minVal, maxVal] = [np.finfo("double").min, np.finfo("double").max]

    return [minVal, maxVal]


def getCmdLineArgs():

    parser = argparse.ArgumentParser()

    parser.add_argument(
        "-a",
        action="store_true",
        default=False,
        dest="allCases",
        help="Set to generate all cases",
    )

    myArgs = parser.parse_args()

    return myArgs
