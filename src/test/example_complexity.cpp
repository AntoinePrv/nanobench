#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <set>

TEST_CASE("example_complexity_set") {
    ankerl::nanobench::Bench bench;

    ankerl::nanobench::Rng rng;
    for (size_t range = 10; range <= 1000; range = range * 3 / 2) {
        // create vector with random data
        std::set<uint64_t> set;
        for (size_t i = 0; i < range; ++i) {
            set.insert(rng());
        }

        bench.complexityN(range).run("std::set find " + std::to_string(range),
                                     [&] { ankerl::nanobench::doNotOptimizeAway(set.find(rng())); });
    }
    std::cout << bench.complexityBigO() << std::endl;
}

TEST_CASE("example_complexity_sort") {
    ankerl::nanobench::Rng rng;
    ankerl::nanobench::Bench bench;
    for (size_t n = 10; n < 10000; n *= 2) {
        // prepare a set with range number of elements
        std::vector<uint64_t> data(n);
        for (size_t i = 0; i < n; ++i) {
            data[i] = rng();
        }

        // sort should be O(n log n), shuffle is O(n), so we expect O(n log n).
        bench.complexityN(n).run("std::sort " + std::to_string(n), [&] {
            std::shuffle(data.begin(), data.end(), rng);
            std::sort(data.begin(), data.end());
        });
    }

    // calculates bigO of all preconfigured complexity functions
    std::cout << bench.complexityBigO() << std::endl;

    // calculates bigO for a custom function
    auto logLogN = bench.complexityBigO("O(log log n)", [](double n) { return std::log2(std::log2(n)); });
    std::cout << logLogN << std::endl;
}

TEST_CASE("example_complexity_quadratic") {
    // create an ankerl::nanobench::Config object that is used in all the benchmarks
    ankerl::nanobench::Bench bench;
    ankerl::nanobench::Rng rng;

    // run the same benchmark multiple times with different ranges
    for (size_t range = 10; range <= 1000; range *= 2) {
        // create vector with random data
        std::vector<double> vec(range, 0.0);
        for (auto& x : vec) {
            x = rng.uniform01();
        }

        // each run is configured with complexityN(range) to specify the run's input N
        bench.complexityN(range).run("minimum pair " + std::to_string(range), [&] {
            // Actual algorithm we want to evaluate
            double minVal = std::numeric_limits<double>::max();
            for (size_t i = 0; i < vec.size() - 1; ++i) {
                for (size_t j = i + 1; j < vec.size(); ++j) {
                    auto diff = vec[i] - vec[j];
                    minVal = std::min(minVal, diff * diff);
                }
            }
            ankerl::nanobench::doNotOptimizeAway(minVal);
        });
    }

    // after all the runs are done, calculate the BigO, and show the results
    std::cout << bench.complexityBigO() << std::endl;
}
