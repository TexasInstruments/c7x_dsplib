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


def write_header_file(X, Y, testCase, headerFileName):
    testId = testCase["ID"]
    dType = testCase["dType"]

    attributesX = '__attribute__((section(".staticData"))) static '
    attributesY = '__attribute__((section(".staticData"))) static '

    outputDir = "../../DSPLIB_mat_submat_copy/test_data/"

    # A Matrix
    with open(outputDir + headerFileName, "w") as f:
        f.write("// X Matrix\n")
        write_attributes(
            f,
            attributesX,
            dType + " " + "staticRefXCase" + str(testId) + "[]=\n",
        )
        for dimY in range(0, X.shape[0]):
            for dimX in range(0, X.shape[1]):
                print("{:>2}".format(X[dimY, dimX]), file=f, end=", ")
            print(" ", file=f)
        f.write("\n};")
        f.write("\n\n")
        f.close()
    # Y Vector
    with open(outputDir + headerFileName, "a") as f:
        f.write("// Y vector\n")
        write_attributes(
            f,
            attributesY,
            dType + " " + "staticRefYCase" + str(testId) + "[]=\n",
        )
        for dimY in range(0, Y.shape[0]):
            for dimX in range(0, Y.shape[1]):
                print("{:>2}".format(Y[dimY, dimX]), file=f, end=", ")
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
