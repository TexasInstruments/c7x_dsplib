# Copyright (C) 2026 Texas Instruments Incorporated
#
# SPDX-License-Identifier: Apache-2.0

from __future__ import print_function
from subprocess import call
from sys import platform

import sys
import numpy as np
import csv

sys.path.append("../common")
import DSPLIB_utils
import file_io


# Map dTypeIn CSV string to DSPLIB enum string used in idat.c
DSPLIB_TYPE_MAP = {
    "float":   "DSPLIB_FLOAT32",
    "double":  "DSPLIB_FLOAT64",
    "int8_t":  "DSPLIB_INT8",
    "uint8_t": "DSPLIB_UINT8",
    "int16_t": "DSPLIB_INT16",
    "uint16_t":"DSPLIB_UINT16",
    "int32_t": "DSPLIB_INT32",
    "uint32_t":"DSPLIB_UINT32",
    "cfloat":  "DSPLIB_CFLOAT32",
    "cshort":  "DSPLIB_CINT16",
    "cint":    "DSPLIB_CINT32",
}

# For complex types the test data is stored as interleaved scalar pairs.
# The scalar dtype used in numpy for each dTypeIn:
NP_SCALAR_DTYPE = {
    "float":   "float32",
    "double":  "float64",
    "int8_t":  "int8",
    "uint8_t": "uint8",
    "int16_t": "int16",
    "uint16_t":"uint16",
    "int32_t": "int32",
    "uint32_t":"uint32",
    "cfloat":  "float32",
    "cshort":  "int16",
    "cint":    "int32",
}


def is_complex(dTypeIn):
    return dTypeIn in ("cfloat", "cshort", "cint")


def is_float(dTypeIn):
    return dTypeIn in ("float", "double", "cfloat")


def gen_random_data(dTypeIn, n_elements):
    """Generate n_elements of the given type as a flat numpy array.
    For complex types, returns 2*n_elements scalars (interleaved re/im).
    """
    np_dtype = NP_SCALAR_DTYPE[dTypeIn]

    if dTypeIn in ("float", "double", "cfloat"):
        raw = np.random.uniform(-10.0, 10.0,
                                size=(n_elements * 2 if is_complex(dTypeIn) else n_elements)
                                ).astype(np_dtype)
    elif dTypeIn[0] == "u":
        bits = int(np_dtype.replace("uint", ""))
        maxv = (2**bits) - 1
        n = n_elements * 2 if is_complex(dTypeIn) else n_elements
        raw = np.random.randint(0, maxv + 1, size=n, dtype=np_dtype)
    else:
        bits = int(np_dtype.replace("int", ""))
        half = 2 ** (bits - 1)
        n = n_elements * 2 if is_complex(dTypeIn) else n_elements
        raw = np.random.randint(-half, half, size=n, dtype=np_dtype)

    return raw


def complex_multiply(a_flat, b_flat, dTypeIn):
    """Compute element-wise complex multiply.
    a_flat: interleaved [re0, im0] (1 complex element = 2 scalars)
    b_flat: interleaved [re0, im0, re1, im1, ...] (N complex elements)
    Returns flat result of same length as b_flat.
    """
    np_dtype = NP_SCALAR_DTYPE[dTypeIn]

    a_re = a_flat[0]
    a_im = a_flat[1]

    b_re = b_flat[0::2]
    b_im = b_flat[1::2]

    if dTypeIn == "cfloat":
        out_re = (a_re * b_re - a_im * b_im).astype("float32")
        out_im = (a_re * b_im + a_im * b_re).astype("float32")
    elif dTypeIn == "cshort":
        # Q15 fixed-point complex multiply with rounding (matches __cmpy_fx)
        # __cmpy_fx: out = (a * b) >> 15, rounded to nearest
        a_re32 = np.int32(a_re)
        a_im32 = np.int32(a_im)
        b_re32 = b_re.astype("int32")
        b_im32 = b_im.astype("int32")
        out_re32 = a_re32 * b_re32 - a_im32 * b_im32
        out_im32 = a_re32 * b_im32 + a_im32 * b_re32
        # Round and right-shift by 15
        out_re = np.int16(np.clip((out_re32 + (1 << 14)) >> 15, -32768, 32767))
        out_im = np.int16(np.clip((out_im32 + (1 << 14)) >> 15, -32768, 32767))
    elif dTypeIn == "cint":
        # Q31 complex multiply matching __cmpy_fx / VCMPYR1WW:
        # out = (a * b + 0x40000000) >> 31, clamped to int32
        a_re64 = np.int64(a_re)
        a_im64 = np.int64(a_im)
        b_re64 = b_re.astype("int64")
        b_im64 = b_im.astype("int64")
        out_re32 = a_re64 * b_re64 - a_im64 * b_im64
        out_im32 = a_re64 * b_im64 + a_im64 * b_re64
        out_re = np.int32(np.clip((out_re32 + (1 << 30)) >> 31, -2147483648, 2147483647))
        out_im = np.int32(np.clip((out_im32 + (1 << 30)) >> 31, -2147483648, 2147483647))
    else:
        raise ValueError("Unknown complex type: " + dTypeIn)

    # Interleave re/im back
    out = np.empty(len(b_flat), dtype=np_dtype)
    out[0::2] = out_re
    out[1::2] = out_im
    return out


def compute_output(in1_flat, in2_flat, dTypeIn, length):
    """Compute the expected output: each element of in2 multiplied by the scalar in1."""
    if is_complex(dTypeIn):
        return complex_multiply(in1_flat, in2_flat, dTypeIn)
    else:
        a = in1_flat[0]
        b = in2_flat
        np_dtype = NP_SCALAR_DTYPE[dTypeIn]
        if is_float(dTypeIn):
            return (a * b).astype(np_dtype)
        else:
            return (a * b).astype(np_dtype)


def gen_test_case_header_file(testCase):
    testId  = testCase["ID"]
    dTypeIn = testCase["dTypeIn"]
    length  = testCase["length"]

    in1_flat = gen_random_data(dTypeIn, 1)       # scalar constant
    in2_flat = gen_random_data(dTypeIn, length)  # input vector

    out_flat = compute_output(in1_flat, in2_flat, dTypeIn, length)

    headerFileName = "staticRefCase" + str(testId) + ".h"
    file_io.write_header_file(in1_flat, in2_flat, out_flat, testCase, headerFileName)

    print("Test case", testId, "generated:", dTypeIn, "len =", length)
    return [np.min(out_flat), np.max(out_flat)]


def gen_testCaseParams(idatFile, currPrm):
    testId              = currPrm["ID"]
    dTypeIn             = currPrm["dTypeIn"]
    length              = currPrm["length"]
    testType            = currPrm["testType"]
    demoCase            = currPrm["demoCase"]
    numReps             = currPrm["numReps"]
    outputDataLocation  = currPrm["outputDataLocation"]
    dsplib_type         = DSPLIB_TYPE_MAP[dTypeIn]

    if testType == "STATIC":
        in1 = "staticRefIn1Case" + testId
        in2 = "staticRefIn2Case" + testId
        out = "staticRefOutCase" + testId
    else:
        in1 = "NULL"
        in2 = "NULL"
        out = "NULL"

    if demoCase == "true":
        idatFile.write(
            "#if (defined(ALL_TEST_CASES) || (TEST_CASE == %d) || defined(DEMO_CASE))\n"
            % int(testId)
        )
    else:
        idatFile.write(
            "#if (defined(ALL_TEST_CASES) || (TEST_CASE == %d))\n" % int(testId)
        )

    idatFile.write("{\n")
    idatFile.write("STATIC, // Test pattern\n")
    idatFile.write("%s, // Input 1 (constant scalar)\n" % in1)
    idatFile.write("%s, // Input 2 (vector)\n" % in2)
    idatFile.write("%s, // Output vector\n" % out)
    idatFile.write("%s, // Data type\n" % dsplib_type)
    idatFile.write("%s, // Data length\n" % length)
    idatFile.write("%s, // Output data location\n" % outputDataLocation)
    idatFile.write("%d, // Number of reps\n" % int(numReps))
    idatFile.write("%d, // Test ID\n" % int(testId))
    idatFile.write("},\n")
    idatFile.write("#endif\n\n")


def gen_idat_file(idatFile, testCases, testIdList):
    fIn = open("../common/ti_lice_header_idat.txt", "r")
    idatFile.writelines(fIn.readlines())
    fIn.close()

    idatFile.write('#include "DSPLIB_mulConstant_idat.h"\n')
    idatFile.write("/*This file is autogenerated*/\n")
    idatFile.write("/*Please do not edit*/\n\n\n")

    for testCase in testCases:
        if int(testCase["ID"]) in testIdList and testCase["testType"] == "STATIC":
            DSPLIB_utils.gen_ifdefs(idatFile, testCase)

    idatFile.write("static DSPLIB_mulConstant_testParams_t testParams[] =\n{\n")

    for testCase in testCases:
        if int(testCase["ID"]) in testIdList:
            gen_testCaseParams(idatFile, testCase)

    idatFile.write("};\n\n")

    fIn = open("test_param_function.txt", "r")
    idatFile.writelines(fIn.readlines())
    fIn.close()


def process_test_case(testCase):
    testCase["testId"] = int(testCase["ID"])
    testCase["length"] = int(testCase["length"])
    return testCase


def gen_test_case(testCase):
    if testCase["testType"] == "STATIC":
        gen_test_case_header_file(testCase)
    else:
        print("Skipping RANDOM test case", testCase["ID"])


# -----------------------------------------------------------------------
testCasesCsvFile = "test_cases_list.csv"
testIdList = list(range(1, 999))

myArgs = DSPLIB_utils.getCmdLineArgs()

with open(testCasesCsvFile, encoding="utf-8-sig") as csv_file:
    testCaseReader = csv.DictReader(csv_file)
    testCases = list(testCaseReader)

for testCase in testCases:
    testCase = process_test_case(testCase)

for testCase in testCases:
    if myArgs.allCases or int(testCase["ID"]) in testIdList:
        gen_test_case(testCase)

idatFileName = "../../DSPLIB_mulConstant/DSPLIB_mulConstant_idat.c"

with open(idatFileName, "w") as idatFile:
    gen_idat_file(idatFile, testCases, testIdList)

if platform == "linux" or platform == "linux2":
    call(["clang-format", "-style=file", "-i", idatFileName])
    call(["indent", "-nut", "-i3", "-c55", "-l200", idatFileName])
    call(["rm", "-f", idatFileName + "~"])

print("Idat file generation completed")
