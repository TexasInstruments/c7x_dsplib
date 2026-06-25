# Copyright (C) 2026 Texas Instruments Incorporated
#
# SPDX-License-Identifier: Apache-2.0

import itertools
import csv

dataSize_values = [32, 64, 128, 256, 512, 1024]
filterSize_values = [1, 8, 16, 32]
batchSize_values = [1, 4, 8]
enableNChCoefs_values = [0, 1]
enableMMA_values = [0, 1]
Q_values = [23]

with open("./test_cases_list.csv", "w", newline="") as csvfile:
    fieldnames = [
        "ID",
        "testType",
        "dataSize",
        "dTypeIn",
        "filterSize",
        "dTypeFilter",
        "dTypeOut",
        "batchSize",
        "enableNChCoefs",
        "enableMMA",
        "enableQ",
        "Q",
        "numReps",
        "demoCase",
        "outputDataLocation",
    ]
    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)

    writer.writeheader()
    id = 1
    for dataSize, filterSize in [
        (ds, fs) for ds in dataSize_values for fs in filterSize_values if ds > fs
    ]:
        for batchSize in batchSize_values:
            for enableNChCoefs in enableNChCoefs_values:
                for enableMMA in enableMMA_values:
                    for Q in Q_values:
                        enableQ = enableMMA
                        writer.writerow(
                            {
                                "ID": id,
                                "testType": "STATIC",
                                "dataSize": dataSize,
                                "dTypeIn": "float",
                                "filterSize": filterSize,
                                "dTypeFilter": "float",
                                "dTypeOut": "float",
                                "batchSize": batchSize,
                                "enableNChCoefs": enableNChCoefs,
                                "enableMMA": enableMMA,
                                "enableQ": enableQ,
                                "Q": Q,
                                "numReps": 1,
                                "demoCase": "False",
                                "outputDataLocation": "DSPLIB_TEST_OUTPUT_HEAP",
                            }
                        )
                        id += 1
