# Copyright (C) 2026 Texas Instruments Incorporated
#
# SPDX-License-Identifier: Apache-2.0

import numpy as np


class DSPLIB_matTrans:
    """
    API class for DSPLIB matrix transpose kernel algorithm.

    This class encapsulates the algorithm portion of the matrix transpose kernel,
    providing a clean interface for initialization and execution.
    """

    def __init__(self, dataType="float", dimX=32, dimY=32, inputMatrix=None):
        """
        Initialize the DSPLIB_matTrans with kernel parameters.

        Args:
            dataType (str): Data type for processing ("float", "int8_t", "int16_t", "int32_t", etc.)
            dimX (int): Number of columns (width) of input matrix
            dimY (int): Number of rows (height) of input matrix
        """
        self.dataType = dataType
        self.dimX = dimX
        self.dimY = dimY

        # Initialize input and output matrices
        if inputMatrix is None:
            self._generate_input_matrix()
        else:
            self._set_input_matrix(inputMatrix)

    def _generate_input_matrix(self):
        """
        Generate random input matrix based on data type.
        """
        minVal, maxVal, data_type = self._get_data_range_type()
        self.inputMatrix = np.random.uniform(
            low=minVal, high=maxVal, size=(self.dimY, self.dimX)
        ).astype(data_type)

    def _set_input_matrix(self, inputMatrix):
        """
        Set custom input matrix.

        Args:
            inputMatrix: 2D array of input data (dimY x dimX)
        """
        if inputMatrix.shape != (self.dimY, self.dimX):
            raise ValueError(
                f"Input matrix shape {inputMatrix.shape} does not match expected shape ({self.dimY}, {self.dimX})"
            )
        # Verify dtype compatibility (simple string check)
        if str(inputMatrix.dtype) not in self.dataType and self.dataType not in str(
            inputMatrix.dtype
        ):
            raise ValueError(
                f"Input matrix shape {inputMatrix.dtype} does not match expected datatype {self.dataType})"
            )
        self.inputMatrix = inputMatrix

    def exec(self):
        """
        Execute the matrix transpose operation.

        Returns:
            numpy.ndarray: Transposed matrix of shape (dimX, dimY)
        """
        # Transpose the input matrix
        outputMatrix = np.transpose(self.inputMatrix)
        return outputMatrix

    def _get_data_range_type(self):
        """
        Get min and max values for data type.

        Returns:
            tuple: (minVal, maxVal)
        """
        if self.dataType == "int8_t":
            bits = 8
            is_unsigned = False
            data_type = "int8"
        elif self.dataType == "uint8_t":
            bits = 8
            is_unsigned = True
            data_type = "uint8"
        elif self.dataType == "int16_t":
            bits = 16
            is_unsigned = False
            data_type = "int16"
        elif self.dataType == "uint16_t":
            bits = 16
            is_unsigned = True
            data_type = "uint16"
        elif self.dataType == "int32_t":
            bits = 32
            is_unsigned = False
            data_type = "int32"
        elif self.dataType == "uint32_t":
            bits = 32
            data_type = "uint32"
            is_unsigned = True
        elif self.dataType == "int64_t":
            bits = 64
            is_unsigned = False
            data_type = "int64"
        elif self.dataType == "uint64_t":
            bits = 64
            is_unsigned = True
            data_type = "uint64"
        elif self.dataType == "float" or self.dataType == "float32":
            return -1.0, 1.0, np.float32
        elif self.dataType == "double" or self.dataType == "float64":
            return -1.0, 1.0, np.double
        else:
            print("self.dataType : ", self.dataType)
            raise ValueError(f"Unsupported data type: {self.dataType}")

        # Integer types
        if is_unsigned:
            minVal = 0
            maxVal = (2**bits) - 1
        else:
            minVal = -(2 ** (bits - 1))
            maxVal = 2 ** (bits - 1)

        return minVal, maxVal, data_type
