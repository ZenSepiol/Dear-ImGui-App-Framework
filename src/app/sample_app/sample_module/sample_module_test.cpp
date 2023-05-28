#include "sample_module.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Sample module test of truth", "[sample_module]")
{
    SECTION("The Truth")
    {
        REQUIRE(42 == 42);
    }
}

TEST_CASE("Sample module other test", "[sample_module]")
{
    auto my_sample = SampleModule(10);
    REQUIRE(my_sample.GetNumber() == 10);
}
