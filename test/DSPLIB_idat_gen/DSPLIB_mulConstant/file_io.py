# Copyright (C) 2026 Texas Instruments Incorporated
#
# SPDX-License-Identifier: Apache-2.0

from __future__ import print_function
from subprocess import call
from sys import platform

import sys
import numpy as np


def _write_array_section(f, attr, c_type, array_name, data):
    f.write("#ifdef WIN32\n")
    f.write(c_type + " " + array_name + "[]=\n")
    f.write("#else\n")
    f.write(attr + c_type + " " + array_name + "[]=\n")
    f.write("#endif\n")
    f.write("{\n")
    for val in np.asarray(data).flat:
        _write_val(f, val, c_type)
    f.write("\n};\n\n")


def _write_val(f, val, c_type):
    if c_type == "double":
        print("{:.17g}".format(float(val)), file=f, end=", ")
    elif c_type == "float":
        print("{:.9g}".format(float(val)), file=f, end=", ")
    else:
        print("{}".format(int(val)), file=f, end=", ")


def _c_scalar_type(dTypeIn):
    """Return the C scalar base type to use in array declarations."""
    mapping = {
        "float":    "float",
        "double":   "double",
        "int8_t":   "int8_t",
        "uint8_t":  "uint8_t",
        "int16_t":  "int16_t",
        "uint16_t": "uint16_t",
        "int32_t":  "int32_t",
        "uint32_t": "uint32_t",
        # complex: stored as interleaved scalar pairs
        "cfloat":   "float",
        "cshort":   "int16_t",
        "cint":     "int32_t",
    }
    return mapping.get(dTypeIn, "int32_t")


def write_header_file(in1_scalar, in2_vec, out_vec, testCase, headerFileName):
    """Write staticRefIn1, staticRefIn2, staticRefOut arrays to a header file.

    in1_scalar: flat numpy array for the constant (1 real or 2 scalars for complex)
    in2_vec:    flat numpy array for the input vector
    out_vec:    flat numpy array for the expected output
    """
    testId  = testCase["ID"]
    dTypeIn = testCase["dTypeIn"]

    outputDir = "../../DSPLIB_mulConstant/test_data/"
    attr      = '__attribute__((section(".staticData"))) static '
    c_t       = _c_scalar_type(dTypeIn)

    with open(outputDir + headerFileName, "w") as f:
        _write_array_section(f, attr, c_t, "staticRefIn1Case" + str(testId), in1_scalar)
        _write_array_section(f, attr, c_t, "staticRefIn2Case" + str(testId), in2_vec)
        _write_array_section(f, attr, c_t, "staticRefOutCase" + str(testId), out_vec)

    if platform == "linux" or platform == "linux2":
        call(["indent", "-nut", "-i3", "-l120", outputDir + headerFileName])
        call(["rm", "-f", outputDir + headerFileName + "~"])
