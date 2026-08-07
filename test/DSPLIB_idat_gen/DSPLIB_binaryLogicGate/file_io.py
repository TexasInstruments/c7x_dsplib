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


def write_header_file(in0, in1, out, testCase, headerFileName):
    testId = testCase["ID"]
    dTypeIn = testCase["dType"]
    numSamples = testCase["numSamples"]
    numChannels = testCase["numChannels"]

    # Determine output data type based on input type
    if dTypeIn == "int8_t" or dTypeIn == "uint8_t":
        dTypeOut = "uint8_t"
    elif dTypeIn == "int16_t" or dTypeIn == "uint16_t":
        dTypeOut = "uint16_t"
    elif dTypeIn == "int32_t" or dTypeIn == "uint32_t" or dTypeIn == "float":
        dTypeOut = "uint32_t"
    else:
        dTypeOut = "uint64_t"

    attributesIn0 = '__attribute__((section(".staticData"))) static '
    attributesIn1 = '__attribute__((section(".staticData"))) static '
    attributesOut = '__attribute__((section(".staticData"))) static '

    outputDir = "../../DSPLIB_binaryLogicGate/test_data/"
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
                print("{:>2}".format(in0[dimY, dimX]), file=f, end=", ")
            print(" ", file=f)
        f.write("\n};")
        f.write("\n\n")
        f.close()
        
    with open(outputDir + headerFileName, "a") as f:
        write_attributes(
            f,
            attributesIn1,
            dTypeIn + " " + "staticRefIn1Case" + str(testId) + "[]=\n",
        )
        for dimY in range(0, numChannels):
            for dimX in range(0, numSamples):
                print("{:>2}".format(in1[dimY, dimX]), file=f, end=", ")
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
        for dimY in range(0, numChannels):
            for dimX in range(0, numSamples):
                print("{:>2}".format(out[dimY, dimX]), file=f, end=", ")
            print(" ", file=f)
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
