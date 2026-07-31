Repository Layout
=================

Top-level structure of the DSPLIB repository:

.. code-block:: text

   dsplib/
   ├── CMakeLists.txt                   Top-level CMake entry point
   ├── CMakePresets.json                Shared, tracked build presets
   ├── README.md                        Overview, features, kernel list
   ├── LICENSE                          Apache-2.0 license text
   ├── CONTRIBUTING.md                  Contribution policy
   ├── makefile.am62dx.c75x.ti-c7000    Standalone makefile (AM62D / C7504)
   ├── makefile.am275x.c75x.ti-c7000    Standalone makefile (AM275 / C7524)
   ├── cmake/                           CMake helper modules (toolchains, docs, tests)
   ├── src/                             Kernel implementations — one folder per kernel
   │   ├── common/                      Shared types, buffer params, inlines
   │   └── DSPLIB_<kernel>/             Per-kernel sources + public header
   ├── test/                            Test drivers and generated vectors
   │   ├── DSPLIB_idat_gen/             Python test-vector generators
   │   └── DSPLIB_<kernel>/             Per-kernel test driver + data
   ├── examples/                        Minimal usage example per kernel
   └── docs/                            Documentation
       ├── doxygen/                     Doxyfiles, content pages, performance HTML
       └── sphinx/                      Sphinx site (conf.py, rst, TI theme)

Kernel sources — ``src/DSPLIB_<kernel>/``
-----------------------------------------

Each kernel lives in its own folder. The public header carries the Doxygen
documentation that this site renders.

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - File
     - Contents
   * - ``DSPLIB_<kernel>.h``
     - Public API — init/exec prototypes and Doxygen documentation
   * - ``DSPLIB_<kernel>_cn.cpp``
     - C-native reference implementation (``_cn`` / ``NATC``)
   * - ``DSPLIB_<kernel>_ci.cpp``
     - C7™ intrinsic-optimized implementation (``_ci`` / ``OPT``)
   * - ``DSPLIB_<kernel>_priv.h``, ``*_common.*``
     - Internal helpers and shared code — not part of the public API
   * - ``CMakeLists.txt``
     - Build rules for the kernel

``src/common/`` holds cross-kernel definitions: ``DSPLIB_types.h`` (status
codes, kernel handle, function-style enums), ``DSPLIB_bufParams.h``,
``DSPLIB_inlines.h``, ``DSPLIB_utility.h``, and ``TI_platforms.h``.

Tests — ``test/DSPLIB_<kernel>/``
---------------------------------

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - File
     - Contents
   * - ``DSPLIB_<kernel>_d.c``
     - Test driver (entry point) for the kernel
   * - ``DSPLIB_<kernel>_check.{cpp,h}``
     - Result verification against the reference
   * - ``DSPLIB_<kernel>_idat.{c,h}``
     - Generated test vectors (see :doc:`getting_started`)

Test-vector generators — ``test/DSPLIB_idat_gen/<kernel>/``
-----------------------------------------------------------

.. list-table::
   :header-rows: 1
   :widths: 45 55

   * - File
     - Contents
   * - ``gen_data.py``
     - Generates the ``_idat`` vectors for the kernel
   * - ``test_cases_list.csv``
     - Parametric test-case definitions (sizes, types, options)
   * - ``file_io.py``
     - Shared I/O helper for the generators

Other folders
-------------

- ``examples/DSPLIB_<kernel>/`` — a minimal, self-contained usage example per kernel.
- ``cmake/`` — build-system modules: toolchain files (``c7x.cmake``, ``gcc.cmake``),
  documentation (``doxygen.cmake``), and test/vector generation
  (``test.cmake``, ``gen_test_cases.cmake``).
- ``docs/doxygen/`` — Doxygen configuration, narrative content pages, and the
  prebuilt performance HTML tables.
- ``docs/sphinx/`` — this documentation site (``conf.py``, ``.rst`` sources, and
  the TI-branded theme assets under ``_static/``).
