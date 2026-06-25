# Copyright (C) 2026 Texas Instruments Incorporated
#
# SPDX-License-Identifier: Apache-2.0

from __future__ import print_function
from subprocess import call
from sys import platform

import numpy as np


def write_attributes(f, attributes, stringToWrite):
    f.write("#ifdef WIN32\n")
    f.write(stringToWrite)
    f.write("#else\n")
    f.write(attributes + stringToWrite)
    f.write("#endif\n")
    f.write("{\n")


def write_header_file(inp, out, testCase, headerFileName):
    """
    Write DSPLIB matTrans header file.

    Conventions:
      dimX = width  (number of columns)
      dimY = height (number of rows)

      Input matrix shape  = (dimY, dimX)
      Output matrix shape = (dimX, dimY)
    """

    testId = testCase["ID"]
    dimX = int(testCase["dimX"])  # width  (columns)
    dimY = int(testCase["dimY"])  # height (rows)
    dType = testCase["dType"]

    attributesIn = '__attribute__((section(".staticData"))) static '
    attributesOut = '__attribute__((section(".staticData"))) static '

    outputDir = "../../DSPLIB_matTrans/test_data/"

    in_rows, in_cols = inp.shape  # expected: (dimY, dimX)
    out_rows, out_cols = out.shape  # expected: (dimX, dimY)

    assert inp.shape == (
        dimY,
        dimX,
    ), f"Input matrix shape mismatch: expected {(dimY, dimX)}, got {inp.shape}"

    assert out.shape == (
        dimX,
        dimY,
    ), f"Output matrix shape mismatch: expected {(dimX, dimY)}, got {out.shape}"

    with open(outputDir + headerFileName, "w") as f:
        write_attributes(
            f,
            attributesIn,
            f"{dType} staticRefInCase{testId}[]=\n",
        )

        for y in range(in_rows):  # height
            for x in range(in_cols):  # width
                print("{:>2}".format(inp[y, x]), file=f, end=", ")
            print(" ", file=f)

        f.write("\n};\n\n")

    with open(outputDir + headerFileName, "a") as f:
        write_attributes(
            f,
            attributesOut,
            f"{dType} staticRefOutCase{testId}[]=\n",
        )

        for y in range(out_rows):  # width
            for x in range(out_cols):  # height
                print("{:>2}".format(out[y, x]), file=f, end=", ")
            print(" ", file=f)

        f.write("\n};\n\n")

    if platform == "linux" or platform == "linux2":
        call(
            [
                "indent",
                "-nut",
                "-i3",
                "-l100",
                outputDir + headerFileName,
            ]
        )
        call(["rm", outputDir + headerFileName + "~"])
