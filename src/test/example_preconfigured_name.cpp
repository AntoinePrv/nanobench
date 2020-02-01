#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

#include <iostream>

// Demonstrates a very simple benchmark that evalutes the performance of a CAS operation using std::atomic.
// On my system, this prints:
//
// | relative |               ns/op |                op/s |   MdAPE | benchmark
// |---------:|--------------------:|--------------------:|--------:|:----------------------------------------------
// |          |                5.63 |      177,553,749.61 |    0.0% | `compare_exchange_strong`
//
// example from https://github.com/cameron314/microbench
TEST_CASE("preconfigured_name") {
    ankerl::nanobench::Config cfg;
    cfg.name("start & stop 1").run([] { std::vector<int> v = {{11, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7}}; });

    cfg.name("run2");
    cfg.run([] { std::vector<int> v = {{11, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7}}; });
}

TEST_CASE("keep_only_results") {

    std::vector<ankerl::nanobench::Result> results;
    ankerl::nanobench::Config cfg;
    ankerl::nanobench::Rng rng(123);

    cfg.run("a", [&] { rng(); });
    results.insert(results.end(), cfg.results().begin(), cfg.results().end());
}