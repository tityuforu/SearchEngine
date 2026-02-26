#include "search_server.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <set>
#include <cmath> 

std::vector<std::string> SearchServer::tokenizeQuery(const std::string& query) {
    std::vector<std::string> words;
    std::string word;
    std::stringstream ss(query);

    while (ss >> word) {
        std::transform(word.begin(), word.end(), word.begin(),
            [](unsigned char c) { return std::tolower(c); });
        words.push_back(word);
    }

    return words;
}

std::map<size_t, float> SearchServer::calculateAbsoluteRelevance(
    const std::vector<std::string>& uniqueWords,
    const std::set<size_t>& candidateDocs) {

    std::map<size_t, float> absRelevance;

    for (size_t doc_id : candidateDocs) {
        float relevance = 0;
        for (const auto& word : uniqueWords) {
            auto entries = _index.getWordCount(word);
            for (const auto& entry : entries) {
                if (entry.doc_id == doc_id) {
                    relevance += entry.count;
                    break;
                }
            }
        }
        absRelevance[doc_id] = relevance;
    }

    return absRelevance;
}

std::vector<RelativeIndex> SearchServer::calculateRelativeRelevance(
    const std::map<size_t, float>& absRelevance) {

    if (absRelevance.empty()) {
        return {};
    }

    float maxRelevance = 0;
    for (const auto& [doc_id, relevance] : absRelevance) {
        if (relevance > maxRelevance) {
            maxRelevance = relevance;
        }
    }

    std::vector<RelativeIndex> result;
    for (const auto& [doc_id, relevance] : absRelevance) {
        result.push_back({ doc_id, relevance / maxRelevance });
    }

    std::sort(result.begin(), result.end(),
        [](const RelativeIndex& a, const RelativeIndex& b) {
            return a.rank > b.rank;
        });

    return result;
}

std::vector<std::vector<RelativeIndex>> SearchServer::search(
    const std::vector<std::string>& queries_input) {

    std::vector<std::vector<RelativeIndex>> results;

    for (const auto& query : queries_input) {
        auto words = tokenizeQuery(query);

        std::set<std::string> uniqueWordsSet(words.begin(), words.end());
        std::vector<std::string> uniqueWords(uniqueWordsSet.begin(), uniqueWordsSet.end());

        if (uniqueWords.empty()) {
            results.push_back({});
            continue;
        }

        std::sort(uniqueWords.begin(), uniqueWords.end(),
            [this](const std::string& a, const std::string& b) {
                return _index.getWordCount(a).size() < _index.getWordCount(b).size();
            });

        std::set<size_t> candidateDocs;
        bool first = true;

        for (const auto& word : uniqueWords) {
            auto entries = _index.getWordCount(word);
            std::set<size_t> docsForWord;
            for (const auto& entry : entries) {
                docsForWord.insert(entry.doc_id);
            }

            if (first) {
                candidateDocs = docsForWord;
                first = false;
            }
            else {
                std::set<size_t> intersection;
                std::set_intersection(
                    candidateDocs.begin(), candidateDocs.end(),
                    docsForWord.begin(), docsForWord.end(),
                    std::inserter(intersection, intersection.begin())
                );
                candidateDocs = intersection;
            }

            if (candidateDocs.empty()) break;
        }

        if (candidateDocs.empty()) {
            results.push_back({});
            continue;
        }

        auto absRelevance = calculateAbsoluteRelevance(uniqueWords, candidateDocs);
        auto relRelevance = calculateRelativeRelevance(absRelevance);

        results.push_back(relRelevance);
    }

    return results;
}