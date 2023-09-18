#include "thread_pool.hpp"
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <future>
#include <queue>
#include <chrono>
#include <thread>

void EmptyTask()
{
}

void Checksum(const std::uint32_t num, std::atomic_uint64_t* checksum)
{
    *checksum += num;
}

TEST_CASE("Construction and deconstruction", "[thread_pool]")
{
    BENCHMARK("Construction and destruction")
    {
        ThreadPool thread_pool{4};
    };
}

TEST_CASE("N tasks", "[thread_pool]")
{
    ThreadPool thread_pool{4};
    std::queue<std::future<void>> results;

    BENCHMARK("1000 tasks")
    {
        for (int n = 0; n < 1000; ++n)
        {
            results.emplace(thread_pool.AddTask(EmptyTask));
        }
        while (results.size())
        {
            results.front().get();
            results.pop();
        }
    };
}

TEST_CASE("N tasks checksum", "[thread_pool]")
{
    ThreadPool thread_pool{4};
    std::queue<std::future<void>> results;
    std::atomic_uint64_t checksum{0};
    std::uint64_t localChecksum{0};

    for (std::uint32_t n = 0; n < 1000; ++n)
    {
        results.emplace(thread_pool.AddTask(Checksum, n, &checksum));
        localChecksum += n;
    }
    while (results.size())
    {
        results.front().get();
        results.pop();
    }

    CHECK(checksum == localChecksum);
}