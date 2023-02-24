#include <chrono>
#include <iostream>
#include <vector>

class SpotData
{
    class TradingValues
    {
        double open;
        double close;
        double high;
        double low;
    };

  public:
    SpotData(const std::string& csv_data)
    {
        parseCSV();
    }

  private:
    void parseCSV(const std::string& csv_data)
    {
        std::istringstream csvStream(csv_data);
        std::string line;

        // Remove the header line
        std::getline(csvStream, line);

        while (std::getline(csvStream, line))
        {
            // Split line delimited by comma separator into a vector
            std::vector<std::string> spotVector;
            std::stringstream iss(line);
            std::string lineItem;
            while (std::getline(iss, lineItem, ','))
            {
                spotVector.push_back(lineItem);
            }

            if (spotVector[0] != "null" && spotVector[1] != "null")
            {
                timepoint = spotVector[0]

                TradingValues values;
                values.open = std::atof(spotVector[1].c_str());
                values.high = std::atof(spotVector[2].c_str());
                values.low = std::atof(spotVector[3].c_str());
                values.close = std::atof(spotVector[4].c_str());

                spots.emplace_back(timepoint, values);
            }
        }
    }

    std::map<time_point, TradingValues> spots;
};
