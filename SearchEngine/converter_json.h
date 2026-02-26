#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include "json.hpp"

using json = nlohmann::json;

struct Config {
    std::string name;
    std::string version;
    int max_responses;
    std::vector<std::string> files;
};

class ConverterJSON {
public:
    ConverterJSON() = default;

    std::vector<std::string> getTextDocuments();
    int getResponsesLimit();
    std::vector<std::string> getRequests();
    void putAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers);
    Config getConfig();

private:
    Config config;
    void loadConfig();
    bool fileExists(const std::string& path);
    std::string intToRequestId(int num);
};