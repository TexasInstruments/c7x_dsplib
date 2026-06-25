# Copyright (C) 2026 Texas Instruments Incorporated
#
# SPDX-License-Identifier: Apache-2.0

import numpy as np
import math
from scipy import signal


class DSPLIB_cascadeBiquad:
    """
    API class for DSPLIB cascade biquad kernel algorithm.

    This class encapsulates the algorithm portion of the cascade biquad kernel,
    providing a clean interface for initialization and execution.
    """

    # Constants
    NUM_COEFS_PER_BIQUAD_FULL = (
        6  # Number of coefficients per biquad, including a0 coefficient
    )
    NUM_COEFS_PER_BIQUAD = (
        5  # Number of coefficients per biquad, excluding a0 coefficient
    )
    NUM_COEFS_PER_TI_BIQUAD = 4  # Number of coefficients per TI biquad
    NUM_STATES_PER_DF2_BIQUAD = 2  # Number of states variables per DF2/TDF2 biquad
    NUM_STAGES_PER_STAGE_GROUP = 7  # Number of stages per stage group

    # Filter cutoff frequency constants (normalized to Nyquist frequency)
    LPF_MIN_FREQ = 0.25  # Low pass filter cutoff minimum frequency
    LPF_RNG_FREQ = 0.25  # Low pass filter cutoff frequency range
    HPF_MIN_FREQ = 0.5  # High pass filter cutoff minimum frequency
    HPF_RNG_FREQ = 0.25  # High pass filter cutoff frequency range
    BF_LOW_MIN_FREQ = 0.25  # Bandpass/bandstop low cutoff minimum frequency
    BF_LOW_RNG_FREQ = 0.15  # Bandpass/bandstop low cutoff frequency range
    BF_HIGH_MIN_FREQ = 0.6  # Bandpass/bandstop high cutoff minimum frequency
    BF_HIGH_RNG_FREQ = 0.15  # Bandpass/bandstop high cutoff frequency range

    def __init__(
        self,
        dataType="float",
        numStages=1,
        numChannels=1,
        biquadForm="DSPLIB_BIQUAD_DF2",
        enableNChCoeffs=False,
        numSamples=1,
    ):
        """
        Initialize the DSPLIB_cascadeBiquad with kernel parameters.

        Args:
            dataType (str): Data type for processing ("float" is currently supported)
            numStages (int): Number of biquad stages
            numChannels (int): Number of channels to process
            biquadForm (str): Biquad form to use (DSPLIB_BIQUAD_DF1, DSPLIB_BIQUAD_DF2, DSPLIB_BIQUAD_TDF2, DSPLIB_BIQUAD_TIF)
            enableNChCoeffs (bool): Whether to use different coefficients for each channel
            numSamples (int): Number of samples to process
        """
        self.dataType = dataType
        self.numStages = numStages
        self.numChannels = numChannels
        self.biquadForm = biquadForm
        self.enableNChCoeffs = enableNChCoeffs
        self.numSamples = numSamples

        # Set number of channels for coefficients generation
        self.filterNumChannels = 1 if not enableNChCoeffs else numChannels

        # Initialize filter coefficients
        self.sosCoefs = None
        self.tifBiquadCascadeCoefs = None
        self.tifBiquadCascadeGains = None

        # Initialize filter states
        self.zi = np.zeros((self.numStages, 2, self.numChannels))

        # Generate default filter coefficients
        self._generate_filter_coefficients()

    def _generate_filter_coefficients(self, filter_types=None, filter_cutoffs=None):
        """
        Generate filter coefficients based on specified parameters or random values.

        Args:
            filter_types (list, optional): List of filter types for each channel
            filter_cutoffs (numpy.ndarray, optional): Array of filter cutoffs for each channel
        """
        # Initialize filter types
        filterTypes = [
            "low",
            "high",
            "bandpass",
            "bandstop",
        ]  # butterworth filter types
        filterType = []

        # Initialize filter order and cutoff
        filterOrder = np.zeros(self.filterNumChannels)
        filterCutoff = np.zeros((self.filterNumChannels, 2))

        # Initialize biquad coefficients
        bCoefs = np.zeros((self.filterNumChannels, 2 * self.numStages + 1))
        aCoefs = np.zeros((self.filterNumChannels, 2 * self.numStages + 1))

        # Initialize SOS coefficients
        self.sosCoefs = np.zeros(
            (self.numStages, self.NUM_COEFS_PER_BIQUAD_FULL, self.filterNumChannels)
        )

        # Loop over channels for filter coefficient generation
        for ch in range(self.filterNumChannels):
            # Use provided filter type or randomly select one
            if filter_types is not None and ch < len(filter_types):
                filterType.append(filter_types[ch])
            else:
                filterType.append(np.random.choice(filterTypes))

            # Set filter order based on filter type
            # Use provided cutoffs or randomly generate filter cutoffs based on filter type
            if filterType[ch] == "low":
                filterOrder[ch] = 2 * self.numStages
                if filter_cutoffs is not None and ch < filter_cutoffs.shape[0]:
                    filterCutoff[ch, 0] = filter_cutoffs[ch, 0]
                else:
                    filterCutoff[ch, 0] = (
                        self.LPF_MIN_FREQ + np.random.rand() * self.LPF_RNG_FREQ
                    )

                # design filter coefficients
                [b, a] = signal.butter(
                    filterOrder[ch], filterCutoff[ch, 0], btype=filterType[ch]
                )
                bCoefs[ch, : len(b)] = b
                aCoefs[ch, : len(a)] = a

            elif filterType[ch] == "high":
                filterOrder[ch] = 2 * self.numStages
                if filter_cutoffs is not None and ch < filter_cutoffs.shape[0]:
                    filterCutoff[ch, 0] = filter_cutoffs[ch, 0]
                else:
                    filterCutoff[ch, 0] = (
                        self.HPF_MIN_FREQ + np.random.rand() * self.HPF_RNG_FREQ
                    )

                # design filter coefficients
                [b, a] = signal.butter(
                    filterOrder[ch], filterCutoff[ch, 0], btype=filterType[ch]
                )
                bCoefs[ch, : len(b)] = b
                aCoefs[ch, : len(a)] = a

            elif filterType[ch] == "bandpass" or filterType[ch] == "bandstop":
                filterOrder[ch] = self.numStages
                if filter_cutoffs is not None and ch < filter_cutoffs.shape[0]:
                    filterCutoff[ch, 0] = filter_cutoffs[ch, 0]
                    filterCutoff[ch, 1] = filter_cutoffs[ch, 1]
                else:
                    filterCutoff[ch, 0] = (
                        self.BF_LOW_MIN_FREQ + np.random.rand() * self.BF_LOW_RNG_FREQ
                    )
                    filterCutoff[ch, 1] = (
                        self.BF_HIGH_MIN_FREQ + np.random.rand() * self.BF_HIGH_RNG_FREQ
                    )

                # design filter coefficients
                [b, a] = signal.butter(
                    filterOrder[ch],
                    [filterCutoff[ch, 0], filterCutoff[ch, 1]],
                    btype=filterType[ch],
                )

                bCoefs[ch, : len(b)] = b
                aCoefs[ch, : len(a)] = a

            else:
                raise ValueError(f"Unsupported filter type: {filterType[ch]}")

            # Design 2nd order sections
            sosChan = signal.tf2sos(bCoefs[ch, : len(b)], aCoefs[ch, : len(a)])
            self.sosCoefs[:, :, ch] = sosChan

        # Generate TI biquad form coefficients if needed
        if self.biquadForm == "DSPLIB_BIQUAD_TIF":
            self.tifBiquadCascadeCoefs, self.tifBiquadCascadeGains = (
                self._gen_tif_biquad_coefficients(self.sosCoefs)
            )

    def _gen_tif_biquad_coefficients(self, sosCoefs):
        """
        Generate TI biquad form coefficients.

        Args:
            sosCoefs: 3D array of SOS coefficients.
                    Dim1: Number of SOS stages.
                    Dim2: Number of coefficients per stage (6).
                    Dim3: Number of channels.

        Returns:
            numpy.ndarray: 3D array of TI biquad coefficients
                        Dim1: Number of stages.
                        Dim2: Number of TI coefficients per stage (4).
                        Dim3: Number of channels.
            numpy.ndarray: 2D array of TI biquad gains
                        Dim1: Number of stage groups.
                        Dim2: Number of channels.
        """
        # Get parameters
        numStages = sosCoefs.shape[0]
        numChannels = sosCoefs.shape[2]

        ti_biquad_coefs = np.zeros(
            (numStages, self.NUM_COEFS_PER_TI_BIQUAD, numChannels)
        )
        for ch in range(numChannels):
            # Calculate biquad coefficients for each channel
            b0_r = 1 / sosCoefs[:, 0, ch]
            ti_biquad_coefs[:, [0, 1], ch] = b0_r[:, None] * sosCoefs[:, [1, 2], ch]
            ti_biquad_coefs[:, [2, 3], ch] = -sosCoefs[:, [4, 5], ch]
            ti_biquad_coefs[:, 0, ch] = (
                ti_biquad_coefs[:, 0, ch] + ti_biquad_coefs[:, 2, ch]
            )
            ti_biquad_coefs[:, 1, ch] = (
                ti_biquad_coefs[:, 1, ch] + ti_biquad_coefs[:, 3, ch]
            )

        # Compute aggregate gain
        numStageGroups = math.ceil(numStages / self.NUM_STAGES_PER_STAGE_GROUP)
        ti_gains = np.zeros((numStageGroups, numChannels))
        for ch in range(numChannels):
            for i in range(numStageGroups):
                start_idx = i * self.NUM_STAGES_PER_STAGE_GROUP
                end_idx = min((i + 1) * self.NUM_STAGES_PER_STAGE_GROUP, numStages)
                stage_group_sos_coefs = sosCoefs[start_idx:end_idx, :, ch]
                ti_gains[i, ch] = np.prod(stage_group_sos_coefs[:, 0])

        return ti_biquad_coefs, ti_gains

    def set_filter_coefficients(self, sosCoefs):
        """
        Set custom filter coefficients.

        Args:
            sosCoefs: 3D array of SOS coefficients.
                    Dim1: Number of SOS stages.
                    Dim2: Number of coefficients per stage (6).
                    Dim3: Number of channels.
        """
        self.sosCoefs = sosCoefs

        # Generate TI biquad form coefficients if needed
        if self.biquadForm == "DSPLIB_BIQUAD_TIF":
            self.tifBiquadCascadeCoefs, self.tifBiquadCascadeGains = (
                self._gen_tif_biquad_coefficients(self.sosCoefs)
            )

    def reset_states(self):
        """Reset filter states to zeros."""
        self.zi = np.zeros((self.numStages, 2, self.numChannels))

    def exec(self, inputData):
        """
        Execute the cascade biquad filter on input data.

        Args:
            inputData: Input data array of shape (dataSize, numChannels)

        Returns:
            numpy.ndarray: Filtered output data of the same shape as inputData
        """
        # Check input data shape
        if inputData.shape[1] != self.numChannels:
            raise ValueError(
                f"Input data has {inputData.shape[1]} channels, but filter is configured for {self.numChannels} channels"
            )

        # Initialize output data
        outputData = np.zeros_like(inputData)
        zf = np.zeros_like(self.zi)

        if not self.enableNChCoeffs:
            # All channels use the same coefficients
            for ch in range(self.numChannels):
                outputData[:, ch], zf[:, :, ch] = signal.sosfilt(
                    np.ascontiguousarray(self.sosCoefs[:, :, 0]),
                    inputData[:, ch],
                    axis=0,
                    zi=self.zi[:, :, ch],
                )
        else:
            # Different coefficients for each channel
            for ch in range(self.numChannels):
                outputData[:, ch], zf[:, :, ch] = signal.sosfilt(
                    np.ascontiguousarray(self.sosCoefs[:, :, ch]),
                    inputData[:, ch],
                    axis=0,
                    zi=self.zi[:, :, ch],
                )

        # Update filter states
        self.zi = zf

        return outputData
