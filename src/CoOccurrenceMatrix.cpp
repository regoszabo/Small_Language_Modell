#include "CoOccurrenceMatrix.hpp"
#include <stdexcept>

unsigned long customHash(const std::string& str) {
    unsigned long hash = 5381;
    for (char c : str) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

TargetNode::TargetNode(std::string w) : word(w), count(1) {}

TargetTable::TargetTable(int s) : size(s) {
    buckets.resize(size);
}

void TargetTable::add(const std::string& targetWord) {
    unsigned long idx = customHash(targetWord) % size;
    for (auto& node : buckets[idx]) {
        if (node.word == targetWord) {
            node.count++;
            return;
        }
    }
    buckets[idx].push_back(TargetNode(targetWord));
}

ContextRow::ContextRow() : globalCount(0), targets(nullptr), occupied(false) {}

ContextRow::~ContextRow() {
    if (targets) delete targets;
}

CoOccurrenceMatrix::CoOccurrenceMatrix(int size) : tableSize(size), totalWordCount(0) {
    table.resize(tableSize);
}

void CoOccurrenceMatrix::addOccurrence(const std::string& prevWord, const std::string& currentWord) {
    int currIdx = findOrCreateIndex(currentWord);
    table[currIdx].globalCount++;
    totalWordCount++;

    if (!prevWord.empty()) {
        int prevIdx = findOrCreateIndex(prevWord);
        table[prevIdx].targets->add(currentWord);
    }
}

int CoOccurrenceMatrix::findOrCreateIndex(const std::string& word) {
    unsigned long hash = customHash(word);
    int idx = hash % tableSize;
    int startIdx = idx;

    while (table[idx].occupied) {
        if (table[idx].word == word) return idx;
        idx = (idx + 1) % tableSize;
        if (idx == startIdx) {
            throw std::runtime_error("CoOccurrence Hash Table Overflow: Table is full.");
        }
    }

    table[idx].occupied = true;
    table[idx].word = word;
    table[idx].globalCount = 0;
    table[idx].targets = new TargetTable();
    return idx;
}

int CoOccurrenceMatrix::getSize() const { return tableSize; }
const ContextRow& CoOccurrenceMatrix::getRow(int idx) const { return table[idx]; }
long long CoOccurrenceMatrix::getN() const { return totalWordCount; }

int CoOccurrenceMatrix::getGlobalCount(const std::string& word) const {
    unsigned long hash = customHash(word);
    int idx = hash % tableSize;
    int startIdx = idx;
    while (table[idx].occupied) {
        if (table[idx].word == word) return table[idx].globalCount;
        idx = (idx + 1) % tableSize;
        if (idx == startIdx) break;
    }
    return 0;
}