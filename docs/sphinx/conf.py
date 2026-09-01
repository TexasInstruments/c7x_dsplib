# Configuration file for the Sphinx documentation builder.
#
# This file is part of the DSPLIB documentation build.
#
# Theme and CSS override approach (html_theme + theme_overrides.css) are
# deliberately mirrored from TexasInstruments/processor-sdk-doc:
#   html_theme = 'sphinx_rtd_theme'
#   html_logo  pointing at _static/img/ti_logo.png
#   html_show_sphinx = False
#   def setup(app): app.add_css_file("css/theme_overrides.css")
#
# All DEVFAMILY / OS / tag-loader machinery present in processor-sdk-doc has
# been omitted — DSPLIB is a single-target C library and does not need it.
#
# Doxygen XML is produced by the CMake build (not by Exhale).  Pass the
# output directory via the DSPLIB_DOXYGEN_XML environment variable before
# invoking sphinx-build, e.g.:
#   export DSPLIB_DOXYGEN_XML=/path/to/build/docs/xml
#   sphinx-build -b html docs/sphinx docs/sphinx/_build/html

import os
import sys

# ---------------------------------------------------------------------------
# Project information
# ---------------------------------------------------------------------------

project   = "DSPLIB"
author    = "Texas Instruments Incorporated"
copyright = "2026, Texas Instruments Incorporated"

# Show the source branch under the logo so the rendered docs correlate with
# the code they were generated from. Priority: explicit env override
# (DSPLIB_DOC_REF, set by CI) -> current git branch -> "main".
import subprocess


def _doc_ref():
    ref = os.environ.get("DSPLIB_DOC_REF")
    if ref:
        return ref.strip()
    try:
        out = subprocess.run(
            ["git", "rev-parse", "--abbrev-ref", "HEAD"],
            cwd=os.path.dirname(os.path.abspath(__file__)),
            capture_output=True, text=True, timeout=5)
        branch = out.stdout.strip()
        if branch and branch != "HEAD":
            return branch
    except Exception:
        pass
    return "main"


version = _doc_ref()
release = version

# ---------------------------------------------------------------------------
# Doxygen XML directory
# ---------------------------------------------------------------------------

_fallback_xml_dir = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "..", "..", "build", "docs", "xml")
)

xml_dir = os.environ.get("DSPLIB_DOXYGEN_XML", _fallback_xml_dir)

if not os.path.isdir(xml_dir):
    import warnings
    warnings.warn(
        f"[conf.py] Doxygen XML directory not found: {xml_dir!r}\n"
        "  Set DSPLIB_DOXYGEN_XML to the path produced by the CMake 'doxygen' target, "
        "or run CMake first to generate the XML before invoking sphinx-build.",
        stacklevel=2,
    )

# ---------------------------------------------------------------------------
# Extensions
# ---------------------------------------------------------------------------

extensions = [
    "breathe",
    "sphinx.ext.mathjax",
    "sphinx_design",
    "sphinx_copybutton",
    "sphinx.ext.graphviz",
    "sphinx_rtd_theme",
]

# ---------------------------------------------------------------------------
# Source / master document
# ---------------------------------------------------------------------------

master_doc = "index"

# ---------------------------------------------------------------------------
# C-centric language settings
# ---------------------------------------------------------------------------

primary_domain     = "c"
highlight_language = "c"

# ---------------------------------------------------------------------------
# Breathe configuration
# ---------------------------------------------------------------------------

breathe_projects        = {"dsplib": xml_dir}
breathe_default_project = "dsplib"
# DSPLIB is C, not C++. Parse .h/.c as the C domain so typedef'd handle
# signatures (e.g. DSPLIB_kernelHandle) resolve instead of tripping the
# C++ declaration parser.
breathe_domain_by_extension = {"h": "c", "c": "c"}

# ---------------------------------------------------------------------------
# API reference generation (per-kernel Doxygen groups)
# ---------------------------------------------------------------------------
# DSPLIB organizes each kernel as a Doxygen group (\defgroup / \addtogroup).
# The kernel OVERVIEW — including the LaTeX formulas embedded in the header
# comments — lives in the group's detailed description, which Exhale's
# file/function tree does not surface. Instead we emit one page per group
# using Breathe's ``.. doxygengroup::`` directive (which renders the overview
# prose + formulas + all member functions). This auto-scales to every kernel
# and is reused unchanged across the sibling libraries (audiolib, fftlib, ...).

import glob
import xml.etree.ElementTree as ET

_API_DIR = os.path.join(os.path.dirname(__file__), "api")

# Groups rendered without ``:members:`` because their header has an unbalanced
# Doxygen group (``@{`` with no ``@}``), which duplicates a member under ``-W``.
# Remove entries here once the corresponding header is fixed on Bitbucket dev.
_NO_MEMBERS_GROUPS = {"DSPLIB_addNCh"}


def _generate_api_pages(xml_directory, api_directory):
    """Emit api/index.rst + one api/<group>.rst per Doxygen group found in XML."""
    if not os.path.isdir(xml_directory):
        return
    os.makedirs(api_directory, exist_ok=True)

    groups = []  # list of (group_name, title)
    for path in sorted(glob.glob(os.path.join(xml_directory, "group__*.xml"))):
        try:
            root = ET.parse(path).getroot()
        except ET.ParseError:
            continue
        compound = root.find("./compounddef[@kind='group']")
        if compound is None:
            continue
        name_el = compound.find("compoundname")
        if name_el is None or not name_el.text:
            continue
        name = name_el.text.strip()
        title_el = compound.find("title")
        title = (title_el.text.strip() if title_el is not None and title_el.text
                 else name)
        groups.append((name, title))

    # One page per kernel group. ``:members:`` expands inner structs (e.g.
    # <kernel>_InitArgs) so their fields render, not just the struct name.
    #
    # Exception: a group whose header has an unclosed Doxygen group (``@{``
    # without a matching ``@}``) double-attributes a member, which trips a
    # duplicate-ID error under ``-W`` when ``:members:`` is used. Such groups
    # render without ``:members:`` until the header is fixed (code fix lands on
    # Bitbucket dev, tracked separately). Their struct fields will populate
    # automatically once the source group is balanced.
    for name, title in groups:
        underline = "=" * max(len(title), 3)
        with open(os.path.join(api_directory, f"{name}.rst"), "w",
                  encoding="utf-8") as fh:
            fh.write(f"{title}\n{underline}\n\n")
            opts = "" if name in _NO_MEMBERS_GROUPS else "\n   :members:"
            fh.write(f".. doxygengroup:: {name}{opts}\n")

    # API landing page with a toctree of every kernel page.
    with open(os.path.join(api_directory, "index.rst"), "w",
              encoding="utf-8") as fh:
        fh.write("API Reference\n=============\n\n")
        fh.write("Per-kernel API documentation generated from the source "
                 "headers.\n\n")
        fh.write(".. toctree::\n   :maxdepth: 1\n\n")
        for name, _title in groups:
            fh.write(f"   {name}\n")


_generate_api_pages(xml_dir, _API_DIR)


# ---------------------------------------------------------------------------
# Performance pages (embed the prebuilt perf/datasheet HTML tables)
# ---------------------------------------------------------------------------
# The legacy Doxygen docs inlined per-kernel performance tables via
# ``\htmlinclude ./html/<file>.html`` inside content_pages/performance_summary.h,
# split into per-device blocks (#if __C7100__ / __C7120__ / __C7504__ / __C7524__).
# The referenced files under docs/doxygen/html/ are self-contained <table>
# fragments. We reproduce that here: one page per device, each kernel table
# embedded via a ``.. raw:: html`` block, so the numbers render exactly as before.

_PERF_DIR = os.path.join(os.path.dirname(__file__), "perf")
_PERF_SUMMARY = os.path.join(
    os.path.dirname(__file__), "..", "doxygen", "content_pages",
    "performance_summary.h")
_PERF_HTML_DIR = os.path.join(os.path.dirname(__file__), "..", "doxygen", "html")

# Doxygen device macro -> (slug, human title)
# Only C7504 (AM62D) and C7524 (AM275) numbers are published for now.
_PERF_DEVICES = [
    ("__C7504__", "c7504", "C7504 (AM62D)"),
    ("__C7524__", "c7524", "C7524 (AM275)"),
]


def _parse_perf_summary(summary_path):
    """Return {device_macro: [(kernel_title, html_basename), ...]} from the .h."""
    import re
    result = {macro: [] for macro, _slug, _title in _PERF_DEVICES}
    if not os.path.isfile(summary_path):
        return result
    current = None
    title = None
    with open(summary_path, encoding="utf-8", errors="ignore") as fh:
        for line in fh:
            s = line.strip()
            m = re.match(r"#(?:el)?if\s+defined\((__C7\d+__)\)", s)
            if m:
                # Skip device blocks we do not publish (current -> None).
                current = m.group(1) if m.group(1) in result else None
                title = None
                continue
            if s.startswith("#endif"):
                current = None
                continue
            if current is None:
                continue
            m = re.match(r"\\subsection\s+\S+\s+(.*)", s)
            if m:
                title = m.group(1).strip()
                continue
            # Use search (not match): some entries put the \htmlinclude on the
            # same line as a trailing \f$...\f$ formula, so it is not at column 0.
            m = re.search(r"\\htmlinclude\s+\S*?/([^/\s]+\.html)", s)
            if m:
                result[current].append((title or m.group(1), m.group(1)))
                title = None
    return result


def _generate_perf_pages(summary_path, html_dir, perf_dir):
    """Emit perf/index.rst + one perf/<device>.rst per device with embedded tables."""
    data = _parse_perf_summary(summary_path)
    if not any(data.values()):
        return
    os.makedirs(perf_dir, exist_ok=True)

    written = []  # (slug, title) for pages that actually have tables
    for macro, slug, title in _PERF_DEVICES:
        entries = data.get(macro, [])
        # keep only entries whose table file exists on disk
        entries = [(t, f) for (t, f) in entries
                   if os.path.isfile(os.path.join(html_dir, f))]
        if not entries:
            continue
        underline = "=" * len(title)
        with open(os.path.join(perf_dir, f"{slug}.rst"), "w",
                  encoding="utf-8") as fh:
            fh.write(f"{title}\n{underline}\n\n")
            for ktitle, fname in entries:
                kunder = "-" * max(len(ktitle), 3)
                fh.write(f"{ktitle}\n{kunder}\n\n")
                fh.write(".. raw:: html\n\n")
                with open(os.path.join(html_dir, fname), encoding="utf-8",
                          errors="ignore") as tbl:
                    for tline in tbl.read().splitlines():
                        fh.write(f"   {tline}\n")
                fh.write("\n")
        written.append((slug, title))

    with open(os.path.join(perf_dir, "index.rst"), "w",
              encoding="utf-8") as fh:
        fh.write("Performance\n===========\n\n")
        fh.write("Measured kernel performance per device. EVM warm cycles are "
                 "obtained by profiling the kernel's compute code after a cold "
                 "run. See each kernel's API page for parameter definitions.\n\n")
        fh.write(".. toctree::\n   :maxdepth: 1\n\n")
        for slug, _title in written:
            fh.write(f"   {slug}\n")


_generate_perf_pages(_PERF_SUMMARY, _PERF_HTML_DIR, _PERF_DIR)

# ---------------------------------------------------------------------------
# MathJax (version 3 is the Sphinx default; AMS extensions load automatically)
# ---------------------------------------------------------------------------
# No custom macros needed for DSPLIB — rely on MathJax 3 defaults.

# ---------------------------------------------------------------------------
# HTML output
# ---------------------------------------------------------------------------

html_theme        = "sphinx_rtd_theme"
html_static_path  = ["_static"]
html_logo         = "_static/img/ti_logo.svg"

html_show_sphinx      = False
html_show_sourcelink  = False
html_copy_source      = False
html_last_updated_fmt = "%b %d, %Y"

# ---------------------------------------------------------------------------
# Theme customisation hook
# This follows the exact pattern used in TexasInstruments/processor-sdk-doc:
# the CSS override is loaded via app.add_css_file() inside setup() rather
# than through html_css_files, so it is applied after the RTD theme styles.
# ---------------------------------------------------------------------------

def setup(app):
    app.add_css_file("css/theme_overrides.css")
