#include "inverted_index.h"
#include <sstream>
#include <algorithm>
#include <cctype>

void InvertedIndex::addWord(const std::string& word, size_t doc_id) {
    std::lock_guard<std::mutex> lock(index_mutex);

    auto& entries = freq_dictionary[word];
    bool found = false;

    for (auto& entry : entries) {
        if (entry.doc_id == doc_id) {
            entry.count++;
            found = true;
            break;
        }
    }

    if (!found) {
        entries.push_back({ doc_id, 1 });
    }
}

void InvertedIndex::indexDocument(const std::string& doc, size_t doc_id) {
    std::string word;
    std::stringstream ss(doc);

    while (ss >> word) {
        std::transform(word.begin(), word.end(), word.begin(),
            [](unsigned char c) { return std::tolower(c); });
        addWord(word, doc_id);
    }
}

void InvertedIndex::updateDocumentBase(const std::vector<std::string>& input_docs) {
    docs = input_docs;
    freq_dictionary.clear();

    std::vector<std::thread> threads;

    for (size_t i = 0; i < docs.size(); ++i) {
        threads.emplace_back(&InvertedIndex::indexDocument, this, docs[i], i);
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

std::vector<Entry> InvertedIndex::getWordCount(const std::string& word) {
    std::string lowerWord = word;
    std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(),
        [](unsigned char c) { return std::tolower(c); });

    auto it = freq_dictionary.find(lowerWord);
    if (it != freq_dictionary.end()) {
        return it->second;
    }
    return {};
}