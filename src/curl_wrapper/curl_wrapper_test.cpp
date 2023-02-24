#include "curl_wrapper.hpp"
#include <catch2/catch.hpp>

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
    std::string url = wrapper.GenerateURL("^GSPC", sys_days{December / 01 / 2017}, sys_days{December / 31 / 2017}, "1d");
    wrapper.DownloadCSV(url);
    std::cout << wrapper.response << std::endl;
}
