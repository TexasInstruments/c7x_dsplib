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


def write_header_file(in0, out, testCase, headerFileName):
    testId = testCase["ID"]
    dTypeIn = testCase["dType"]
    numSamples = testCase["numSamples"]
    numChannels = testCase["numChannels"]

    # Output data type is the same as the input data type
    dTypeOut = dTypeIn

    # 64-bit integer literals need an explicit suffix in C
    suffix = {"int64_t": "ll", "uint64_t": "ull"}.get(dTypeIn, "")

    def fmt(val):
        return "{:>2}".format(val) + suffix

    attributesIn0 = '__attribute__((section(".staticData"))) static '
    attributesIn1 = '__attribute__((section(".staticData"))) static '
    attributesOut = '__attribute__((section(".staticData"))) static '

    outputDir = "../../DSPLIB_nonZeroCheck/test_data/"
    if not os.path.exists(outputDir):
        os.makedirs(outputDir)

    with open(outputDir + headerFileName, "w") as f:
        write_attributes(
            f,
            attributesIn0,
            dTypeIn + " " + "staticRefIn0Case" + str(testId) + "[]=\n",
        )
        for dimY in range(0, numChannels):
            for dimX in range(0, numSamples):
                print(fmt(in0[dimY, dimX]), file=f, end=", ")
            print(" ", file=f)
        f.write("\n};")
        f.write("\n\n")
        f.close()
        
    with open(outputDir + headerFileName, "a") as f:
        write_attributes(
            f,
            attributesOut,
            dTypeOut + " " + "staticRefOutCase" + str(testId) + "[]=\n",
        )
        for dimX in range(0, out.shape[0]):
            print(fmt(out[dimX]), file=f, end=", ")
        f.write("\n};")
        f.write("\n\n")
        f.close()

    if platform == "linux" or platform == "linux2":
        call(
            [
                "indent",
                "-nut",
                "-i3",
                # "-c55",
                "-l100",
                outputDir + headerFileName,
            ]
        )
        call(["rm", outputDir + headerFileName + "~"])  # remove backup file
