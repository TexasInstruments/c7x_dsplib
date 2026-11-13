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


def write_array(f, attributes, dType, name, data):
    write_attributes(f, attributes, dType + " " + name + "[]=\n")
    for dimY in range(0, data.shape[0]):
        for dimX in range(0, data.shape[1]):
            print("{:>2}".format(data[dimY, dimX]), file=f, end=", ")
        print(" ", file=f)
    f.write("\n};")
    f.write("\n\n")


def write_header_file(in0, in1, out, testCase, headerFileName):
    testId = testCase["ID"]
    dType = testCase["dType"]

    attributes = '__attribute__((section(".staticData"))) static '

    outputDir = "../../DSPLIB_tableSourceRowColSelector/test_data/"

    if not os.path.exists(outputDir):
        os.mkdir(outputDir)

    with open(outputDir + headerFileName, "w") as f:
        write_array(f, attributes, "int32_t", "staticRefIn0Case" + str(testId), in0.reshape(1, -1))
        write_array(f, attributes, dType, "staticRefIn1Case" + str(testId), in1)
        write_array(f, attributes, dType, "staticRefOutCase" + str(testId), out)
        f.close()
