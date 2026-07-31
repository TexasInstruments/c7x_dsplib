Getting Started
===============

DSPLIB uses **CMake presets** to manage build configurations, keeping the
workflow consistent across PC simulation and on-target builds.

Prerequisites
-------------

- **Linux** host (Ubuntu 20.04 or later recommended)
- **CMake** ≥ 3.21
- **TI C7000 Code Generation Tools** (``ti-cgt-c7000``)

  Download from `TI C7000-CGT <https://www.ti.com/tool/C7000-CGT>`_ and install to ``~/ti/``
  (default expected path ``~/ti/ti-cgt-c7000_5.0.0.LTS/``), or point the build at it explicitly:

  .. code-block:: bash

     export CGT7X_ROOT=/path/to/ti-cgt-c7000_<version>

Available Presets
-----------------

.. list-table::
   :header-rows: 1
   :widths: 40 15 45

   * - Preset
     - SoC
     - Platform
   * - ``release-autotest-am62d-pc``
     - AM62D
     - PC simulation + tests
   * - ``release-autotest-am62d-target``
     - AM62D
     - On-device + tests
   * - ``release-autotest-am275-pc``
     - AM275
     - PC simulation + tests
   * - ``release-autotest-am275-target``
     - AM275
     - On-device + tests
   * - ``release-buildlib-am62d-pc``
     - AM62D
     - PC (library only)
   * - ``release-buildlib-am62d-target``
     - AM62D
     - On-device (library only)
   * - ``release-buildlib-am275-pc``
     - AM275
     - PC (library only)
   * - ``release-buildlib-am275-target``
     - AM275
     - On-device (library only)

Quick Start — PC simulation (AM62D)
-----------------------------------

.. code-block:: bash

   # 1. Configure
   cmake -S . -B build --preset=release-autotest-am62d-pc

   # 2. Populate test-vector headers — one-time step (needs Python; see
   #    "Generating Test Data" below). Reused by later builds.
   cmake --build build --target=gen_all_test_case_headers

   # 3. Build library + test binaries
   cmake --build build -- -j$(nproc)

On-Target Build (AM62D)
-----------------------

.. code-block:: bash

   cmake -S . -B build --preset=release-autotest-am62d-target
   cmake --build build --target=gen_all_test_case_headers
   cmake --build build -- -j$(nproc)

Library-Only Build
------------------

.. code-block:: bash

   cmake -S . -B build --preset=release-buildlib-am62d-pc
   cmake --build build -- -j$(nproc)

Custom Configurations
---------------------

The shared, tracked presets live in ``CMakePresets.json`` and expose hidden
per-device base presets: ``base-am62d-pc``, ``base-am62d-target``,
``base-am275-pc``, ``base-am275-target``. To define your own configurations,
create a git-ignored ``CMakeUserPresets.json`` that **composes** a device base
with your build-type and test settings via ``inherits``:

.. code-block:: json

   {
     "version": 3,
     "configurePresets": [
       {
         "name": "base-test",
         "hidden": true,
         "cacheVariables": {
           "BUILD_TEST": "1",
           "KERNEL_NAME": "DSPLIB_fir",
           "ALL_TEST_CASES": "1"
         }
       },
       {
         "name": "base-release",
         "hidden": true,
         "inherits": "base-test",
         "cacheVariables": { "CMAKE_BUILD_TYPE": "Release" }
       },
       {
         "name": "release-am62d-pc",
         "inherits": ["base-am62d-pc", "base-release"],
         "displayName": "Release AM62D (PC)",
         "description": "Release build for AM62D PC platform"
       }
     ]
   }

Each concrete preset layers a device base (from ``CMakePresets.json``) with your
own ``base-release`` / ``base-debug`` settings. Configure with it directly:

.. code-block:: bash

   cmake -S . -B build --preset=release-am62d-pc

Set ``KERNEL_NAME`` to target one kernel's tests, and ``ALL_TEST_CASES`` to build
the full suite. To build a single test case instead, replace ``ALL_TEST_CASES``
with ``TEST_CASE`` set to the desired test-case number, e.g.
``"TEST_CASE": "2"``.

Generating Test Data (Static Headers)
-------------------------------------

The per-kernel test vectors are **not packaged** in the repository. Before
building an autotest preset you must generate them once, after configuring:

.. code-block:: bash

   cmake --build build --target=gen_all_test_case_headers

This runs the Python generators under
``test/DSPLIB_idat_gen/<kernel>/gen_data.py`` for every kernel, emitting the
static data headers (``test/<kernel>/<kernel>_idat.c``) that the test drivers
compile against. The headers are written into the **source tree**, so this is a
**one-time** step — they are reused by later builds and reconfigures. You only
need to regenerate when changing the test vectors themselves (by editing a
kernel's ``test_cases_list.csv``), an advanced workflow that is out of scope here.

Because the vectors are generated rather than shipped, the generators require
Python. Use the same versions as the CI regression environment:

- ``numpy==1.26.4`` — pinned exactly. numpy 2.x is *not* supported (API changes
  break the generators).
- ``scipy``
- ``scikit-learn``

.. code-block:: bash

   pip install numpy==1.26.4 scipy scikit-learn

Running
-------

PC-simulation test binaries are emitted under ``bin/<build-type>/``, where
``<build-type>`` is the lowercase ``CMAKE_BUILD_TYPE`` — the release presets
produce ``bin/release/``:

.. code-block:: bash

   bin/release/test_DSPLIB_<kernel>_<device>_x86_64

For on-target runs, load the resulting ``.out`` onto the EVM (No Boot Mode) to
reproduce the datasheet numbers.

.. note::

   When switching between target and PC-simulation builds, delete the ``build``
   directory before reconfiguring to avoid stale CMake cache state.
