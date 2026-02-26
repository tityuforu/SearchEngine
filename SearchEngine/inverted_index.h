#pragma once

#include <string>
#include <vector>
#include <map>
#include <thread>
#include <mutex>

struct Entry {
    size_t doc_id;
    size_t count;

    bool operator==(const Entry& other) const {
        return (doc_id == other.doc_id && count == other.count);
    }
};

class InvertedIndex {
public:
    InvertedIndex() = default;

    void updateDocumentBase(const std::vector<std::string>& input_docs);
    std::vector<Entry> getWordCount(const std::string& word);
    const std::vector<std::string>& getDocs() const { return docs; }

private:
    std::vector<std::string> docs;
    std::map<std::string, std::vector<Entry>> freq_dictionary;
    std::mutex index_mutex;

    void indexDocument(const std::string& doc, size_t doc_id);
    void addWord(const std::string& word, size_t doc_id);
};