from __future__ import print_function
from subprocess import call
from sys import platform

import sys
import numpy as np
import csv
import os

def write_attributes(f, attributes, stringToWrite):
    """Write platform-specific attributes for section and alignment."""
    f.write("#ifdef WIN32\n")
    f.write(stringToWrite)
    f.write("#else\n")
    f.write(attributes + stringToWrite)
    f.write("#endif\n")
    f.write("{\n")


def c_literal(value, dType):
    """Format one array element as a C literal."""
    if dType == "int64_t" and value == np.iinfo(np.int64).min:
        # -9223372036854775808 is not a valid C literal (the positive part overflows)
        return "(-9223372036854775807LL - 1)"
    return "{:>5}".format(value)


def write_header_file(inData, outData, testCase, headerFileName):
    """Write input and output test data to a C header file."""
    testId = testCase["ID"]
    width = testCase["width"]
    height = testCase["height"]
    dTypeIn = testCase["dType"]

    attributesIn = '__attribute__((section(".staticData"))) static '
    attributesOut = '__attribute__((section(".staticData"))) static '

    outputDir = "../../DSPLIB_absoluteValue/test_data/"

    if not os.path.exists(outputDir):
        os.makedirs(outputDir)

    with open(outputDir + headerFileName, "w") as f:
        write_attributes(
            f,
            attributesIn,
            dTypeIn + " " + "staticRefInCase" + str(testId) + "[]=\n",
        )
        for row in range(height):
            for col in range(width):
                print(c_literal(inData[row, col], dTypeIn), file=f, end=", ")
            print(" ", file=f)

        f.write("\n};")
        f.write("\n\n")
        f.close()

    with open(outputDir + headerFileName, "a") as f:
        write_attributes(
            f,
            attributesOut,
            dTypeIn + " " + "staticRefOutCase" + str(testId) + "[]=\n",
        )

        for row in range(height):
            for col in range(width):
                print(c_literal(outData[row, col], dTypeIn), file=f, end=", ")
            print(" ", file=f)

        f.write("\n};")
        f.write("\n\n")
        f.close()
