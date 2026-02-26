#include <gtest/gtest.h>
#include "inverted_index.h"
#include "search_server.h"

using namespace std;

void TestInvertedIndexFunctionality(
    const vector<string>& docs,
    const vector<string>& requests,
    const std::vector<vector<Entry>>& expected
) {
    std::vector<std::vector<Entry>> result;
    InvertedIndex idx;
    idx.updateDocumentBase(docs);

    for (auto& request : requests) {
        std::vector<Entry> word_count = idx.getWordCount(request);
        result.push_back(word_count);
    }

    ASSERT_EQ(result, expected);
}

TEST(TestCaseInvertedIndex, TestBasic) {
    const vector<string> docs = {
        "london is the capital of great britain",
        "big ben is the nickname for the Great bell of the striking clock"
    };
    const vector<string> requests = { "london", "the" };
    const vector<vector<Entry>> expected = {
        {{0, 1}},
        {{0, 1}, {1, 3}}
    };
    TestInvertedIndexFunctionality(docs, requests, expected);
}

TEST(TestCaseInvertedIndex, TestBasic2) {
    const vector<string> docs = {
        "milk milk milk milk water water water",
        "milk water water",
        "milk milk milk milk milk water water water water water",
        "americano cappuccino"
    };
    const vector<string> requests = { "milk", "water", "cappuccino" };
    const vector<vector<Entry>> expected = {
        {{0, 4}, {1, 1}, {2, 5}},
        {{0, 2}, {1, 2}, {2, 5}},
        {{3, 1}}
    };
    TestInvertedIndexFunctionality(docs, requests, expected);
}

TEST(TestCaseInvertedIndex, TestInvertedIndexMissingWord) {
    const vector<string> docs = {
        "a b c d e f g h i j k l",
        "statement"
    };
    const vector<string> requests = { "m", "statement" };
    const vector<vector<Entry>> expected = {
        {},
        {{1, 1}}
    };
    TestInvertedIndexFunctionality(docs, requests, expected);
}

TEST(TestCaseSearchServer, TestSimple) {
    const vector<string> docs = {
        "milk milk milk milk water water water",
        "milk water water",
        "milk milk milk milk milk water water water water water",
        "americano cappuccino"
    };
    const vector<string> request = { "milk water", "sugar" };
    const std::vector<vector<RelativeIndex>> expected = {
        {
            {2, 1},
            {0, 0.7f},
            {1, 0.3f}
        },
        {}
    };

    InvertedIndex idx;
    idx.updateDocumentBase(docs);
    SearchServer srv(idx);
    std::vector<vector<RelativeIndex>> result = srv.search(request);

    ASSERT_EQ(result.size(), expected.size());
    for (size_t i = 0; i < result.size(); ++i) {
        ASSERT_EQ(result[i].size(), expected[i].size());
        for (size_t j = 0; j < result[i].size(); ++j) {
            EXPECT_EQ(result[i][j].doc_id, expected[i][j].doc_id);
            EXPECT_NEAR(result[i][j].rank, expected[i][j].rank, 0.01);
        }
    }
}