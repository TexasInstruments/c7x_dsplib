# Copyright (C) 2026 Texas Instruments Incorporated
#
# SPDX-License-Identifier: Apache-2.0

import numpy as np


def tableSourceRowColSelector(index, table, isColSelect, isOutputInterleave):
    """Golden model of DSPLIB_tableSourceRowColSelector.

    Each index selects one vector of the row-major table: a row, or a column
    when isColSelect is set. Indices are clamped to the valid range of the selection.

    Args:
        index (np.ndarray): [numIndices] int32 indices, one selected vector per index
        table (np.ndarray): [numRows, numCols] row-major table
        isColSelect (int): flag, set when an index selects a table column instead of a table row
        isOutputInterleave (int): flag, set for interleaved output instead of non-interleaved

    Returns:
        np.ndarray: interleaved     -> [selLen, numIndices], column i is the vector selected by index[i]
                    non-interleaved -> [numIndices, selLen], row i is the vector selected by index[i]

    """
    numRows, numCols = table.shape
    if isColSelect == 0:
        sel = np.clip(index.astype(np.int64), 0, numRows - 1)
        vecs = table[sel, :]
    elif isColSelect == 1:
        sel = np.clip(index.astype(np.int64), 0, numCols - 1)
        vecs = table[:, sel].T
    else:
        raise ValueError("invalid isColSelect " + str(isColSelect))

    if isOutputInterleave == 1:
        return vecs.T.copy()
    elif isOutputInterleave == 0:
        return vecs.copy()
    raise ValueError("invalid isOutputInterleave " + str(isOutputInterleave))
