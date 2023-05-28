#include "spot_data.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CSV parsing", "[spot_data]")
{
    auto csv = std::string("Date,Open,High,Low,Close,Adj "
                    "Close,Volume\n2022-12-30,3829.060059,3839.850098,3800.340088,3839.500000,3839.500000,2979870000");

    SpotData spot_data(csv);

    REQUIRE(spot_data.spots[sys_days{December / 30 / 2022}].high == 3839.850098);
    REQUIRE(spot_data.spots[sys_days{December / 30 / 2022}].open == 3829.060059);
    REQUIRE(spot_data.spots[sys_days{December / 30 / 2022}].low == 3800.340088);
    REQUIRE(spot_data.spots[sys_days{December / 30 / 2022}].close == 3839.500000);
}