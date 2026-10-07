#ifndef COOCCURRENCE_MATRIX_HPP
#define COOCCURRENCE_MATRIX_HPP

#include <string>
#include <vector>
#include <list>

unsigned long customHash(const std::string& str);

struct TargetNode {
    std::string word;
    int count;
    TargetNode(std::string w);
};

struct TargetTable {
    std::vector<std::list<TargetNode>> buckets;
    int size;

    TargetTable(int s = 128);
    void add(const std::string& targetWord);
};

struct ContextRow {
    std::string word;
    int globalCount;
    TargetTable* targets;
    bool occupied;

    ContextRow();
    ~ContextRow();
};

class CoOccurrenceMatrix {
private:
    std::vector<ContextRow> table;
    int tableSize;
    long long totalWordCount;

    int findOrCreateIndex(const std::string& word);

public:
    CoOccurrenceMatrix(int size = 100003);

    void addOccurrence(const std::string& prevWord, const std::string& currentWord);

    int getSize() const;
    const ContextRow& getRow(int idx) const;
    long long getN() const;
    int getGlobalCount(const std::string& word) const;
};

#endif