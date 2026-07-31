DSPLIB — C7x DSP Library
=========================

DSPLIB is TI's optimized digital signal processing library for the **C7™+MMA** DSP architecture.
It provides a suite of kernels targeting the C7™ vector and streaming engines (SE/SA) and the
Matrix Multiply Accelerator (MMA), enabling high-throughput signal processing and linear algebra
on TI SoCs such as AM62D and AM275.

Each kernel ships in two variants selected at init time: a C-native reference (``_cn``, portable
and functionally correct) and a C7™ intrinsic-optimized variant (``_ci``, hardware-optimized for
SE/SA streaming engines).

.. toctree::
   :maxdepth: 2
   :caption: Contents

   getting_started
   repository_layout
   api/index
   perf/index
   license
