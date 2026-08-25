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
        precision="fp64",
    ):
        """
        Initialize the DSPLIB_cascadeBiquad with kernel parameters.

        Args:
            dataType (str): Data type for processing ("float" is currently supported)
            numStages (int): Number of biquad stages
            numChannels (int): Number of channels to process
            biquadForm (str): Biquad form to use (DSPLIB_BIQUAD_DF1, DSPLIB_BIQUAD_DF2,
                DSPLIB_BIQUAD_TDF2, DSPLIB_BIQUAD_TIF)
            enableNChCoeffs (bool): Whether to use different coefficients for each channel
            numSamples (int): Number of samples to process
            precision (str): Arithmetic precision for exec().
                'fp64' (default) — scipy.signal.sosfilt in float64; maximum precision,
                    suitable for unit-test SNR measurement against any C kernel.
                'fp32' — form-specific float32 recursions that mirror the C kernel
                    operation-for-operation; use when the reference must match a
                    float32 C7x target (e.g. near-unit-circle validation).
                    Note: SNR vs fp64 degrades as poles approach the unit circle
                    (~80 dB at r=0.994, ~56 dB at r=0.999) — expected C kernel behaviour.
        """
        self.dataType = dataType
        self.numStages = numStages
        self.numChannels = numChannels
        self.biquadForm = biquadForm
        self.enableNChCoeffs = enableNChCoeffs
        self.numSamples = numSamples
        self.precision = precision

        # Set number of channels for coefficients generation
        self.filterNumChannels = 1 if not enableNChCoeffs else numChannels

        # Initialize filter coefficients
        self.sosCoefs = None
        self.tifBiquadCascadeCoefs = None
        self.tifBiquadCascadeGains = None

        # fp32+DF1 requires 4 state variables per stage (xd1, xd2, yd1, yd2).
        # All other precision/form combinations use 2 (sosfilt TDF2 layout for fp64;
        # DF2/TDF2/TIF f32 loops also use 2 states).
        num_states = 4 if (precision == "fp32" and biquadForm == "DSPLIB_BIQUAD_DF1") else 2
        self.zi = np.zeros((self.numStages, num_states, self.numChannels))

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
        num_states = 4 if (self.precision == "fp32" and self.biquadForm == "DSPLIB_BIQUAD_DF1") else 2
        self.zi = np.zeros((self.numStages, num_states, self.numChannels))

    def exec(self, inputData):
        """
        Execute the cascade biquad filter on input data.

        Dispatches to the fp64 (default) or fp32 path based on self.precision.

        Args:
            inputData: Input data array of shape (dataSize, numChannels)

        Returns:
            numpy.ndarray: Filtered output data of the same shape as inputData.
                fp64 path returns float64; fp32 path returns float32.
        """
        if inputData.shape[1] != self.numChannels:
            raise ValueError(
                f"Input data has {inputData.shape[1]} channels, but filter is "
                f"configured for {self.numChannels} channels"
            )

        if self.precision == "fp64":
            return self._exec_fp64(inputData)
        elif self.precision == "fp32":
            return self._exec_fp32(inputData)
        else:
            raise ValueError(f"Unsupported precision: {self.precision!r}. Use 'fp64' or 'fp32'.")

    def _exec_fp64(self, inputData):
        """fp64 path: scipy.signal.sosfilt for all forms (maximum precision)."""
        outputData = np.zeros_like(inputData)
        zf = np.zeros_like(self.zi)

        if not self.enableNChCoeffs:
            for ch in range(self.numChannels):
                outputData[:, ch], zf[:, :, ch] = signal.sosfilt(
                    np.ascontiguousarray(self.sosCoefs[:, :, 0]),
                    inputData[:, ch],
                    axis=0,
                    zi=self.zi[:, :, ch],
                )
        else:
            for ch in range(self.numChannels):
                outputData[:, ch], zf[:, :, ch] = signal.sosfilt(
                    np.ascontiguousarray(self.sosCoefs[:, :, ch]),
                    inputData[:, ch],
                    axis=0,
                    zi=self.zi[:, :, ch],
                )

        self.zi = zf
        return outputData

    def _exec_fp32(self, inputData):
        """fp32 path: form-specific float32 recursions matching the C kernel."""
        if self.biquadForm == "DSPLIB_BIQUAD_DF1":
            return self._exec_df1_f32(inputData)
        elif self.biquadForm == "DSPLIB_BIQUAD_DF2":
            return self._exec_df2_f32(inputData)
        elif self.biquadForm == "DSPLIB_BIQUAD_TDF2":
            return self._exec_tdf2_f32(inputData)
        elif self.biquadForm == "DSPLIB_BIQUAD_TIF":
            return self._exec_tif_f32(inputData)
        else:
            raise ValueError(f"Unsupported biquadForm: {self.biquadForm}")

    # ------------------------------------------------------------------
    # Form-specific float32 exec methods
    # ------------------------------------------------------------------

    def _exec_df1_f32(self, inputData):
        """
        Direct Form I (DF1) — replicates cascadeBiquad_cn.cpp:230.

        Recursion per stage per sample:
            output = b0*x + b1*xd1 + b2*xd2 - a1*yd1 - a2*yd2
            xd2 = xd1;  xd1 = x;  yd2 = yd1;  yd1 = output

        State layout: zi[stage, 0, ch] = xd1
                      zi[stage, 1, ch] = xd2
                      zi[stage, 2, ch] = yd1
                      zi[stage, 3, ch] = yd2

        SOS coefficient layout (standard scipy):
            sos[stage, :] = [b0, b1, b2, a0=1, a1, a2]
        """
        numSamples = inputData.shape[0]
        outputData = np.zeros((numSamples, self.numChannels), dtype=np.float32)

        # Work on a float32 copy of the input
        x_in = inputData.astype(np.float32)

        if not self.enableNChCoeffs:
            # All channels share the same SOS coefficients (index 0)
            sos_f32 = self.sosCoefs[:, :, 0].astype(np.float32)  # (numStages, 6)
            b0 = sos_f32[:, 0]  # (numStages,)
            b1 = sos_f32[:, 1]
            b2 = sos_f32[:, 2]
            a1 = sos_f32[:, 4]
            a2 = sos_f32[:, 5]

            # State: (numStages, 4, numChannels) — all channels share coefs
            # zi[:, 0, :] = xd1, zi[:, 1, :] = xd2,
            # zi[:, 2, :] = yd1, zi[:, 3, :] = yd2
            zi = self.zi.astype(np.float32)

            for n in range(numSamples):
                x_stage = x_in[n, :].copy()  # (numChannels,)
                for s in range(self.numStages):
                    # .copy() makes reads independent of the writes below
                    xd1 = zi[s, 0, :].copy()   # (numChannels,)
                    xd2 = zi[s, 1, :].copy()
                    yd1 = zi[s, 2, :].copy()
                    yd2 = zi[s, 3, :].copy()

                    out = (
                        np.float32(b0[s]) * x_stage
                        + np.float32(b1[s]) * xd1
                        + np.float32(b2[s]) * xd2
                        - np.float32(a1[s]) * yd1
                        - np.float32(a2[s]) * yd2
                    )
                    out = out.astype(np.float32)

                    zi[s, 1, :] = xd1
                    zi[s, 0, :] = x_stage
                    zi[s, 3, :] = yd1
                    zi[s, 2, :] = out

                    x_stage = out

                outputData[n, :] = x_stage

            # Uniform float64 dtype for self.zi regardless of precision mode;
            # _exec_fp64 passes it directly to sosfilt without re-casting.
            self.zi = zi.astype(np.float64)

        else:
            # Different coefficients per channel — loop channels separately
            for ch in range(self.numChannels):
                sos_f32 = self.sosCoefs[:, :, ch].astype(np.float32)
                b0 = sos_f32[:, 0]
                b1 = sos_f32[:, 1]
                b2 = sos_f32[:, 2]
                a1 = sos_f32[:, 4]
                a2 = sos_f32[:, 5]

                zi_ch = self.zi[:, :, ch].astype(np.float32)  # (numStages, 4)

                for n in range(numSamples):
                    x_stage = np.float32(x_in[n, ch])
                    for s in range(self.numStages):
                        xd1 = zi_ch[s, 0]
                        xd2 = zi_ch[s, 1]
                        yd1 = zi_ch[s, 2]
                        yd2 = zi_ch[s, 3]

                        out = np.float32(
                            np.float32(b0[s]) * x_stage
                            + np.float32(b1[s]) * xd1
                            + np.float32(b2[s]) * xd2
                            - np.float32(a1[s]) * yd1
                            - np.float32(a2[s]) * yd2
                        )

                        zi_ch[s, 1] = xd1
                        zi_ch[s, 0] = x_stage
                        zi_ch[s, 3] = yd1
                        zi_ch[s, 2] = out

                        x_stage = out

                    outputData[n, ch] = x_stage

                self.zi[:, :, ch] = zi_ch.astype(np.float64)

        return outputData

    def _exec_df2_f32(self, inputData):
        """
        Direct Form II (DF2) — replicates cascadeBiquad_cn.cpp:84.

        Recursion per stage per sample:
            wd0 = x - a1*wd1 - a2*wd2
            y   = b0*wd0 + b1*wd1 + b2*wd2
            wd2 = wd1;  wd1 = wd0

        State layout: zi[stage, 0, ch] = wd1
                      zi[stage, 1, ch] = wd2

        SOS coefficient layout (standard scipy):
            sos[stage, :] = [b0, b1, b2, a0=1, a1, a2]
        """
        numSamples = inputData.shape[0]
        outputData = np.zeros((numSamples, self.numChannels), dtype=np.float32)

        x_in = inputData.astype(np.float32)

        if not self.enableNChCoeffs:
            sos_f32 = self.sosCoefs[:, :, 0].astype(np.float32)
            b0 = sos_f32[:, 0]
            b1 = sos_f32[:, 1]
            b2 = sos_f32[:, 2]
            a1 = sos_f32[:, 4]
            a2 = sos_f32[:, 5]

            zi = self.zi.astype(np.float32)  # (numStages, 2, numChannels)

            for n in range(numSamples):
                x_stage = x_in[n, :].copy()  # (numChannels,)
                for s in range(self.numStages):
                    wd1 = zi[s, 0, :]
                    wd2 = zi[s, 1, :]

                    wd0 = (
                        x_stage
                        - np.float32(a1[s]) * wd1
                        - np.float32(a2[s]) * wd2
                    ).astype(np.float32)

                    out = (
                        np.float32(b0[s]) * wd0
                        + np.float32(b1[s]) * wd1
                        + np.float32(b2[s]) * wd2
                    ).astype(np.float32)

                    zi[s, 1, :] = wd1
                    zi[s, 0, :] = wd0

                    x_stage = out

                outputData[n, :] = x_stage

            self.zi = zi.astype(np.float64)

        else:
            for ch in range(self.numChannels):
                sos_f32 = self.sosCoefs[:, :, ch].astype(np.float32)
                b0 = sos_f32[:, 0]
                b1 = sos_f32[:, 1]
                b2 = sos_f32[:, 2]
                a1 = sos_f32[:, 4]
                a2 = sos_f32[:, 5]

                zi_ch = self.zi[:, :, ch].astype(np.float32)

                for n in range(numSamples):
                    x_stage = np.float32(x_in[n, ch])
                    for s in range(self.numStages):
                        wd1 = zi_ch[s, 0]
                        wd2 = zi_ch[s, 1]

                        wd0 = np.float32(
                            x_stage
                            - np.float32(a1[s]) * wd1
                            - np.float32(a2[s]) * wd2
                        )
                        out = np.float32(
                            np.float32(b0[s]) * wd0
                            + np.float32(b1[s]) * wd1
                            + np.float32(b2[s]) * wd2
                        )

                        zi_ch[s, 1] = wd1
                        zi_ch[s, 0] = wd0

                        x_stage = out

                    outputData[n, ch] = x_stage

                self.zi[:, :, ch] = zi_ch.astype(np.float64)

        return outputData

    def _exec_tdf2_f32(self, inputData):
        """
        Transposed Direct Form II (TDF2) — replicates cascadeBiquad_cn.cpp:386.

        Recursion per stage per sample:
            y   = wd1 + x*b0
            wd1 = wd2 + x*b1 - a1*y
            wd2 = x*b2 - a2*y

        State layout: zi[stage, 0, ch] = wd1
                      zi[stage, 1, ch] = wd2

        SOS coefficient layout (standard scipy):
            sos[stage, :] = [b0, b1, b2, a0=1, a1, a2]
        """
        numSamples = inputData.shape[0]
        outputData = np.zeros((numSamples, self.numChannels), dtype=np.float32)

        x_in = inputData.astype(np.float32)

        if not self.enableNChCoeffs:
            sos_f32 = self.sosCoefs[:, :, 0].astype(np.float32)
            b0 = sos_f32[:, 0]
            b1 = sos_f32[:, 1]
            b2 = sos_f32[:, 2]
            a1 = sos_f32[:, 4]
            a2 = sos_f32[:, 5]

            zi = self.zi.astype(np.float32)  # (numStages, 2, numChannels)

            for n in range(numSamples):
                x_stage = x_in[n, :].copy()  # (numChannels,)
                for s in range(self.numStages):
                    wd1 = zi[s, 0, :]
                    wd2 = zi[s, 1, :]

                    out = (wd1 + np.float32(b0[s]) * x_stage).astype(np.float32)
                    new_wd1 = (
                        wd2
                        + np.float32(b1[s]) * x_stage
                        - np.float32(a1[s]) * out
                    ).astype(np.float32)
                    new_wd2 = (
                        np.float32(b2[s]) * x_stage
                        - np.float32(a2[s]) * out
                    ).astype(np.float32)

                    zi[s, 0, :] = new_wd1
                    zi[s, 1, :] = new_wd2

                    x_stage = out

                outputData[n, :] = x_stage

            self.zi = zi.astype(np.float64)

        else:
            for ch in range(self.numChannels):
                sos_f32 = self.sosCoefs[:, :, ch].astype(np.float32)
                b0 = sos_f32[:, 0]
                b1 = sos_f32[:, 1]
                b2 = sos_f32[:, 2]
                a1 = sos_f32[:, 4]
                a2 = sos_f32[:, 5]

                zi_ch = self.zi[:, :, ch].astype(np.float32)

                for n in range(numSamples):
                    x_stage = np.float32(x_in[n, ch])
                    for s in range(self.numStages):
                        wd1 = zi_ch[s, 0]
                        wd2 = zi_ch[s, 1]

                        out = np.float32(wd1 + np.float32(b0[s]) * x_stage)
                        new_wd1 = np.float32(
                            wd2
                            + np.float32(b1[s]) * x_stage
                            - np.float32(a1[s]) * out
                        )
                        new_wd2 = np.float32(
                            np.float32(b2[s]) * x_stage
                            - np.float32(a2[s]) * out
                        )

                        zi_ch[s, 0] = new_wd1
                        zi_ch[s, 1] = new_wd2

                        x_stage = out

                    outputData[n, ch] = x_stage

                self.zi[:, :, ch] = zi_ch.astype(np.float64)

        return outputData

    def _exec_tif_f32(self, inputData):
        """
        Transposed Indirect Form (TIF) — replicates tif_lStage_ci.cpp.

        TIF uses pre-computed TI biquad coefficients stored in
        self.tifBiquadCascadeCoefs and aggregate per-group gains in
        self.tifBiquadCascadeGains.

        TI coefficient layout per stage (index in tifBiquadCascadeCoefs):
            [0] c0 = b1/b0 - a1
            [1] c1 = b2/b0 - a2
            [2] c2 = -a1  (used for new_d0 state update, maps to C kernel a11)
            [3] c3 = -a2  (used for new_d1 state update, maps to C kernel a12)

        Note: Python _gen_tif_biquad_coefficients stores coef[2]=-a1 and
        coef[3]=-a2, which are swapped relative to the C kernel's storage order
        (C stores coef[2]=-a2, coef[3]=-a1). The exec method compensates by
        using coef[2] for new_d0 and coef[3] for new_d1.

        Recursion per stage per sample (using Python coefficient ordering):
            y      = x + d0
            d0_new = c2*d0 + c0*x + d1   (coef[2]=-a1 = C kernel a11)
            d1_new = c3*d0 + c1*x         (coef[3]=-a2 = C kernel a12)

        After the last stage in each stage group, apply the aggregate gain:
            y *= outGain   (product of b0 values for stages in that group)

        Stage grouping: groups of NUM_STAGES_PER_STAGE_GROUP (7) stages.
        When numStages <= 8, all stages form a single group.

        State layout: zi[stage, 0, ch] = d0
                      zi[stage, 1, ch] = d1
        """
        numSamples = inputData.shape[0]
        outputData = np.zeros((numSamples, self.numChannels), dtype=np.float32)

        x_in = inputData.astype(np.float32)

        numStageGroups = math.ceil(self.numStages / self.NUM_STAGES_PER_STAGE_GROUP)

        if not self.enableNChCoeffs:
            # tifBiquadCascadeCoefs shape: (numStages, 4, filterNumChannels=1)
            # tifBiquadCascadeGains shape: (numStageGroups, filterNumChannels=1)
            coefs_f32 = self.tifBiquadCascadeCoefs[:, :, 0].astype(np.float32)
            # shape: (numStages, 4)
            gains_f32 = self.tifBiquadCascadeGains[:, 0].astype(np.float32)
            # shape: (numStageGroups,)

            zi = self.zi.astype(np.float32)  # (numStages, 2, numChannels)

            for n in range(numSamples):
                x_stage = x_in[n, :].copy()  # (numChannels,)
                for grp in range(numStageGroups):
                    start_s = grp * self.NUM_STAGES_PER_STAGE_GROUP
                    end_s = min(
                        (grp + 1) * self.NUM_STAGES_PER_STAGE_GROUP, self.numStages
                    )
                    for s in range(start_s, end_s):
                        c0 = np.float32(coefs_f32[s, 0])
                        c1 = np.float32(coefs_f32[s, 1])
                        c2 = np.float32(coefs_f32[s, 2])
                        c3 = np.float32(coefs_f32[s, 3])

                        d0 = zi[s, 0, :]  # (numChannels,)
                        d1 = zi[s, 1, :]

                        out = (x_stage + d0).astype(np.float32)
                        # Match C kernel association: a11*d0 + (c0*x + d1)
                        new_d0 = (c2 * d0 + (c0 * x_stage + d1)).astype(np.float32)
                        new_d1 = (c3 * d0 + c1 * x_stage).astype(np.float32)

                        zi[s, 0, :] = new_d0
                        zi[s, 1, :] = new_d1

                        x_stage = out

                    # Apply aggregate gain at the end of each stage group
                    x_stage = (x_stage * np.float32(gains_f32[grp])).astype(np.float32)

                outputData[n, :] = x_stage

            self.zi = zi.astype(np.float64)

        else:
            # Different coefficients per channel
            for ch in range(self.numChannels):
                coefs_f32 = self.tifBiquadCascadeCoefs[:, :, ch].astype(np.float32)
                gains_f32 = self.tifBiquadCascadeGains[:, ch].astype(np.float32)

                zi_ch = self.zi[:, :, ch].astype(np.float32)  # (numStages, 2)

                for n in range(numSamples):
                    x_stage = np.float32(x_in[n, ch])
                    for grp in range(numStageGroups):
                        start_s = grp * self.NUM_STAGES_PER_STAGE_GROUP
                        end_s = min(
                            (grp + 1) * self.NUM_STAGES_PER_STAGE_GROUP,
                            self.numStages,
                        )
                        for s in range(start_s, end_s):
                            c0 = np.float32(coefs_f32[s, 0])
                            c1 = np.float32(coefs_f32[s, 1])
                            c2 = np.float32(coefs_f32[s, 2])
                            c3 = np.float32(coefs_f32[s, 3])

                            d0 = zi_ch[s, 0]
                            d1 = zi_ch[s, 1]

                            out = np.float32(x_stage + d0)
                            # Match C kernel association: a11*d0 + (c0*x + d1)
                            new_d0 = np.float32(c2 * d0 + np.float32(c0 * x_stage + d1))
                            new_d1 = np.float32(c3 * d0 + c1 * x_stage)

                            zi_ch[s, 0] = new_d0
                            zi_ch[s, 1] = new_d1

                            x_stage = out

                        x_stage = np.float32(x_stage * gains_f32[grp])

                    outputData[n, ch] = x_stage

                self.zi[:, :, ch] = zi_ch.astype(np.float64)

        return outputData
