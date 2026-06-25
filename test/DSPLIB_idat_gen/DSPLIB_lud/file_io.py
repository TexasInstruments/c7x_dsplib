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


def write_header_file(in0, in1, out0, out1, out2, out3, out4, testCase, headerFileName):
    testId = testCase["ID"]
    dType = testCase["dType"]

    attributesIn0 = '__attribute__((section(".staticData"))) static '
    attributesOut = '__attribute__((section(".staticData"))) static '

    outputDir = "../../DSPLIB_lud/test_data/"

    # A Matrix
    with open(outputDir + headerFileName, "w") as f:
        f.write("// A Matrix\n")
        write_attributes(
            f,
            attributesIn0,
            dType + " " + "staticRefInACase" + str(testId) + "[]=\n",
        )
        for dimY in range(0, in0.shape[0]):
            for dimX in range(0, in0.shape[1]):
                print("{:>.4f}".format(in0[dimY, dimX]), file=f, end=", ")
            print(" ", file=f)
        f.write("\n};")
        f.write("\n\n")
        f.close()
    # B Vector
    with open(outputDir + headerFileName, "a") as f:
        f.write("// B vector\n")
        write_attributes(
            f,
            attributesIn0,
            dType + " " + "staticRefInBCase" + str(testId) + "[]=\n",
        )
        for dimY in range(0, in1.shape[0]):
            print("{:>.4f}".format(in1[dimY]), file=f, end=", ")
        f.write("\n};")
        f.write("\n\n")
        f.close()

    # P Matrix
    with open(outputDir + headerFileName, "a") as f:
        f.write("// P Matrix\n")
        write_attributes(
            f,
            attributesOut,
            "unsigned short" + " " + "staticRefOutPCase" + str(testId) + "[]=\n",
        )
        for dimY in range(0, out0.shape[0]):
            for dimX in range(0, out0.shape[1]):
                print("{:>2}".format(int(out0[dimY, dimX])), file=f, end=", ")
            print(" ", file=f)
        f.write("\n};")
        f.write("\n\n")
        f.close()

    # L Matrix
    with open(outputDir + headerFileName, "a") as f:
        f.write("// L Matrix\n")
        write_attributes(
            f,
            attributesOut,
            dType + " " + "staticRefOutLCase" + str(testId) + "[]=\n",
        )
        for dimY in range(0, out1.shape[0]):
            for dimX in range(0, out0.shape[1]):
                print("{:>.4f}".format(out1[dimY, dimX]), file=f, end=", ")
            print(" ", file=f)
        f.write("\n};")
        f.write("\n\n")
        f.close()

    # U Matrix
    with open(outputDir + headerFileName, "a") as f:
        f.write("// U Matrix\n")
        write_attributes(
            f,
            attributesOut,
            dType + " " + "staticRefOutUCase" + str(testId) + "[]=\n",
        )

        for dimY in range(0, out2.shape[0]):
            for dimX in range(0, out2.shape[1]):
                print("{:>.4f}".format(out2[dimY, dimX]), file=f, end=", ")
            print(" ", file=f)
        f.write("\n};")
        f.write("\n\n")
        f.close()
    # X Vector
    with open(outputDir + headerFileName, "a") as f:
        f.write("// X vector\n")
        write_attributes(
            f,
            attributesOut,
            dType + " " + "staticRefOutXCase" + str(testId) + "[]=\n",
        )
        for dimY in range(0, out3.shape[0]):
            print("{:>.4f}".format(out3[dimY]), file=f, end=", ")
        f.write("\n};")
        f.write("\n\n")
        f.close()
    # invA Matrix
    with open(outputDir + headerFileName, "a") as f:
        f.write("// invA Matrix\n")
        write_attributes(
            f,
            attributesOut,
            dType + " " + "staticRefOutInvACase" + str(testId) + "[]=\n",
        )
        for dimY in range(0, out4.shape[0]):
            for dimX in range(0, out4.shape[1]):
                print("{:>.4f}".format(out4[dimY, dimX]), file=f, end=", ")
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
