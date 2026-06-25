# Copyright (C) 2026 Texas Instruments Incorporated
#
# SPDX-License-Identifier: Apache-2.0

from __future__ import print_function
from subprocess import call
from sys import platform

import sys
import numpy as np
import csv


def write_attributes(f, attributes, stringToWrite):
    f.write("#ifdef WIN32\n")
    f.write(stringToWrite)
    f.write("#else\n")
    f.write(attributes + stringToWrite)
    f.write("#endif\n")
    f.write("{\n")


def write_header_file(in0, out0, out1, out2, testCase, headerFileName):
    testId = testCase["ID"]
    dType = testCase["dType"]

    attributesIn0 = '__attribute__((section(".staticData"))) static '
    attributesOut = '__attribute__((section(".staticData"))) static '

    outputDir = "../../DSPLIB_svd/test_data/"

    # A Matrix
    with open(outputDir + headerFileName, "w") as f:
        f.write("// A Matrix\n")
        write_attributes(
            f,
            attributesIn0,
            dType + " " + "staticRefInCase" + str(testId) + "[]=\n",
        )
        for dimY in range(0, in0.shape[0]):
            for dimX in range(0, in0.shape[1]):
                print("{:>2}".format(in0[dimY, dimX]), file=f, end=", ")
            print(" ", file=f)
        f.write("\n};")
        f.write("\n\n")
        f.close()

    # U Matrix
    with open(outputDir + headerFileName, "a") as f:
        f.write("// U Matrix\n")
        write_attributes(
            f,
            attributesIn0,
            dType + " " + "staticRefOutUCase" + str(testId) + "[]=\n",
        )
        for dimY in range(0, out0.shape[0]):
            for dimX in range(0, out0.shape[1]):
                print("{:>2}".format(out0[dimY, dimX]), file=f, end=", ")
            print(" ", file=f)
        f.write("\n};")
        f.write("\n\n")
        f.close()

    # Diag Matrix
    with open(outputDir + headerFileName, "a") as f:
        f.write("// Diag Matrix\n")
        write_attributes(
            f,
            attributesOut,
            dType + " " + "staticRefOutDiagCase" + str(testId) + "[]=\n",
        )
        for dimY in range(0, out1.shape[0]):
            print("{:>2}".format(out1[dimY]), file=f, end=", ")
        f.write("\n};")
        f.write("\n\n")
        f.close()

    # V Matrix
    with open(outputDir + headerFileName, "a") as f:
        write_attributes(
            f,
            attributesOut,
            dType + " " + "staticRefOutVCase" + str(testId) + "[]=\n",
        )

        for dimY in range(0, out2.shape[0]):
            for dimX in range(0, out2.shape[1]):
                print("{:>2}".format(out2[dimY, dimX]), file=f, end=", ")
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
