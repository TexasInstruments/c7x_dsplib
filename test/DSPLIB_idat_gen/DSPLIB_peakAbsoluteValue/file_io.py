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


def write_header_file(inData, outData, testCase, headerFileName):
    testId = testCase["ID"]
    channels = testCase["channels"]
    length = testCase["length"]
    dTypeIn = testCase["dTypeIn"]

    attributesIn = '__attribute__((section(".staticData"))) static '
    attributesOut = '__attribute__((section(".staticData"))) static '

    outputDir = "../../DSPLIB_peakAbsoluteValue/test_data/"

    if not os.path.exists(outputDir):
        os.mkdir(outputDir)

    with open(outputDir + headerFileName, "w") as f:
        write_attributes(
            f,
            attributesIn,
            dTypeIn + " " + "staticRefInCase" + str(testId) + "[]=\n",
        )
        # The input is stored packed (channels rows of length elements); the test pads the rows to the stride
        for dimY in range(0, channels):
            for dimX in range(0, length):
                if dTypeIn == "int64_t" and inData[dimY][dimX] == np.iinfo(np.int64).min:
                    # -9223372036854775808 is not a valid C literal (the positive part overflows)
                    print("(-9223372036854775807LL - 1)", file=f, end=", ")
                else:
                    print("{:>5}".format(inData[dimY][dimX]), file=f, end=", ")
        f.write("\n};")
        f.write("\n\n")
        f.close()

    with open(outputDir + headerFileName, "a") as f:
        write_attributes(
            f,
            attributesOut,
            dTypeIn + " " + "staticRefOutCase" + str(testId) + "[]=\n",
        )
        # One output value per channel
        for dimY in range(0, channels):
            print("{:>5}".format(outData[dimY]), file=f, end=", ")
        f.write("\n};")
        f.write("\n\n")
        f.close()

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
        call(["rm", outputDir + headerFileName + "~"])  # remove backup file
