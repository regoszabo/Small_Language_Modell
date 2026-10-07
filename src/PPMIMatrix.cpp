#include "PPMIMatrix.hpp"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdexcept>

PPMITargetNode::PPMITargetNode(std::string w, double v) : word(w), ppmiValue(v) {}

PPMITargetTable::PPMITargetTable(int s) : size(s) {
    buckets.resize(size);
}

void PPMITargetTable::add(const std::string& w, double val) {
    int idx = customHash(w) % size;
    buckets[idx].push_back(PPMITargetNode(w, val));
}

PPMIRow::PPMIRow() : targets(nullptr), occupied(false) {}

PPMIRow::~PPMIRow() {
    if (targets) delete targets;
}

PPMIMatrix::PPMIMatrix(const CoOccurrenceMatrix& cooc, int size) : tableSize(size) {
    table.resize(tableSize);
    buildFromCoOccurrence(cooc);
}

int PPMIMatrix::findEmptySlot(const std::string& word) const {
    unsigned long hash = customHash(word);
    int idx = hash % tableSize;
    while (table[idx].occupied) {
        idx = (idx + 1) % tableSize;
    }
    return idx;
}

int PPMIMatrix::findRowIndex(const std::string& word) const {
    unsigned long hash = customHash(word);
    int idx = hash % tableSize;
    int startIdx = idx;
    while (table[idx].occupied) {
        if (table[idx].word == word) return idx;
        idx = (idx + 1) % tableSize;
        if (idx == startIdx) break;
    }
    return -1;
}

void PPMIMatrix::buildFromCoOccurrence(const CoOccurrenceMatrix& cooc) {
    table.clear();
    table.resize(tableSize);

    long long N = cooc.getN();
    int coocSize = cooc.getSize();

    for (int i = 0; i < coocSize; ++i) {
        const ContextRow& cRow = cooc.getRow(i);
        if (!cRow.occupied) continue;

        int ppmIdx = findEmptySlot(cRow.word);
        table[ppmIdx].occupied = true;
        table[ppmIdx].word = cRow.word;
        table[ppmIdx].targets = new PPMITargetTable();

        double f_w = static_cast<double>(cRow.globalCount);
        TargetTable* tTable = cRow.targets;

        for (const auto& bucket : tTable->buckets) {
            for (const auto& node : bucket) {
                double f_wc = static_cast<double>(node.count);
                double f_c = static_cast<double>(cooc.getGlobalCount(node.word));

                if (f_c > 0 && f_w > 0) {
                    double numerator = f_wc * static_cast<double>(N);
                    double denominator = f_w * f_c;
                    double ratio = numerator / denominator;

                    double pmi = 0.0;
                    if (ratio > 0) pmi = std::log(ratio) / std::log(2.0);

                    if (pmi > 0) {
                        table[ppmIdx].targets->add(node.word, pmi);
                    }
                }
            }
        }
    }
}

std::string PPMIMatrix::predictDeterministic(const std::string& contextWord) const {
    int idx = findRowIndex(contextWord);
    if (idx == -1) return "";

    const PPMITargetTable* targets = table[idx].targets;
    std::string bestWord = "";
    double maxVal = -1.0;

    for (const auto& bucket : targets->buckets) {
        for (const auto& node : bucket) {
            if (node.ppmiValue > maxVal) {
                maxVal = node.ppmiValue;
                bestWord = node.word;
            }
        }
    }
    return bestWord;
}

std::string PPMIMatrix::predictStochastic(const std::string& contextWord, int K) const {
    int idx = findRowIndex(contextWord);
    if (idx == -1) return "";

    const PPMITargetTable* targets = table[idx].targets;
    struct Candidate { std::string w; double v; };
    std::vector<Candidate> candidates;

    for (const auto& bucket : targets->buckets) {
        for (const auto& node : bucket) {
            candidates.push_back({node.word, node.ppmiValue});
        }
    }

    if (candidates.empty()) return "";

    int n = static_cast<int>(candidates.size());
    int limit = (n < K) ? n : K;

    for (int i = 0; i < limit; ++i) {
        int maxIdx = i;
        for (int j = i + 1; j < n; ++j) {
            if (candidates[j].v > candidates[maxIdx].v) {
                maxIdx = j;
            }
        }
        if (maxIdx != i) {
            Candidate temp = candidates[i];
            candidates[i] = candidates[maxIdx];
            candidates[maxIdx] = temp;
        }
    }

    int rnd = std::rand() % limit;
    return candidates[rnd].w;
}

void PPMIMatrix::generateSequence(const CoOccurrenceMatrix& /*cooc*/, const std::string& startWord, int length, bool deterministic) const {
    std::string currentContext = startWord;
    std::cout << "Generalas eredmenye: \"" << currentContext << " ";

    for (int i = 0; i < length; ++i) {
        std::string nextWord;
        if (deterministic) {
            nextWord = predictDeterministic(currentContext);
        } else {
            nextWord = predictStochastic(currentContext, 3);
        }

        if (nextWord.empty()) {
            std::cout << "[VEGE]";
            break;
        }

        std::cout << nextWord << " ";
        currentContext = nextWord;
    }
    std::cout << "\"" << std::endl;
}