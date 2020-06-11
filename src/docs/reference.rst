=============
API Reference
=============

.. How to link: https://breathe.readthedocs.io/en/latest/domains.html
   E.g. :cpp:class:`ankerl::nanobench::Bench`

----------------------------------------------------------------
:cpp:class:`Bench <ankerl::nanobench::Bench>` - Main Entry Point
----------------------------------------------------------------


.. doxygenclass:: ankerl::nanobench::Bench
    :members:



---------------------------------------------------------------
:cpp:class:`Rng <ankerl::nanobench::Rng>` - Extremely fast PRNG
---------------------------------------------------------------

.. doxygenclass:: ankerl::nanobench::Rng
    :members:



-------------------------------------------------------------------
:cpp:class:`Result <ankerl::nanobench::Result>` - Benchmark Results
-------------------------------------------------------------------

.. doxygenclass:: ankerl::nanobench::Result
    :members:



----------------------------------------------------------------------
:cpp:func:`doNotOptimizeAway() <ankerl::nanobench::doNotOptimizeAway>`
----------------------------------------------------------------------

.. doxygenfunction:: ankerl::nanobench::doNotOptimizeAway()



--------------------------------------------------------------------------
:cpp:func:`render() <ankerl::nanobench::render>` - Mustache-like Templates
--------------------------------------------------------------------------


.. doxygenfunction:: ankerl::nanobench::render


:cpp:func:`templates::csv <ankerl::nanobench::templates::csv>`
--------------------------------------------------------------

.. doxygenfunction:: ankerl::nanobench::templates::csv



:cpp:func:`templates::htmlBoxplot <ankerl::nanobench::templates::htmlBoxplot>`
------------------------------------------------------------------------------

.. doxygenfunction:: ankerl::nanobench::templates::htmlBoxplot



:cpp:func:`templates::json <ankerl::nanobench::templates::json>`
----------------------------------------------------------------

.. doxygenfunction:: ankerl::nanobench::templates::json


---------------------
Environment Variables
---------------------

``NANOBENCH_ENDLESS``
---------------------

Sometimes it helps to run a benchmark for a very long time, so that it's possible to attach with a profiler like
`perf <https://perf.wiki.kernel.org/index.php/Main_Page>`_ and get meaningful statistics. This can be done with the environment variable
``NANOBENCH_ENDLESS``. E.g. to run the benchmark with the name ``x += x`` endlessly, call the app this way:

.. code-block:: sh

   NANOBENCH_ENDLESS="x += x" ./yourapp

When your app runs it will run all benchmark normally, but when it encounters a benchmarked named ``x += x``, it will run this one endlessly.
It will print in nice friendly letters 

.. code-block:: text

   NANOBENCH_ENDLESS set: running 'x += x' endlessly`` once it reaches that state.


.. warning::

    For optimal profiling with ``perf``, you shouldn't use ``pyperf system tune`` in the endless mode. PyPerf dramatically reduces the
    number of events that can be captured per second. This is a good to get accurate benchmark numbers from nanobench, but a bad when
    you actually want to use perf to analyze hotspots.



---------------
Generated Index
---------------

Index for the nanobench API: :ref:`genindex`. 
