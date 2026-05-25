import numpy as np


class DSPLIB_peakAbsoluteValue:
    """Maximum absolute value operation for test data generation.

    Encapsulates peakAbsoluteValue computation for different data types with test data generation.
    """

    def __init__(self, dType="float", dataSize=256,
                 testType="STATIC", numReps=1):
        """Initialize peakAbsoluteValue operation with vector size and data type.

        Args:
            dType: Data type ("int8_t", "int16_t", "int32_t", "float", "double")
            dataSize: Number of elements in input vector
            testType: Test pattern ("STATIC" or "RANDOM")
            numReps: Number of test repetitions

        """
        self._dType = dType
        self._dataSize = dataSize
        self._testType = testType
        self._numReps = numReps

        self._set_minmax_values()
        self._totalElements = dataSize

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
        """Generate random input vector.

        Returns:
            ndarray: Input data with shape (dataSize,)

        """
        inData = np.random.uniform(
            low=self._minVal,
            high=self._maxVal,
            size=self._dataSize
        ).astype(self._numpy_dtype)
        return inData

    def exec(self, inData):
        """Compute maximum absolute value of input vector.

        Args:
            inData: Input data array with shape (dataSize,)

        Returns:
            scalar: Maximum absolute value

        """
        outData = np.max(np.abs(inData))
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
