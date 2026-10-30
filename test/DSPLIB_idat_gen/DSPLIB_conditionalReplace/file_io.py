# Copyright (C) 2026 Texas Instruments Incorporated
#
# SPDX-License-Identifier: Apache-2.0

from __future__ import print_function
from subprocess import call
from sys import platform

import sys
import numpy as np
import csv
import os


def write_attributes(f, attributes, stringToWrite):
    f.write("#ifdef WIN32\n")
    f.write(stringToWrite)
    f.write("#else\n")
    f.write(attributes + stringToWrite)
    f.write("#endif\n")
    f.write("{\n")


def fmt_elem(x, dType):
    """Format a numpy scalar as a C literal for the given C data type"""
    if dType in ("float", "double"):
        if np.isnan(x):
            return "NAN"
        if np.isinf(x):
            return "INFINITY" if x > 0 else "-INFINITY"
        if dType == "float":
            return repr(float(np.float32(x))) + "f"
        return repr(float(x))
    if dType == "uint64_t":
        return str(int(x)) + "ULL"
    if dType == "int64_t":
        if int(x) == np.iinfo(np.int64).min:
            return "(-9223372036854775807LL-1)"  # the literal 9223372036854775808 does not fit in int64_t
        return str(int(x)) + "LL"
    return str(int(x))


def write_matrix(f, attributes, dType, name, mat, numChannels, numSamples):
    write_attributes(f, attributes, dType + " " + name + "[]=\n")
    for dimY in range(0, numChannels):
        for dimX in range(0, numSamples):
            print(fmt_elem(mat[dimY, dimX], dType), file=f, end=", ")
        print(" ", file=f)
    f.write("\n};")
    f.write("\n\n")


def write_header_file(in0, compareType, constValue, replaceValue, out, testCase, headerFileName):
    testId = testCase["ID"]
    dType = testCase["dType"]
    numSamples = testCase["numSamples"]
    numChannels = testCase["numChannels"]

    attributes = '__attribute__((section(".staticData"))) static '

    outputDir = "../../DSPLIB_conditionalReplace/test_data/"
    if not os.path.exists(outputDir):
        os.makedirs(outputDir)

    with open(outputDir + headerFileName, "w") as f:
        write_matrix(f, attributes, dType, "staticRefInCase" + str(testId), in0, numChannels, numSamples)
        # Per-channel parameters, one entry per channel (row)
        # compare types are always int32_t, constants and replacement values have the data type of the test case
        write_matrix(f, attributes, "int32_t", "staticRefCompareTypeCase" + str(testId), compareType[None, :], 1, numChannels)
        write_matrix(f, attributes, dType, "staticRefConstValueCase" + str(testId), constValue[None, :], 1, numChannels)
        write_matrix(f, attributes, dType, "staticRefReplaceValueCase" + str(testId), replaceValue[None, :], 1, numChannels)
        write_matrix(f, attributes, dType, "staticRefOutCase" + str(testId), out, numChannels, numSamples)

    if platform == "linux" or platform == "linux2":
        call(["indent", "-nut", "-i3", "-l100", outputDir + headerFileName])
        call(["rm", "-f", outputDir + headerFileName + "~"])  # remove backup file
