#include "date.h"
#include <chrono>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using namespace std::chrono;

class SpotData
{
    struct TradingValues
    {
        double open;
        double close;
        double high;
        double low;
    };

  public:
    SpotData(){};
    SpotData(const std::string& csv_data)
    {
        ParseCSV(csv_data);
    }

  private:
    void ParseCSV(const std::string& csv_data)
    {
        std::istringstream csv_stream(csv_data);
        std::string line;

        // Remove the header line
        std::getline(csv_stream, line);

        while (std::getline(csv_stream, line))
        {
            // Split line delimited by comma separator into a vector
            const std::regex regex{"((?:[^\\\\,]|\\\\.)*?)(?:,|$)"};
            std::vector<std::string> vector{std::sregex_token_iterator(line.begin(), line.end(), regex, 1),
                                            std::sregex_token_iterator()};

            if (vector[0] != "null" && vector[1] != "null")
            {
                // Will parse the string of the time as timepoint (e.g. "%Y-%m-%d")
                std::istringstream in{vector[0]};
                system_clock::time_point timepoint;
                in >> date::parse("%F", timepoint);

                TradingValues values;
                values.open  = std::stod(vector[1]);
                values.high  = std::stod(vector[2]);
                values.low   = std::stod(vector[3]);
                values.close = std::stod(vector[4]);

                spots.emplace(timepoint, values);
            }
        }
    }

  public:
    std::map<system_clock::time_point, TradingValues> spots;
};
