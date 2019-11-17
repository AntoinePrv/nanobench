#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

#include <deque>
#include <iostream>
#include <list>
#include <vector>

namespace {

template <typename ContainerT>
static void testBenchSet(std::string const& label, int n, ankerl::nanobench::Config& cfg) {
    cfg.run(label, [&] {
        ContainerT items;
        for (int i = 0; i < n; ++i) {
            items.push_back(i);
        }
        ankerl::nanobench::doNotOptimizeAway(&items);
    });
}

void exampleCsv(bool useCsv) {
    ankerl::nanobench::Config cfg;
    if (useCsv) {
        cfg.output(nullptr);
    }

    for (int n = 100; n <= 10000; n *= 10) {
        cfg.title("Size " + std::to_string(n));
        testBenchSet<std::vector<int>>("std::vector<int>", n, cfg);
        testBenchSet<std::deque<int>>("std::deque<int>", n, cfg);
        testBenchSet<std::list<int>>("std::list<int>", n, cfg);
        if (useCsv) {
            // could also use ankerl::nanobench::templates::csv() which contains a header
            cfg.render(
                R"({{#benchmarks}}"{{name}}"; {{relative}}; {{median_sec_per_unit}}; {{min}}; {{max}}; {{md_ape}}; {{num_measurements}}; {{median_ins_per_unit}}; {{median_branches_per_unit}}; {{median_branchmisses_per_unit}}
{{/benchmarks}})",
                std::cout);
        }
    }
}

} // namespace

TEST_CASE("example_csv_csv") {
    exampleCsv(true);
}

TEST_CASE("example_csv_md") {
    exampleCsv(false);
}
