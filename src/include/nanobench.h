//  __   _ _______ __   _  _____  ______  _______ __   _ _______ _     _
//  | \  | |_____| | \  | |     | |_____] |______ | \  | |       |_____|
//  |  \_| |     | |  \_| |_____| |_____] |______ |  \_| |_____  |     |
//
// Microbenchmark framework for C++11/14/17/20
// https://github.com/martinus/nanobench
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 Martin Ankerl <http://martin.ankerl.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef ANKERL_NANOBENCH_H_INCLUDED
#define ANKERL_NANOBENCH_H_INCLUDED

// see https://semver.org/
#define ANKERL_NANOBENCH_VERSION_MAJOR 1 // incompatible API changes
#define ANKERL_NANOBENCH_VERSION_MINOR 0 // backwards-compatible changes
#define ANKERL_NANOBENCH_VERSION_PATCH 0 // backwards-compatible bug fixes

///////////////////////////////////////////////////////////////////////////////////////////////////
// public facing api - as minimal as possible
///////////////////////////////////////////////////////////////////////////////////////////////////

#define ANKERL(x) ANKERL_PRIVATE_##x()
#define ANKERL_PRIVATE_CXX() __cplusplus
#define ANKERL_PRIVATE_CXX98() 199711L
#define ANKERL_PRIVATE_CXX11() 201103L
#define ANKERL_PRIVATE_CXX14() 201402L
#define ANKERL_PRIVATE_CXX17() 201703L

#if ANKERL(CXX) >= ANKERL(CXX17)
#    define ANKERL_PRIVATE_NODISCARD() [[nodiscard]]
#else
#    define ANKERL_PRIVATE_NODISCARD()
#endif

#include <chrono>
#include <string>

// declarations ///////////////////////////////////////////////////////////////////////////////////

namespace ankerl {
namespace nanobench {

using Clock = std::chrono::high_resolution_clock;
class Config;
class Result;
class Rng;

namespace detail {

class IterationLogic;

} // namespace detail
} // namespace nanobench
} // namespace ankerl

// definitions ////////////////////////////////////////////////////////////////////////////////////

namespace ankerl {
namespace nanobench {

// Result returned after a benchmark has finished. Can be used as a baseline for relative().
class Result {
public:
    Result(std::string u, std::chrono::duration<double> const* secPerUnit, size_t size) noexcept;
    Result() noexcept;

    ANKERL(NODISCARD) std::string const& unit() const noexcept;
    ANKERL(NODISCARD) std::chrono::duration<double> median() const noexcept;
    ANKERL(NODISCARD) double medianAbsolutePercentError() const noexcept;
    ANKERL(NODISCARD) std::chrono::duration<double> minimum() const noexcept;
    ANKERL(NODISCARD) std::chrono::duration<double> maximum() const noexcept;

    // Convenience: makes sure none of the given arguments are optimized away by the compiler.
    template <typename... Args>
    Result& doNotOptimizeAway(Args&&... args);

private:
    std::string mUnit{};
    std::chrono::duration<double> mMedian{};
    double mMedianAbsolutePercentError{};
    std::chrono::duration<double> mMinimum{};
    std::chrono::duration<double> mMaximum{};
};

// Small Fast Counting RNG, version 4
class Rng final {
public:
    using result_type = uint64_t;

    static constexpr uint64_t(min)();
    static constexpr uint64_t(max)();

    Rng();

    // don't allow copying, it's dangerous
    Rng(Rng const&) = delete;
    Rng& operator=(Rng const&) = delete;

    // moving is ok
    Rng(Rng&&) noexcept = default;
    Rng& operator=(Rng&&) noexcept = default;
    ~Rng() noexcept = default;

    explicit Rng(uint64_t seed) noexcept;
    ANKERL(NODISCARD) Rng copy() const noexcept;
    void assign(Rng const& other) noexcept;

    // that one's inline so it is fast
    inline uint64_t operator()() noexcept;
    // random double in range [0, 1(
    inline double uniform01() noexcept;

private:
    static constexpr uint64_t rotl(uint64_t x, unsigned k) noexcept;

    uint64_t mA;
    uint64_t mB;
    uint64_t mC;
    uint64_t mCounter;
};

// Configuration of a microbenchmark.
class Config {
public:
    Config();

    Config(Config&& other) noexcept;
    Config& operator=(Config&& other) noexcept;

    Config(Config const& other);
    Config& operator=(Config const& other);

    ~Config() noexcept;

    // Set the batch size, e.g. number of processed bytes, or some other metric for the size of the processed data in each iteration.
    // Any argument is cast to double.
    template <typename T>
    Config& batch(T b) noexcept;
    ANKERL(NODISCARD) double batch() const noexcept;

    // Set a baseline to compare it to. 100% it is exactly as fast as the baseline, >100% means it is faster than the baseline, <100%
    // means it is slower than the baseline.
    Config& relative(Result const& baseline) noexcept;
    ANKERL(NODISCARD) Result const& relative() const noexcept;

    // Operation unit. Defaults to "op", could be e.g. "byte" for string processing.
    // Use singular (byte, not bytes).
    Config& unit(std::string unit);
    ANKERL(NODISCARD) std::string const& unit() const noexcept;

    Config& title(std::string benchmarkTitle);
    ANKERL(NODISCARD) std::string const& title() const noexcept;

    // Number of epochs to evaluate. The reported result will be the median of evaluation of each epoch.
    Config& epochs(size_t numEpochs) noexcept;
    ANKERL(NODISCARD) size_t epochs() const noexcept;

    // Desired evaluation time is a multiple of clock resolution. Default is to be 1000 times above this measurement precision.
    Config& clockResolutionMultiple(size_t multiple) noexcept;
    ANKERL(NODISCARD) size_t clockResolutionMultiple() const noexcept;

    // Sets the maximum time each epoch should take. Default is 100ms.
    Config& maxEpochTime(std::chrono::nanoseconds t) noexcept;
    ANKERL(NODISCARD) std::chrono::nanoseconds maxEpochTime() const noexcept;

    Config& warmup(size_t numWarmupIters) noexcept;
    ANKERL(NODISCARD) size_t warmup() const noexcept;

    // Performs all evaluations.
    template <typename Op>
    Result run(std::string const& name, Op op) const;

private:
    std::string mBenchmarkTitle = "benchmark";
    std::string mUnit = "op";
    double mBatch = 1.0;
    size_t mNumEpochs = 51;
    size_t mClockResolutionMultiple = static_cast<size_t>(1000);
    std::chrono::nanoseconds mMaxEpochTime = std::chrono::milliseconds(100);
    Result mRelative{};
    size_t mWarmup = 0;
};

// Makes sure none of the given arguments are optimized away by the compiler.
template <typename... Args>
void doNotOptimizeAway(Args&&... args);

namespace detail {

#if defined(_MSC_VER)
void doNotOptimizeAwaySink(void const*);
#else
template <typename T>
void doNotOptimizeAway(T& value);
#endif

template <typename T>
void doNotOptimizeAway(T const& val);

// internally used, but visible because run() is templated
#if defined(__clang__)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wpadded"
#endif
class IterationLogic {
public:
    IterationLogic(Config const& config, std::string name) noexcept;
    ~IterationLogic() noexcept;

    IterationLogic(IterationLogic const&) = delete;
    IterationLogic& operator=(IterationLogic const&) = delete;
    IterationLogic(IterationLogic&&) = delete;
    IterationLogic& operator=(IterationLogic&&) = delete;

    ANKERL(NODISCARD) size_t numIters() const noexcept;
    void add(std::chrono::nanoseconds elapsed) noexcept;
    ANKERL(NODISCARD) Result const& result() const;

private:
    ANKERL(NODISCARD) Result showResult(std::string const& errorMessage) const;

    Config const& mConfig;
    std::chrono::nanoseconds mTargetRuntime{};
    // we don't use a vector here so we can get away with not including vector in the public interface
    std::chrono::duration<double>* mSecPerUnit = nullptr;
    size_t mSecPerUnitIndex = 0;
    size_t mNumIters = 0;
    std::string mName;
    Result mResult{};
    Rng mRng{};
    bool mIsWarmup = false;
    bool mIsTargetElapsedReached = false;
};
#if defined(__clang__)
#    pragma clang diagnostic pop
#endif

} // namespace detail
} // namespace nanobench
} // namespace ankerl

// implementation /////////////////////////////////////////////////////////////////////////////////

namespace ankerl {
namespace nanobench {

// Small Fast Counting RNG, version 4
constexpr uint64_t(Rng::min)() {
    return 0;
}

constexpr uint64_t(Rng::max)() {
    return (std::numeric_limits<uint64_t>::max)();
}

uint64_t Rng::operator()() noexcept {
    uint64_t tmp = mA + mB + mCounter++;
    mA = mB ^ (mB >> 11U);
    mB = mC + (mC << 3U);
    mC = rotl(mC, 24U) + tmp;
    return tmp;
}

// see http://prng.di.unimi.it/
double Rng::uniform01() noexcept {
    union {
        uint64_t i;
        double d;
    } x{};
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
    x.i = (UINT64_C(0x3ff) << 52U) | (operator()() >> 12U);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
    return x.d - 1.0;
}

constexpr uint64_t Rng::rotl(uint64_t x, unsigned k) noexcept {
    return (x << k) | (x >> (64U - k));
}

// Performs all evaluations.
template <typename Op>
Result Config::run(std::string const& name, Op op) const {
    // It is important that this method is kept short so the compiler can do better optimizations/ inlining of op()
    detail::IterationLogic iterationLogic(*this, name);

    while (auto n = iterationLogic.numIters()) {
        Clock::time_point before = Clock::now();
        while (n-- > 0) {
            op();
        }
        Clock::time_point after = Clock::now();
        iterationLogic.add(after - before);
    }
    return iterationLogic.result();
}

// Set the batch size, e.g. number of processed bytes, or some other metric for the size of the processed data in each iteration.
// Any argument is cast to double.
template <typename T>
Config& Config::batch(T b) noexcept {
    mBatch = static_cast<double>(b);
    return *this;
}

// Convenience: makes sure none of the given arguments are optimized away by the compiler.
template <typename... Args>
Result& Result::doNotOptimizeAway(Args&&... args) {
    (void)std::initializer_list<int>{(detail::doNotOptimizeAway(std::forward<Args>(args)), 0)...};
    return *this;
}

// Makes sure none of the given arguments are optimized away by the compiler.
template <typename... Args>
void doNotOptimizeAway(Args&&... args) {
    (void)std::initializer_list<int>{(detail::doNotOptimizeAway(std::forward<Args>(args)), 0)...};
}

namespace detail {

#if defined(_MSC_VER)
template <typename T>
void doNotOptimizeAway(T const& val) {
    doNotOptimizeAwaySink(&val);
}

#else

template <typename T>
void doNotOptimizeAway(T const& val) {
    // NOLINTNEXTLINE(hicpp-no-assembler)
    asm volatile("" : : "r,m"(val) : "memory");
}

template <typename T>
void doNotOptimizeAway(T& value) {
#    if defined(__clang__)
    // NOLINTNEXTLINE(hicpp-no-assembler)
    asm volatile("" : "+r,m"(value) : : "memory");
#    else
    // NOLINTNEXTLINE(hicpp-no-assembler)
    asm volatile("" : "+m,r"(value) : : "memory");
#    endif
}
#endif

} // namespace detail
} // namespace nanobench
} // namespace ankerl

#ifdef ANKERL_NANOBENCH_IMPLEMENT

///////////////////////////////////////////////////////////////////////////////////////////////////
// implementation part
///////////////////////////////////////////////////////////////////////////////////////////////////

#    include <algorithm> // sort
#    include <cstdlib>   // getenv
#    include <fstream>   // ifstream to parse proc files
#    include <iomanip>   // setw, setprecision
#    include <iostream>  // cout
#    include <vector>    // manage results
#    if defined(__linux__)
#        include <unistd.h> //sysconf
#    endif

// declarations ///////////////////////////////////////////////////////////////////////////////////

namespace ankerl {
namespace nanobench {

// helper stuff that only intended to be used internally
namespace detail {

struct TableInfo;

// formatting utilities
namespace fmt {

class NumSep;
class StreamStateRestorer;
class Number;
class MarkDownCode;

} // namespace fmt
} // namespace detail
} // namespace nanobench
} // namespace ankerl

// definitions ////////////////////////////////////////////////////////////////////////////////////

namespace ankerl {
namespace nanobench {

// helper stuff that only intended to be used internally
namespace detail {

template <typename T>
T parseFile(std::string const& filename);

void printStabilityInformationOnce();

// remembers the last table settings used. When it changes, a new table header is automatically written for the new entry.
uint64_t& singletonLastTableSettingsHash() noexcept;
uint64_t calcTableSettingsHash(std::string const& unit, std::string const& title) noexcept;

// determines resolution of the given clock. This is done by measuring multiple times and returning the minimum time difference.
Clock::duration calcClockResolution(size_t numEvaluations) noexcept;

// Calculates clock resolution once, and remembers the result
inline Clock::duration clockResolution() noexcept;

// calculates median, and properly handles even number of elements too.
template <typename T>
T calcMedian(T const* sortedResults, size_t size) noexcept;

// calculates MdAPE which is the median of percentage error
// see https://www.spiderfinancial.com/support/documentation/numxl/reference-manual/forecasting-performance/mdape
double calcMedianAbsolutePercentageError(std::chrono::duration<double> const* results, size_t size,
                                         std::chrono::duration<double> median) noexcept;

// formatting utilities
namespace fmt {

// adds thousands separator to numbers
#    if defined(__clang__)
#        pragma clang diagnostic push
#        pragma clang diagnostic ignored "-Wpadded"
#    endif
class NumSep : public std::numpunct<char> {
public:
    explicit NumSep(char sep);
    char do_thousands_sep() const override;
    std::string do_grouping() const override;

private:
    char mSep;
};
#    if defined(__clang__)
#        pragma clang diagnostic pop
#    endif

// RAII to save & restore a stream's state
#    if defined(__clang__)
#        pragma clang diagnostic push
#        pragma clang diagnostic ignored "-Wpadded"
#    endif
class StreamStateRestorer {
public:
    explicit StreamStateRestorer(std::ostream& s);
    ~StreamStateRestorer();

    // sets back all stream info that we remembered at construction
    void restore();

    // don't allow copying / moving
    StreamStateRestorer(StreamStateRestorer const&) = delete;
    StreamStateRestorer& operator=(StreamStateRestorer const&) = delete;
    StreamStateRestorer(StreamStateRestorer&&) = delete;
    StreamStateRestorer& operator=(StreamStateRestorer&&) = delete;

private:
    std::ostream& mStream;
    std::locale mLocale;
    std::streamsize const mPrecision;
    std::streamsize const mWidth;
    std::ostream::char_type const mFill;
    std::ostream::fmtflags const mFmtFlags;
};
#    if defined(__clang__)
#        pragma clang diagnostic pop
#    endif

// Number formatter
class Number {
public:
    Number(int width, int precision, double value);

private:
    friend std::ostream& operator<<(std::ostream& os, Number const& n);
    std::ostream& write(std::ostream& os) const;

    int mWidth;
    int mPrecision;
    double mValue;
};

std::ostream& operator<<(std::ostream& os, Number const& n);

// Formats any text as markdown code, escaping backticks.
class MarkDownCode {
public:
    explicit MarkDownCode(std::string const& what);

private:
    friend std::ostream& operator<<(std::ostream& os, MarkDownCode const& mdCode);
    std::ostream& write(std::ostream& os) const;

    std::string mWhat{};
};

std::ostream& operator<<(std::ostream& os, MarkDownCode const& mdCode);

} // namespace fmt
} // namespace detail
} // namespace nanobench
} // namespace ankerl

// implementation /////////////////////////////////////////////////////////////////////////////////

namespace ankerl {
namespace nanobench {
namespace detail {

// Windows version of do not optimize away
// see https://github.com/google/benchmark/blob/master/include/benchmark/benchmark.h#L307
// see https://github.com/facebook/folly/blob/master/folly/Benchmark.h#L280
// see https://docs.microsoft.com/en-us/cpp/preprocessor/optimize
#    if defined(_MSC_VER)
#        pragma optimize("", off)
void doNotOptimizeAwaySink(void const*) {}
#        pragma optimize("", on)
#    endif

template <typename T>
T parseFile(std::string const& filename) {
    std::ifstream fin(filename);
    T num{};
    fin >> num;
    return num;
}

void printStabilityInformationOnce() {
    static bool shouldPrint = true;
    if (shouldPrint) {
        shouldPrint = false;
#    if !defined(NDEBUG)
        std::cerr << "Warning: NDEBUG not defined, this is a debug build" << std::endl;
#    endif

#    if defined(__linux__)
        auto nprocs = sysconf(_SC_NPROCESSORS_CONF);
        if (nprocs <= 0) {
            std::cerr << "Warning: Can't figure out number of processors." << std::endl;
            return;
        }

        // check frequency scaling
        bool isFrequencyLocked = true;
        bool isGovernorPerformance = "performance" == parseFile<std::string>("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");
        for (long id = 0; id < nprocs; ++id) {
            auto sysCpu = "/sys/devices/system/cpu/cpu" + std::to_string(id);
            auto minFreq = parseFile<int64_t>(sysCpu + "/cpufreq/scaling_min_freq");
            auto maxFreq = parseFile<int64_t>(sysCpu + "/cpufreq/scaling_max_freq");
            if (minFreq != maxFreq) {
                isFrequencyLocked = false;
            }
        }
        bool isTurbo = 0 == parseFile<int>("/sys/devices/system/cpu/intel_pstate/no_turbo");
        if (!isFrequencyLocked) {
            std::cerr << "Warning: CPU frequency scaling enabled, results will be invalid" << std::endl;
        }
        if (!isGovernorPerformance) {
            std::cerr << "Warning: CPU governor is not performance, results will be invalid" << std::endl;
        }
        if (isTurbo) {
            std::cerr << "Warning: Turbo is enabled" << std::endl;
        }

        if (!isFrequencyLocked || !isGovernorPerformance || isTurbo) {
            std::cerr << "Recommendation: use 'pyperf system tune' before benchmarking. See https://pypi.org/project/pyperf/"
                      << std::endl;
        }
#    endif
    }
}

// remembers the last table settings used. When it changes, a new table header is automatically written for the new entry.
uint64_t& singletonLastTableSettingsHash() noexcept {
    static uint64_t sTableSettingHash = {};
    return sTableSettingHash;
}

inline uint64_t fnv1a(std::string const& str) noexcept {
    auto val = UINT64_C(14695981039346656037);
    for (auto c : str) {
        val = (val ^ static_cast<uint8_t>(c)) * UINT64_C(1099511628211);
    }
    return val;
}

inline void hash_combine(uint64_t* seed, uint64_t val) {
    *seed ^= val + UINT64_C(0x9e3779b9) + (*seed << 6U) + (*seed >> 2U);
}

inline uint64_t calcTableSettingsHash(Config const& cfg) noexcept {
    uint64_t h = 0;
    hash_combine(&h, fnv1a(cfg.unit()));
    hash_combine(&h, fnv1a(cfg.title()));
    return h;
}

// determines resolution of the given clock. This is done by measuring multiple times and returning the minimum time difference.
Clock::duration calcClockResolution(size_t numEvaluations) noexcept {
    auto bestDuration = Clock::duration::max();
    Clock::time_point tBegin;
    Clock::time_point tEnd;
    for (size_t i = 0; i < numEvaluations; ++i) {
        tBegin = Clock::now();
        do {
            tEnd = Clock::now();
        } while (tBegin == tEnd);
        bestDuration = (std::min)(bestDuration, tEnd - tBegin);
    }
    return bestDuration;
}

// Calculates clock resolution once, and remembers the result
Clock::duration clockResolution() noexcept {
    static Clock::duration sResolution = calcClockResolution(20);
    return sResolution;
}

// calculates median, and properly handles even number of elements too.
template <typename T>
T calcMedian(T const* sortedResults, size_t size) noexcept {
    auto mid = size / 2U;
    if (size & 1U) {
        return sortedResults[mid];
    }
    return (sortedResults[mid - 1U] + sortedResults[mid]) / 2U;
}

// calculates MdAPE which is the median of percentage error
// see https://www.spiderfinancial.com/support/documentation/numxl/reference-manual/forecasting-performance/mdape
double calcMedianAbsolutePercentageError(std::chrono::duration<double> const* results, size_t size,
                                         std::chrono::duration<double> median) noexcept {
    std::vector<double> absolutePercentageErrors;
    for (size_t i = 0; i < size; ++i) {
        auto const& r = results[i];
        auto percent = (r - median) / r;
        if (percent < 0) {
            percent = -percent;
        }
        absolutePercentageErrors.push_back(percent);
    }
    std::sort(absolutePercentageErrors.begin(), absolutePercentageErrors.end());
    return calcMedian(absolutePercentageErrors.data(), absolutePercentageErrors.size());
}

IterationLogic::IterationLogic(Config const& config, std::string name) noexcept
    : mConfig(config)
    , mName(std::move(name)) {
    printStabilityInformationOnce();

    mTargetRuntime = detail::clockResolution() * mConfig.clockResolutionMultiple();
    if (mTargetRuntime > mConfig.maxEpochTime()) {
        mTargetRuntime = mConfig.maxEpochTime();
    }

    mSecPerUnit = new std::chrono::duration<double>[mConfig.epochs()];
    mSecPerUnitIndex = 0;
    mNumIters = mConfig.warmup();

    // check environment variable NANOBENCH_ENDLESS
#    if defined(_MSC_VER)
#        pragma warning(push)
#        pragma warning(disable : 4996) // getenv': This function or variable may be unsafe.
#    endif
    auto endless = std::getenv("NANOBENCH_ENDLESS");
#    if defined(_MSC_VER)
#        pragma warning(pop)
#    endif

    if (nullptr != endless && endless == name) {
        std::cout << "NANOBENCH_ENDLESS set: running '" << name << "' endlessly" << std::endl;
        mNumIters = (std::numeric_limits<size_t>::max)();
    }

    mIsWarmup = mConfig.warmup() != 0;
    if (mNumIters == 0) {
        mNumIters = 1;
    }
} // namespace detail

IterationLogic::~IterationLogic() noexcept {
    delete[] mSecPerUnit;
}

size_t IterationLogic::numIters() const noexcept {
    return mNumIters;
}

void IterationLogic::add(std::chrono::nanoseconds elapsed) noexcept {
    // if we are within 2/3 of the target runtime, add it.
    auto doubleElapsed = std::chrono::duration_cast<std::chrono::duration<double>>(elapsed);
    if (!mIsWarmup && (mIsTargetElapsedReached || elapsed * 3 >= mTargetRuntime * 2)) {
        mIsTargetElapsedReached = true;
        mSecPerUnit[mSecPerUnitIndex] = doubleElapsed / (mConfig.batch() * static_cast<double>(mNumIters));
        ++mSecPerUnitIndex;
        if (mSecPerUnitIndex == mConfig.epochs()) {
            // we got all the results that we need! bail out
            mResult = showResult("");
            mNumIters = 0;
            return;
        }
    }

    // we calculate in double values, so we can't get an overflow.
    auto newIters = static_cast<double>(mNumIters);
    if (elapsed * 10 < mTargetRuntime) {
        // we are far below the target runtime. Multiply iterations by 10 (with overflow check)
        newIters *= 10.0;
    } else {
        // less than 10x below target runtime, try to calculate it precisely
        newIters *= std::chrono::duration_cast<std::chrono::duration<double>>(mTargetRuntime) / doubleElapsed;
    }

    // add 0-10% of noise
    newIters *= 1 + 0.1 * mRng.uniform01();
    if (newIters >= static_cast<double>((std::numeric_limits<size_t>::max)())) {
        // overflow
        mResult = showResult("iterations overflow. Maybe your code got optimized away?");
        mNumIters = 0;
        return;
    }
    mNumIters = static_cast<size_t>(newIters);
    if (mNumIters <= 0) {
        // underflow
        mNumIters = 1;
    }
    mIsWarmup = false;
}

Result const& IterationLogic::result() const {
    return mResult;
}

Result IterationLogic::showResult(std::string const& errorMessage) const {
    auto& os = std::cout;
    if (!errorMessage.empty()) {
        os << "|        - |                   - |                   - |       - | :boom: " << errorMessage << ' '
           << detail::fmt::MarkDownCode(mName) << std::endl;
        return Result();
    }

    Result r(mConfig.unit(), mSecPerUnit, mSecPerUnitIndex);

    detail::fmt::StreamStateRestorer restorer(os);
    auto h = calcTableSettingsHash(mConfig);
    if (h != singletonLastTableSettingsHash()) {
        singletonLastTableSettingsHash() = h;

        os << std::endl
           << "| relative |" << std::setw(20) << std::right << ("ns/" + mConfig.unit()) << " |" << std::setw(20) << std::right
           << (mConfig.unit() + "/s") << " |   MdAPE | " << mConfig.title() << std::endl
           << "|---------:|--------------------:|--------------------:|--------:|:----------------------------------------------"
           << std::endl;
    }

    // we want output that looks like this:
    // |  1208.4% |               14.15 |       70,649,422.38 |    0.3% | `std::vector<std::string> emplace + release`

    // 1st column: relative
    os << '|';
    if (mConfig.relative().median() <= std::chrono::duration<double>::zero()) {
        // relative not set or invalid, print blank column
        os << "          |";
    } else {
        os << detail::fmt::Number(8, 1, mConfig.relative().median() / r.median() * 100) << "% |";
    }

    // 2nd column: ns/unit
    os << detail::fmt::Number(20, 2, 1e9 * r.median().count()) << " |";

    // 3rd column: unit/s
    os << detail::fmt::Number(20, 2, 1 / r.median().count()) << " |";

    // 4th column: MdAPE
    os << detail::fmt::Number(7, 1, r.medianAbsolutePercentError() * 100) << "% |";

    // 5th column: possible symbols, possibly errormessage, benchmark name
    if (r.medianAbsolutePercentError() >= 0.05) {
        // >=5%
        os << " :wavy_dash:";
    }
    os << ' ' << detail::fmt::MarkDownCode(mName) << std::endl;

    return r;
}

// formatting utilities
namespace fmt {

// adds thousands separator to numbers
NumSep::NumSep(char sep)
    : mSep(sep) {}

char NumSep::do_thousands_sep() const {
    return mSep;
}

std::string NumSep::do_grouping() const {
    return "\003";
}

// RAII to save & restore a stream's state
StreamStateRestorer::StreamStateRestorer(std::ostream& s)
    : mStream(s)
    , mLocale(s.getloc())
    , mPrecision(s.precision())
    , mWidth(s.width())
    , mFill(s.fill())
    , mFmtFlags(s.flags()) {}

StreamStateRestorer::~StreamStateRestorer() {
    restore();
}

// sets back all stream info that we remembered at construction
void StreamStateRestorer::restore() {
    mStream.imbue(mLocale);
    mStream.precision(mPrecision);
    mStream.width(mWidth);
    mStream.fill(mFill);
    mStream.flags(mFmtFlags);
}

Number::Number(int width, int precision, double value)
    : mWidth(width)
    , mPrecision(precision)
    , mValue(value) {}

std::ostream& Number::write(std::ostream& os) const {
    StreamStateRestorer restorer(os);
    os.imbue(std::locale(os.getloc(), new NumSep(',')));
    os << std::setw(mWidth) << std::setprecision(mPrecision) << std::fixed << mValue;
    return os;
}

std::ostream& operator<<(std::ostream& os, Number const& n) {
    return n.write(os);
}

// Formats any text as markdown code, escaping backticks.
MarkDownCode::MarkDownCode(std::string const& what) {
    mWhat.reserve(what.size() + 2);
    mWhat.push_back('`');
    for (char c : what) {
        mWhat.push_back(c);
        if ('`' == c) {
            mWhat.push_back('`');
        }
    }
    mWhat.push_back('`');
}

std::ostream& MarkDownCode::write(std::ostream& os) const {
    return os << mWhat;
}

std::ostream& operator<<(std::ostream& os, MarkDownCode const& mdCode) {
    return mdCode.write(os);
}

} // namespace fmt
} // namespace detail

// Result returned after a benchmark has finished. Can be used as a baseline for relative().
Result::Result(std::string u, std::chrono::duration<double> const* secPerUnitData, size_t size) noexcept
    : mUnit(std::move(u)) {

    std::vector<std::chrono::duration<double>> secPerUnit(secPerUnitData, secPerUnitData + size);
    std::sort(secPerUnit.begin(), secPerUnit.end());
    mMinimum = secPerUnit.front();
    mMaximum = secPerUnit.back();
    mMedian = detail::calcMedian(secPerUnit.data(), secPerUnit.size());
    mMedianAbsolutePercentError = detail::calcMedianAbsolutePercentageError(secPerUnit.data(), secPerUnit.size(), mMedian);
}

Result::Result() noexcept = default;

std::string const& Result::unit() const noexcept {
    return mUnit;
}

std::chrono::duration<double> Result::median() const noexcept {
    return mMedian;
}

double Result::medianAbsolutePercentError() const noexcept {
    return mMedianAbsolutePercentError;
}

std::chrono::duration<double> Result::minimum() const noexcept {
    return mMinimum;
}

std::chrono::duration<double> Result::maximum() const noexcept {
    return mMaximum;
}

// Configuration of a microbenchmark.
Config::Config() = default;
Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;
Config::Config(Config const&) = default;
Config& Config::operator=(Config const&) = default;
Config::~Config() noexcept = default;

double Config::batch() const noexcept {
    return mBatch;
}

// Set a baseline to compare it to. 100% it is exactly as fast as the baseline, >100% means it is faster than the baseline, <100%
// means it is slower than the baseline.
Config& Config::relative(Result const& baseline) noexcept {
    mRelative = baseline;
    return *this;
}
Result const& Config::relative() const noexcept {
    return mRelative;
}

// Operation unit. Defaults to "op", could be e.g. "byte" for string processing.
// Use singular (byte, not bytes).
Config& Config::unit(std::string u) {
    mUnit = std::move(u);
    return *this;
}
std::string const& Config::unit() const noexcept {
    return mUnit;
}

Config& Config::title(std::string benchmarkTitle) {
    mBenchmarkTitle = std::move(benchmarkTitle);
    return *this;
}
std::string const& Config::title() const noexcept {
    return mBenchmarkTitle;
}

// Number of epochs to evaluate. The reported result will be the median of evaluation of each epoch.
Config& Config::epochs(size_t numEpochs) noexcept {
    mNumEpochs = numEpochs;
    return *this;
}
size_t Config::epochs() const noexcept {
    return mNumEpochs;
}

// Desired evaluation time is a multiple of clock resolution. Default is to be 1000 times above this measurement precision.
Config& Config::clockResolutionMultiple(size_t multiple) noexcept {
    mClockResolutionMultiple = multiple;
    return *this;
}
size_t Config::clockResolutionMultiple() const noexcept {
    return mClockResolutionMultiple;
}

// Sets the maximum time each epoch should take. Default is 100ms.
Config& Config::maxEpochTime(std::chrono::nanoseconds t) noexcept {
    mMaxEpochTime = t;
    return *this;
}
std::chrono::nanoseconds Config::maxEpochTime() const noexcept {
    return mMaxEpochTime;
}

Config& Config::warmup(size_t numWarmupIters) noexcept {
    mWarmup = numWarmupIters;
    return *this;
}
size_t Config::warmup() const noexcept {
    return mWarmup;
}

Rng::Rng()
    : Rng(UINT64_C(0xd3b45fd780a1b6a3)) {}

Rng::Rng(uint64_t seed) noexcept
    : mA(seed)
    , mB(seed)
    , mC(seed)
    , mCounter(1) {
    for (size_t i = 0; i < 12; ++i) {
        operator()();
    }
}

Rng Rng::copy() const noexcept {
    Rng r;
    r.mA = mA;
    r.mB = mB;
    r.mC = mC;
    r.mCounter = mCounter;
    return r;
}

void Rng::assign(Rng const& other) noexcept {
    mA = other.mA;
    mB = other.mB;
    mC = other.mC;
    mCounter = other.mCounter;
}

} // namespace nanobench
} // namespace ankerl

#endif // ANKERL_NANOBENCH_IMPLEMENT
#endif // ANKERL_NANOBENCH_H_INCLUDED
