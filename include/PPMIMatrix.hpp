#ifndef PPMI_MATRIX_HPP
#define PPMI_MATRIX_HPP

#include "CoOccurrenceMatrix.hpp"
#include <string>
#include <vector>
#include <list>

struct PPMITargetNode {
    std::string word;
    double ppmiValue;
    PPMITargetNode(std::string w, double v);
};

struct PPMITargetTable {
    std::vector<std::list<PPMITargetNode>> buckets;
    int size;

    PPMITargetTable(int s = 128);
    void add(const std::string& w, double val);
};

struct PPMIRow {
    std::string word;
    PPMITargetTable* targets;
    bool occupied;

    PPMIRow();
    ~PPMIRow();
};

class PPMIMatrix {
private:
    std::vector<PPMIRow> table;
    int tableSize;

    int findEmptySlot(const std::string& word) const;
    int findRowIndex(const std::string& word) const;
    void buildFromCoOccurrence(const CoOccurrenceMatrix& cooc);

public:
    PPMIMatrix(const CoOccurrenceMatrix& cooc, int size = 100003);

    std::string predictDeterministic(const std::string& contextWord) const;
    std::string predictStochastic(const std::string& contextWord, int K = 3) const;

    void generateSequence(const CoOccurrenceMatrix& /*cooc*/, const std::string& startWord, int length, bool deterministic) const;
};

#endif