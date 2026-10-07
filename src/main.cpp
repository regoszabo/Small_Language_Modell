#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include "CoOccurrenceMatrix.hpp"
#include "PPMIMatrix.hpp"

using namespace std;

string normalizeWord(const string& input) {
    string result = "";
    for (char c : input) {
        if (isalnum(static_cast<unsigned char>(c)) || c == '\'') {
            result += tolower(static_cast<unsigned char>(c));
        }
    }
    return result;
}

void processFile(const string& filename, CoOccurrenceMatrix& matrix) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "HIBA: Nem sikerult megnyitni a fajlt: " << filename << endl;
        return;
    }
    cout << "Fajl feldolgozasa: " << filename << " ... ";
    string prevWord = "";
    int wordCount = 0;
    char c;
    string currentBuffer = "";
    while (file.get(c)) {
        if (isalnum(static_cast<unsigned char>(c)) || c == '\'') {
            currentBuffer += tolower(static_cast<unsigned char>(c));
        } else {
            if (!currentBuffer.empty()) {
                matrix.addOccurrence(prevWord, currentBuffer);
                prevWord = currentBuffer;
                wordCount++;
                currentBuffer = "";
            }
        }
    }
    if (!currentBuffer.empty()) {
        matrix.addOccurrence(prevWord, currentBuffer);
        wordCount++;
    }
    cout << "SIKERES. (Tokenek szama: " << wordCount << ")" << endl;
    file.close();
}

void runTest(const PPMIMatrix& ppmi, const CoOccurrenceMatrix& cooc, string startWord, int length, bool deterministic) {
    string normStart = normalizeWord(startWord);
    cout << "\n--------------------------------------------------" << endl;
    cout << "TESZT FUTTATASA" << endl;
    cout << "Kezdo szo: '" << startWord << "' (Normalizalva: '" << normStart << "')" << endl;
    cout << "Mod: " << (deterministic ? "DETERMINISZTIKUS" : "SZTOCHASZTIKUS") << endl;
    cout << "--------------------------------------------------" << endl;
    if (normStart.empty()) return;
    ppmi.generateSequence(cooc, normStart, length, deterministic);
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    vector<string> files = {
            "Agatha_Christie_The_Big_Four.txt",
            "Agatha_Christie_The_Plymouth_Express_Affair.txt",
            "Shakespeare_Hamlet.txt",
            "Shakespeare_Othello.txt"
    };

    const int MATRIX_SIZE = 100003;
    CoOccurrenceMatrix coocMatrix(MATRIX_SIZE);

    try {
        for (const auto& file : files) {
            processFile(file, coocMatrix);
        }

        cout << "\nOsszes szo (N): " << coocMatrix.getN() << endl;

        cout << "\n[PPMI Epites...]" << endl;
        PPMIMatrix ppmiMatrix(coocMatrix, MATRIX_SIZE);
        cout << "PPMI Matrix elkeszult." << endl;

        cout << "\n=== TESZTEK FUTTATASA ===" << endl;
        runTest(ppmiMatrix, coocMatrix, "murder", 8, true);
        runTest(ppmiMatrix, coocMatrix, "the", 10, false);
        runTest(ppmiMatrix, coocMatrix, "king", 6, true);
        runTest(ppmiMatrix, coocMatrix, "love", 8, false);

    } catch (const runtime_error& e) {
        cerr << "\nFATALIS HIBA: " << e.what() << endl;
        return 1;
    }

    return 0;
}