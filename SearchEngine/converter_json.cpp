#include "converter_json.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <filesystem>

namespace fs = std::filesystem;

bool ConverterJSON::fileExists(const std::string& path) {
    std::ifstream f(path.c_str());
    return f.good();
}

void ConverterJSON::loadConfig() {
    std::ifstream file("config.json");
    if (!file.is_open()) {
        throw std::runtime_error("config file is missing");
    }

    json j;
    try {
        file >> j;
    }
    catch (const json::parse_error& e) {
        throw std::runtime_error("config.json parse error: " + std::string(e.what()));
    }

    if (j.empty()) {
        throw std::runtime_error("config file is empty");
    }

    if (!j.contains("config")) {
        throw std::runtime_error("config section missing in config.json");
    }

    auto& configSection = j["config"];

    config.name = configSection.value("name", "SearchEngine");
    config.version = configSection.value("version", "0.1");
    config.max_responses = configSection.value("max_responses", 5);

    if (config.version != "0.1") {
        std::cerr << "Warning: config.json has incorrect file version. Expected 0.1, got "
            << config.version << std::endl;
    }

    if (!j.contains("files")) {
        throw std::runtime_error("files section missing in config.json");
    }

    for (const auto& file : j["files"]) {
        std::string path = file.get<std::string>();
        config.files.push_back(path);
    }
}

Config ConverterJSON::getConfig() {
    loadConfig();
    return config;
}

std::vector<std::string> ConverterJSON::getTextDocuments() {
    std::vector<std::string> documents;

    for (const auto& path : getConfig().files) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Warning: Cannot open file " << path << std::endl;
            documents.push_back("");
            continue;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        documents.push_back(buffer.str());
    }

    return documents;
}

int ConverterJSON::getResponsesLimit() {
    return getConfig().max_responses;
}

std::vector<std::string> ConverterJSON::getRequests() {
    std::vector<std::string> requests;
    std::ifstream file("requests.json");

    if (!file.is_open()) {
        throw std::runtime_error("requests file is missing");
    }

    json j;
    try {
        file >> j;
    }
    catch (const json::parse_error& e) {
        throw std::runtime_error("requests.json parse error: " + std::string(e.what()));
    }

    if (!j.contains("requests")) {
        throw std::runtime_error("requests section missing in requests.json");
    }

    for (const auto& req : j["requests"]) {
        requests.push_back(req.get<std::string>());
    }

    return requests;
}

std::string ConverterJSON::intToRequestId(int num) {
    std::ostringstream oss;
    oss << "request" << std::setw(3) << std::setfill('0') << num;
    return oss.str();
}

void ConverterJSON::putAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers) {
    json j;
    j["answers"] = json::object();

    for (size_t i = 0; i < answers.size(); ++i) {
        std::string requestId = intToRequestId(static_cast<int>(i + 1));

        if (answers[i].empty()) {
            j["answers"][requestId] = { {"result", "false"} };
        }
        else {
            json relevance = json::array();
            for (const auto& answer : answers[i]) {
                relevance.push_back({ {"docid", answer.first}, {"rank", answer.second} });
            }
            j["answers"][requestId] = { {"result", "true"}, {"relevance", relevance} };
        }
    }

    std::ofstream file("answers.json");
    if (!file.is_open()) {
        
        if (!fs::exists(fs::current_path())) {
            throw std::runtime_error("Current directory does not exist");
        }

        try {
            auto space = fs::space(fs::current_path());
            if (space.available < 1024) {
                throw std::runtime_error("Not enough disk space to create answers.json");
            }
        }
        catch (...) {
           
        }

        throw std::runtime_error("Cannot create or open answers.json file. "
            "Check permissions, disk space, or if the file is not read-only.");
    }

    file << j.dump(4);

    if (!file.good()) {
        file.close();
        try {
            fs::remove("answers.json");
        }
        catch (...) {
           
        }
        throw std::runtime_error("Error writing to answers.json. Disk may be full or file system is read-only.");
    }

    file.close();

    std::ifstream checkFile("answers.json");
    if (!checkFile.is_open()) {
        throw std::runtime_error("answers.json was created but cannot be opened for reading. "
            "Check file permissions.");
    }
    checkFile.close();
}