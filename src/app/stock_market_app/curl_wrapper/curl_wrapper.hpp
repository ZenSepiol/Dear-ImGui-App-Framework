#include <chrono>
#include <curl/curl.h>
#include <iostream>

using namespace std::chrono;

size_t curl_handler(char* content, size_t size, size_t nmemb, void* userdata)
{
    static_cast<std::string*>(userdata)->append(content, size * nmemb);
    return size * nmemb;
}

class CurlWrapper
{
  public:
    CurlWrapper()
    {
        curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curl_handler);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &this->response);
    }

    ~CurlWrapper()
    {
        curl_easy_cleanup(curl);
    }

    std::string GenerateURL(const std::string& symbol, const system_clock::time_point& date1,
                            const system_clock::time_point& date2, const std::string& interval)
    {
        auto date_1 = std::to_string(duration_cast<seconds>(date1.time_since_epoch()).count());
        auto date_2 = std::to_string(duration_cast<seconds>(date2.time_since_epoch()).count());

        std::string url = "https://query1.finance.yahoo.com/v7/finance/download/" + symbol + "?period1=" + date_1 +
                          "&period2=" + date_2 + "&interval=" + interval + "&events=history";

        return url;
    }

    CURLcode DownloadCSV(const std::string& url)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        CURLcode res = curl_easy_perform(curl);

        return res;
    }

    std::string GetResponse()
    {
        return response;
    }

  private:
    CURL* curl;
    std::string response;
};
