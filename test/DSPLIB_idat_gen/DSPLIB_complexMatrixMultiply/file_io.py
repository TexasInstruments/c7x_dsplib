# Copyright (C) 2026 Texas Instruments Incorporated
#
# SPDX-License-Identifier: Apache-2.0

from __future__ import print_function

import numpy as np


def saturate(x, T):
    return np.clip(x, np.iinfo(T).min, np.iinfo(T).max).astype(T)


def write_attributes(f, attributes, stringToWrite):
    f.write("#ifdef WIN32\n")
    f.write(stringToWrite)
    f.write("#else\n")
    f.write(attributes + stringToWrite)
    f.write("#endif\n")
    f.write("{\n")


def write_test_data(fd, d, name, dType):
    fd.write(f"// Shape: {d.shape}\n")
    write_attributes(
        fd,
        '__attribute__((section(".staticData"))) \nstatic ',
        f"{dType} {name} [] =\n",
    )
    sz = np.shape(d)
    is_complex = np.iscomplexobj(d)
    if len(sz) == 3:
        for dimZ in range(sz[0]):
            for dimY in range(sz[1]):
                for dimX in range(sz[2]):
                    if is_complex:
                        print(
                            "{:>2}, {:>2}".format(
                                np.real(d[dimZ, dimY, dimX]).astype(np.int64),
                                np.imag(d[dimZ, dimY, dimX]).astype(np.int64),
                            ),
                            file=fd,
                            end=", ",
                        )
                    else:
                        print("{:>2}".format(d[dimZ, dimY, dimX]), file=fd, end=", ")
                print("", file=fd)
            print("", file=fd)
        fd.write("};")
        fd.write("\n\n")
    elif len(sz) == 2:
        for dimY in range(sz[0]):
            for dimX in range(sz[1]):
                if is_complex:
                    print(
                        "{:>2}, {:>2}".format(
                            np.real(d[dimY, dimX]).astype(np.int64),
                            np.imag(d[dimY, dimX]).astype(np.int64),
                        ),
                        file=fd,
                        end=", ",
                    )
                else:
                    print("{:>2}".format(d[dimY, dimX]), file=fd, end=", ")
            print(" ", file=fd)
        fd.write("\n};")
        fd.write("\n\n")
    elif len(sz) == 1:
        for dimX in range(sz[0]):
            if is_complex:
                print(
                    "{:>2}, {:>2}".format(
                        np.real(d[dimX]).astype(np.int64),
                        np.imag(d[dimX]).astype(np.int64),
                    ),
                    file=fd,
                    end=", ",
                )
            else:
                print("{:>2}".format(d[dimX]), file=fd, end=", ")
        fd.write("\n};")
        fd.write("\n\n")


def write_params(fd, d, name, dType):
    write_attributes(
        fd,
        '__attribute__((section(".staticData"))) \nstatic ',
        f"{dType} {name} =\n",
    )
    for ln in d:
        fd.write(f"{ln}\n")
    fd.write("};")
    fd.write("\n\n")
