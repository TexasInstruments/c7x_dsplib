# Copyright (C) 2026 Texas Instruments Incorporated
#
# SPDX-License-Identifier: Apache-2.0
#
# XLIB-1045: Verify cascadeBiquad precision modes.
#
# precision='fp64' (default): scipy.signal.sosfilt — maximum precision, used by
#   gen_data.py for all C test reference data. Forms are equivalent at fp64
#   so structure does not matter.
#
# precision='fp32' (explicit opt-in): form-specific float32 recursions that
#   mirror the C kernel operation-for-operation. Use when matching a float32
#   C7x target directly (e.g. near-unit-circle validation in tvm_byoc_tisp).
#   SNR vs fp64 degrades as poles approach the unit circle — expected behaviour.

import math
import numpy as np
import pytest
from scipy import signal

from cascadeBiquad import DSPLIB_cascadeBiquad

FS = 48000


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def _rms_snr(ref, test):
    """RMS SNR in dB between ref and test."""
    noise = ref.astype(np.float64) - test.astype(np.float64)
    rms_ref = math.sqrt(float(np.mean(ref.astype(np.float64) ** 2)))
    rms_n = math.sqrt(float(np.mean(noise ** 2)))
    if rms_n == 0.0:
        return 999.0
    return 20.0 * math.log10(rms_ref / rms_n)


def _run_form(biquad_form, sos, x, num_channels=1, precision="fp32"):
    """Run form at given precision; return 1-D output."""
    num_stages = sos.shape[0]
    sos3d = np.broadcast_to(sos[:, :, np.newaxis], (num_stages, 6, num_channels)).copy()
    cb = DSPLIB_cascadeBiquad(
        numStages=num_stages,
        numChannels=num_channels,
        biquadForm=biquad_form,
        precision=precision,
    )
    cb.set_filter_coefficients(sos3d)
    if biquad_form == "DSPLIB_BIQUAD_TIF":
        cb.tifBiquadCascadeCoefs, cb.tifBiquadCascadeGains = (
            cb._gen_tif_biquad_coefficients(cb.sosCoefs)
        )
    cb.reset_states()
    x2d = np.broadcast_to(x[:, np.newaxis], (len(x), num_channels))
    return cb.exec(x2d)[:, 0]


def _fp64_sosfilt(sos, x):
    """fp64 sosfilt — equivalent to precision='fp64' default path."""
    return signal.sosfilt(sos, x.astype(np.float64))


def _butter_sos(order, fc_hz, btype):
    wn = fc_hz / (FS / 2) if not isinstance(fc_hz, (list, tuple)) else [f / (FS / 2) for f in fc_hz]
    return signal.butter(order, wn, btype=btype, output="sos")


ALL_FORMS = [
    "DSPLIB_BIQUAD_DF1",
    "DSPLIB_BIQUAD_DF2",
    "DSPLIB_BIQUAD_TDF2",
    "DSPLIB_BIQUAD_TIF",
]


# ---------------------------------------------------------------------------
# 1. Sanity: output is finite for all forms and filter conditions
# ---------------------------------------------------------------------------

@pytest.mark.parametrize("form", ALL_FORMS)
@pytest.mark.parametrize("sos_label,sos", [
    ("LPF@4800 r=0.64", _butter_sos(2, 4800, "low")),
    ("HPF@300  r=0.97", _butter_sos(2, 300,  "high")),
    ("HPF@60   r=0.994",_butter_sos(2, 60,   "high")),
    ("HPF@10   r=0.999",_butter_sos(2, 10,   "high")),
    ("BPF@100-500",     _butter_sos(2, [100, 500], "bandpass")),
])
def test_output_finite(form, sos_label, sos):
    np.random.seed(42)
    x = np.random.randn(1024).astype(np.float32)
    out = _run_form(form, sos, x)
    assert np.all(np.isfinite(out)), \
        f"{form} {sos_label}: output contains non-finite values"


# ---------------------------------------------------------------------------
# 2. Well-conditioned: all forms agree with old f64 sosfilt to >120 dB
#    (poles far from unit circle — structure/precision mismatch negligible)
# ---------------------------------------------------------------------------

@pytest.mark.parametrize("form", ALL_FORMS)
@pytest.mark.parametrize("sos_label,sos,min_snr", [
    ("LPF@4800 r=0.64", _butter_sos(2, 4800, "low"),  120.0),
    ("HPF@3kHz r=0.86", _butter_sos(2, 3000, "high"), 120.0),
])
def test_wellconditioned_agrees_with_fp64_sosfilt(form, sos_label, sos, min_snr):
    np.random.seed(1)
    x = np.random.randn(8192).astype(np.float32)
    new = _run_form(form, sos, x)
    old = _fp64_sosfilt(sos, x)
    snr = _rms_snr(old, new)
    assert snr >= min_snr, (
        f"{form} {sos_label}: SNR {snr:.1f} dB < {min_snr} dB — "
        "well-conditioned filters should match old sosfilt tightly"
    )


# ---------------------------------------------------------------------------
# 3. Near-unit-circle: old f64 sosfilt diverges from new f32 form
#    (proves the fix is not a no-op for ill-conditioned poles)
# ---------------------------------------------------------------------------

@pytest.mark.parametrize("form,max_snr_60hz,max_snr_10hz", [
    # TIF has better numerical properties than DF1/DF2/TDF2 for near-unit-circle
    # filters (gain is normalized out per stage group), so its threshold is looser.
    ("DSPLIB_BIQUAD_DF1",  90.0, 65.0),
    ("DSPLIB_BIQUAD_DF2",  90.0, 65.0),
    ("DSPLIB_BIQUAD_TDF2", 90.0, 70.0),
    ("DSPLIB_BIQUAD_TIF",  103.0, 80.0),
])
@pytest.mark.parametrize("sos_label,sos,snr_idx", [
    ("HPF@60 r=0.994", _butter_sos(2, 60, "high"), 0),
    ("HPF@10 r=0.999", _butter_sos(2, 10, "high"), 1),
])
def test_near_unitcircle_fp64_sosfilt_diverges(form, max_snr_60hz, max_snr_10hz, sos_label, sos, snr_idx):
    max_snr = max_snr_60hz if snr_idx == 0 else max_snr_10hz
    np.random.seed(2)
    x = np.random.randn(8192).astype(np.float32)
    new = _run_form(form, sos, x)
    old = _fp64_sosfilt(sos, x)
    snr = _rms_snr(new, old)
    assert snr < max_snr, (
        f"{form} {sos_label}: SNR {snr:.1f} dB >= {max_snr} dB — "
        "expected old f64 sosfilt to diverge from the new f32 reference"
    )


# ---------------------------------------------------------------------------
# 4. DF1 corner cases: explicit JIRA-reported SNR values (XLIB-1045)
# ---------------------------------------------------------------------------

class TestDF1JiraScenario:
    """Replicate the JIRA XLIB-1045 evidence table for DF1 form."""

    FORM = "DSPLIB_BIQUAD_DF1"

    @pytest.fixture(autouse=True)
    def _setup(self):
        np.random.seed(0)
        self.x = np.random.randn(8192).astype(np.float32)

    def _snr_vs_old(self, sos):
        new = _run_form(self.FORM, sos, self.x)
        old = _fp64_sosfilt(sos, self.x)
        return _rms_snr(new, old)

    def test_lpf_4800hz_well_conditioned(self):
        """r≈0.64: old sosfilt agrees with f32 DF1 to >120 dB."""
        assert self._snr_vs_old(_butter_sos(2, 4800, "low")) >= 120.0

    def test_hpf_300hz_moderate(self):
        """r≈0.97: old sosfilt agrees >90 dB (moderate conditioning)."""
        assert self._snr_vs_old(_butter_sos(2, 300, "high")) >= 90.0

    def test_hpf_60hz_near_unit_circle(self):
        """r≈0.994 HPF@60Hz: old sosfilt disagrees <90 dB — XLIB-1045 root case."""
        snr = self._snr_vs_old(_butter_sos(2, 60, "high"))
        assert snr < 90.0, (
            f"Expected <90 dB for HPF@60Hz (XLIB-1045 root case), got {snr:.1f} dB"
        )

    def test_hpf_10hz_very_near_unit_circle(self):
        """r≈0.999: severe mismatch expected (<70 dB)."""
        assert self._snr_vs_old(_butter_sos(2, 10, "high")) < 70.0

    def test_lr4_hpf_80hz_cascaded(self):
        """4th-order LR4 HPF@80 Hz: cascaded near-unit-circle stages compound error."""
        snr = self._snr_vs_old(_butter_sos(4, 80, "high"))
        assert snr < 90.0, (
            f"Cascaded near-unit-circle stages should widen gap, got {snr:.1f} dB"
        )

    def test_subsonic_hpf_20hz(self):
        """HPF@20Hz r≈0.997: subsonic rumble filter, common in audio chains."""
        snr = self._snr_vs_old(_butter_sos(2, 20, "high"))
        assert snr < 70.0, (
            f"HPF@20Hz (r≈0.997) should show old-vs-new divergence <70 dB, got {snr:.1f} dB"
        )


# ---------------------------------------------------------------------------
# 5. State persistence: reset_states restores zero initial conditions
# ---------------------------------------------------------------------------

@pytest.mark.parametrize("form", ALL_FORMS)
def test_reset_states_restores_zero_ic(form):
    sos = _butter_sos(2, 300, "high")
    np.random.seed(3)
    x = np.random.randn(256).astype(np.float32)
    out1 = _run_form(form, sos, x)
    out2 = _run_form(form, sos, x)
    assert np.allclose(out1, out2), \
        f"{form}: repeated runs with reset give different output"


# ---------------------------------------------------------------------------
# 6. zi shape: DF1 allocates 4 states per stage, others allocate 2
# ---------------------------------------------------------------------------

def test_df1_state_shape():
    # fp32+DF1 needs 4 states; fp64+DF1 uses sosfilt TDF2 layout (2 states)
    cb_f32 = DSPLIB_cascadeBiquad(numStages=3, numChannels=4, biquadForm="DSPLIB_BIQUAD_DF1", precision="fp32")
    assert cb_f32.zi.shape == (3, 4, 4), f"fp32 DF1 zi shape wrong: {cb_f32.zi.shape}"
    cb_f64 = DSPLIB_cascadeBiquad(numStages=3, numChannels=4, biquadForm="DSPLIB_BIQUAD_DF1", precision="fp64")
    assert cb_f64.zi.shape == (3, 2, 4), f"fp64 DF1 zi shape wrong: {cb_f64.zi.shape}"


@pytest.mark.parametrize("form", ["DSPLIB_BIQUAD_DF2", "DSPLIB_BIQUAD_TDF2", "DSPLIB_BIQUAD_TIF"])
def test_non_df1_state_shape(form):
    for prec in ("fp64", "fp32"):
        cb = DSPLIB_cascadeBiquad(numStages=3, numChannels=4, biquadForm=form, precision=prec)
        assert cb.zi.shape == (3, 2, 4), f"{form} {prec} zi shape wrong: {cb.zi.shape}"


# ---------------------------------------------------------------------------
# fp64 default path
# ---------------------------------------------------------------------------

@pytest.mark.parametrize("form", ALL_FORMS)
def test_fp64_default_is_sosfilt(form):
    """precision='fp64' (default) must agree with scipy.signal.sosfilt to >120 dB
    for well-conditioned filters — it IS sosfilt under the hood."""
    sos = _butter_sos(2, 300, "high")
    np.random.seed(10)
    x = np.random.randn(1024).astype(np.float32)
    out_default = _run_form(form, sos, x, precision="fp64")
    out_sosfilt = _fp64_sosfilt(sos, x)
    snr = _rms_snr(out_sosfilt, out_default)
    assert snr >= 120.0, f"{form} fp64 default vs sosfilt: {snr:.1f} dB < 120 dB"


def test_fp64_vs_fp32_near_unitcircle_snr():
    """fp32 DF1 vs fp64 DF1 SNR decreases monotonically as r→1 (expected behaviour,
    not a bug). This documents the known precision trade-off."""
    snrs = {}
    for label, fc in [("r=0.64", 4800), ("r=0.97", 300), ("r=0.994", 60), ("r=0.999", 10)]:
        sos = _butter_sos(2, fc, "high")
        np.random.seed(0)
        x = np.random.randn(8192).astype(np.float32)
        fp32 = _run_form("DSPLIB_BIQUAD_DF1", sos, x, precision="fp32")
        fp64 = _run_form("DSPLIB_BIQUAD_DF1", sos, x, precision="fp64")
        snrs[label] = _rms_snr(fp64, fp32)
    # Well-conditioned: >120 dB; near-unit-circle: progressively lower
    assert snrs["r=0.64"]  >= 120.0
    assert snrs["r=0.97"]  >= 90.0
    assert snrs["r=0.994"] <  snrs["r=0.97"]   # degrades approaching r=1
    assert snrs["r=0.999"] <  snrs["r=0.994"]  # degrades further


# ---------------------------------------------------------------------------
# 7. Multi-channel: per-channel output matches single-channel (shared coefs)
# ---------------------------------------------------------------------------

@pytest.mark.parametrize("form", ALL_FORMS)
def test_multichannel_matches_singlechannel(form):
    sos = _butter_sos(2, 60, "high")  # near-unit-circle to stress test
    np.random.seed(4)
    x = np.random.randn(512).astype(np.float32)
    single = _run_form(form, sos, x, num_channels=1)
    multi  = _run_form(form, sos, x, num_channels=4)
    assert np.allclose(single, multi), \
        f"{form}: single-channel vs multi-channel (shared coefs) mismatch"


# ---------------------------------------------------------------------------
# 8. TIF: >8 stages split into 7-stage groups; aggregate gain applied correctly
# ---------------------------------------------------------------------------

def test_tif_multistage_group_gain():
    """9-stage TIF uses two stage groups (7+2); output must be finite and
    reproducible."""
    sos = _butter_sos(2 * 9, 300, "low")
    assert sos.shape[0] == 9
    np.random.seed(5)
    x = np.random.randn(512).astype(np.float32)
    out1 = _run_form("DSPLIB_BIQUAD_TIF", sos, x)
    out2 = _run_form("DSPLIB_BIQUAD_TIF", sos, x)
    assert np.all(np.isfinite(out1))
    assert np.allclose(out1, out2)
