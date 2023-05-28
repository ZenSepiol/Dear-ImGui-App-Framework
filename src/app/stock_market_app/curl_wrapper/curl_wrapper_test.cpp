#include "curl_wrapper.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace std::chrono;

TEST_CASE("URL generation", "[curl_wrapper]")
{
    CurlWrapper wrapper;
    std::string url =
        wrapper.GenerateURL("^GSPC", sys_days{December / 01 / 2017}, sys_days{December / 31 / 2017}, "1d");

    REQUIRE(url == std::string("https://query1.finance.yahoo.com/v7/finance/download/"
                               "^GSPC?period1=1512086400&period2=1514678400&interval=1d&events=history"));
}

TEST_CASE("Test Curl Wrapper", "[curl_wrapper]")
{
    CurlWrapper wrapper;
    std::string url =
        wrapper.GenerateURL("^GSPC", sys_days{December / 30 / 2022}, sys_days{December / 31 / 2022}, "1d");
    wrapper.DownloadCSV(url);

    REQUIRE(
        wrapper.GetResponse() ==
        std::string("Date,Open,High,Low,Close,Adj "
                    "Close,Volume\n2022-12-30,3829.060059,3839.850098,3800.340088,3839.500000,3839.500000,2979870000"));
}
