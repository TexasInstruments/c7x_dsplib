import numpy as np


class DSPLIB_absoluteValue:
    """Absolute value operation for test data generation.

    Encapsulates ABS computation for different data types with test data generation.
    """

    def __init__(self, dType="float", width=1, height=1,
                 strideInElements=1, strideOutElements=1,
                 testType="STATIC", numReps=1):
        """Initialize ABS operation with matrix dimensions and data type.

        Args:
            dType: Data type ("int8_t", "int16_t", "int32_t", "float", "double")
            width: Matrix width in elements
            height: Matrix height in elements
            strideInElements: Input row stride in elements
            strideOutElements: Output row stride in elements
            testType: Test pattern ("STATIC", "RANDOM" or "MINVAL": integers with the type minimum planted)
            numReps: Number of test repetitions

        """
        self._dType = dType
        self._width = width
        self._height = height
        self._strideInElements = strideInElements
        self._strideOutElements = strideOutElements
        self._testType = testType
        self._numReps = numReps

        self._set_minmax_values()
        self._totalElements = width * height

    def _set_minmax_values(self):
        """Set min/max values for random generation based on data type."""
        if self._dType == "int8_t":
            self._minVal = -128
            self._maxVal = 127
            self._numpy_dtype = np.int8
        elif self._dType == "int16_t":
            self._minVal = -32768
            self._maxVal = 32767
            self._numpy_dtype = np.int16
        elif self._dType == "int32_t":
            self._minVal = -2147483648
            self._maxVal = 2147483647
            self._numpy_dtype = np.int32
        elif self._dType == "int64_t":
            self._minVal = -9223372036854775808
            self._maxVal = 9223372036854775807
            self._numpy_dtype = np.int64
        elif self._dType == "float":
            self._minVal = -1e6
            self._maxVal = 1e6
            self._numpy_dtype = np.float32
        elif self._dType == "double":
            self._minVal = -1e10
            self._maxVal = 1e10
            self._numpy_dtype = np.float64
        else:
            self._minVal = -1.0
            self._maxVal = 1.0
            self._numpy_dtype = np.float32

    def generate_input_data(self):
        """Generate random input matrix.

        Returns:
            ndarray: Input data with shape (height, width)

        """
        if np.issubdtype(self._numpy_dtype, np.integer):
            # uniform() cannot represent the full 64-bit range exactly
            inData = np.random.randint(
                low=self._minVal,
                high=self._maxVal,
                size=(self._height, self._width),
                dtype=self._numpy_dtype
            )
            if self._testType == "MINVAL":
                # type minimum at a row start, a row end, the next row start and the last element
                inData[0, 0] = inData[0, self._width - 1] = self._minVal
                inData[1 % self._height, 0] = inData[self._height - 1, self._width - 1] = self._minVal
            return inData
        inData = np.random.uniform(
            low=self._minVal,
            high=self._maxVal,
            size=(self._height, self._width)
        ).astype(self._numpy_dtype)
        return inData

    def exec(self, inData):
        """Compute absolute value of input matrix.

        Args:
            inData: Input data array with shape (height, width)

        Returns:
            ndarray: Absolute value result, same shape and type as input

        """
        if np.issubdtype(self._numpy_dtype, np.integer):
            # saturating absolute value, as the C7x VABS instructions: |minimum| = maximum
            outData = np.where(inData == self._minVal, self._maxVal, np.abs(inData)).astype(self._numpy_dtype)
        else:
            outData = np.abs(inData).astype(self._numpy_dtype)
        return outData

    def get_data_type_info(self):
        """Get data type information (min/max values and numpy type).

        Returns:
            dict: Data type metadata

        """
        return {
            'dType': self._dType,
            'minVal': self._minVal,
            'maxVal': self._maxVal,
            'numpy_dtype': self._dType
        }
