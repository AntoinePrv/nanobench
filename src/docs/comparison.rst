================================
Comparison with Other Frameworks
================================

I've implemented the three different benchmarks: Fast, Slow, Fluctuating in several frameworks for comparison.

Fast
    Benchmarks ``x += x``, starting from 1. This is a single instruction, and prone to be optimized away.

Slow
    Benchmarks ``std::this_thread::sleep_for(std::chrono::milliseconds(10))``. For a microbenchmark this
    is very slow, and it is interesting how the framework's autotuning deals with this.

Fluctuating
    A microbenchark that is tuned so that each evaluation takes a different time. This is fluctuating runtime
    is achieved by randomly producing 0-255 random numbers with `std::mt19937_64`.


All benchmarks are run on an i7-8700 CPU locked at 3.2GHz, using
`pyperf system tune <https://pyperf.readthedocs.io/en/latest/system.html>`_.



--------
Runtimes
--------

I wrote a little timing tool that measures how long exactly it takes to print benchmark output to the screen. 
With this I have measured the runtimes of major benchmarking frameworks which support automatic tuning of
the number of iterations: :ref:`Google Benchmark <comparison-google-benchmark>`, :ref:`Catch2 <comparison-catch2>`,
:ref:`nonius <comparison-nonius>`, and of course :ref:`nanobench <comparison-nanobench>`.

.. image:: _static/totalruntime.svg

======================  =====  ======   =========== ======== ========
Benchmarking Framework  Fast   Slow     Fluctuating Overhead total
======================  =====  ======   =========== ======== ========
Google Benchmark        0.367  11.259    0.825       0.000    12.451
Catch2                  1.004   2.074    0.966       1.737     5.782
nonius                  0.741   1.815    0.740       1.715     5.010
nanobench               0.079   0.112    0.000       0.001     0.192
======================  =====  ======   =========== ======== ========

Nanobench is clearly the fastest autotuning benchmarking framework, by an enormous margin.

-------------------------
Implementations & Results
-------------------------

.. _`comparison-nanobench`:


nanobench
=========


Sourcecode
----------

.. literalinclude:: ../comparisons/nanobench/main.cpp
    :language: c++
    :linenos:


Results
-------

.. literalinclude:: ../comparisons/nanobench/out.txt
    :language: text


.. _`comparison-google-benchmark`:


Google Benchmark
================

Very feature rich, battle proven, but a bit aged. Requires
google test. Get it here: `Google Benchmark <https://github.com/google/benchmark>`_

Sourcecode
----------

.. literalinclude:: ../comparisons/google_benchmark/gbench.cpp
    :language: c++
    :linenos:

Results
-------

Compiled & linked with 

.. code-block:: sh

   g++ -O2 main.cpp -L/home/martinus/git/benchmark/build/src -lbenchmark -lpthread -o gbench

executing it gives this result:

.. literalinclude:: ../comparisons/google_benchmark/out.txt
    :language: text


Running the tests individually takes 0.365s, 11.274 sec, 0.828sec.


.. _`comparison-nonius`:


nonius
======

It gives lots of statistics, but seems a bit complicated to me. Not as straight forward as I'd like it. It shows lots of statistics, which makes the output a bit hard to read. I am not sure if it is still actively maintained. The homepage has been down for a while.
Get it here: `nonius <https://github.com/libnonius/nonius>`_

Sourcecode
----------

.. literalinclude:: ../comparisons/libnonius_nonius/main.cpp
    :language: c++
    :linenos:

Results
-------

.. literalinclude:: ../comparisons/libnonius_nonius/out.txt
    :language: text

The tests individually take 0.713sec, 1.883sec, 0.819sec. Plus a startup overhead of 1.611sec.



Picobench
=========

It took me a while to figure out that I have to configure the slow test, otherwise it would
run for a looong time. The number of iterations is hardcoded, this library seems very basic. Get it here:
`picobench <https://github.com/iboB/picobench>`_

Sourcecode
----------

.. literalinclude:: ../comparisons/iboB_picobench/picobench.cpp
    :language: c++
    :linenos:

Results
-------

.. literalinclude:: ../comparisons/iboB_picobench/out.txt
    :language: text

It doesn't really make sense to provide runtime numbers here, because picobench just executes the given number of iterations, and that's it. No autotuning.


.. _`comparison-catch2`:


Catch2
======

Catch2 is mostly a unit testing framework, and has recently integrated benchmarking faciliy. It is very easy to use,
but does not seem too configurable. I find the way it writes the output very confusing. Get it here:
`Catch2 <https://github.com/catchorg/Catch2>`_

Sourcecode
----------

.. literalinclude:: ../comparisons/catchorg_Catch2/catch.cpp
    :language: c++
    :linenos:

Results
-------

.. literalinclude:: ../comparisons/catchorg_Catch2/out.txt
    :language: text



moodycamel::microbench
======================

A very simple benchmarking tool, and an API that's very similar to ``ankerl::nanobench``. No autotuning,
no doNotOptimize, no output formatting. Get it here: `moodycamel::microbench <https://github.com/cameron314/microbench>`_

Sourcecode
----------


.. literalinclude:: ../comparisons/cameron314_microbench/microbench.cpp
    :language: c++
    :linenos:



Results
-------

.. literalinclude:: ../comparisons/cameron314_microbench/out.txt
    :language: text



Celero
======

Unfortunately I couldn't get it working. I only got segmentation faults for my ``x += x`` benchmarks.
Get it here: `celero <https://github.com/DigitalInBlue/Celero>`_


folly Benchmark
===============

Facebook's folly comes with benchmarking facility. It seems rather basic, but with good ``DoNotOptimizeAway``
functionality. Honestly, I was too lazy to get this working. Too much installation hazzle. Get it here:
`folly <https://github.com/facebook/folly>`_

