#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include "inverted_index.h"

struct RelativeIndex {
    size_t doc_id;
    float rank;

    bool operator==(const RelativeIndex& other) const {
        return (doc_id == other.doc_id && std::abs(rank - other.rank) < 0.001f);
    }
};

class SearchServer {
public:
    SearchServer(InvertedIndex& idx) : _index(idx) {}

    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries_input);

private:
    InvertedIndex& _index;

    std::vector<std::string> tokenizeQuery(const std::string& query);
    std::map<size_t, float> calculateAbsoluteRelevance(const std::vector<std::string>& uniqueWords,
        const std::set<size_t>& candidateDocs);
    std::vector<RelativeIndex> calculateRelativeRelevance(const std::map<size_t, float>& absRelevance);
};