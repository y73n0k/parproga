#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <unordered_map>
#include <cassert>

#include <omp.h>

using namespace std;


struct Symbol {
    char ch;
    int freq;
    string code;
};


bool comparator(const Symbol &a, const Symbol &b) {
    return a.freq > b.freq;
}


void shannon_fano_coding(vector<Symbol> &symbols, int start, int end) {
    if (start >= end)
        return;

    int total = 0;
    for (int i = start; i <= end; ++i)
        total += symbols[i].freq;

    int half = 0, split = start;
    for (int i = start; i <= end; ++i) {
        half += symbols[i].freq;
        if (half >= total / 2) {
            split = i;
            break;
        }
    }

    for (int i = start; i <= split; ++i)
        symbols[i].code += "0";
    for (int i = split + 1; i <= end; ++i)
        symbols[i].code += "1";

    #pragma omp task shared(symbols)
    shannon_fano_coding(symbols, start, split);

    #pragma omp task shared(symbols)
    shannon_fano_coding(symbols, split + 1, end);
}

unordered_map<char, string> build_encoding_map(const vector<Symbol> &symbols) {
    unordered_map<char, string> encodingMap;
    for (const auto &symbol : symbols) {
        encodingMap[symbol.ch] = symbol.code;
    }
    return encodingMap;
}

unordered_map<string, char> build_decoding_map(const vector<Symbol> &symbols) {
    unordered_map<string, char> decodingMap;
    for (const auto &symbol : symbols) {
        decodingMap[symbol.code] = symbol.ch;
    }
    return decodingMap;
}

string decode(const unordered_map<string, char> &decodingMap, const string &encodedStr) {
    string decodedStr = "";
    string currentCode = "";

    for (char bit : encodedStr) {
        currentCode += bit;
        if (decodingMap.find(currentCode) != decodingMap.end()) {
            decodedStr += decodingMap.at(currentCode);
            currentCode = "";
        }
    }

    return decodedStr;
}

string encode(const unordered_map<char, string> &encodingMap, const string &decodedStr) {
    string encodedStr = "";
    for (char ch : decodedStr) {
        encodedStr += encodingMap.at(ch);
    }
    return encodedStr;
}

vector<Symbol> get_symbols(string str) {
    unordered_map<char, int> frequency_map;

    #pragma omp parallel
    {
        unordered_map<char, int> local_frequency_map;

        #pragma omp for
        for (size_t i = 0; i < str.size(); ++i) {
            local_frequency_map[str[i]]++;
        }

        #pragma omp critical
        {
            for (const auto &entry : local_frequency_map) {
                frequency_map[entry.first] += entry.second;
            }
        }
    }

    vector<Symbol> symbols;
    for (const auto &entry : frequency_map) {
        symbols.push_back(Symbol{entry.first, entry.second, ""});
    }

    return symbols;
}


int main(int argc, char *argv[]) {
    const unsigned int threads = strtoul(argv[1], NULL, 10);
    const unsigned int N = 20000000;

    omp_set_num_threads(threads);

    string test;
    test.resize(N);
    cin.read(&test[0], N);

    double time_start = omp_get_wtime();

    auto symbols = get_symbols(test);
    sort(symbols.begin(), symbols.end(), comparator);

    #pragma omp parallel
    {
        #pragma omp single 
        {
            shannon_fano_coding(symbols, 0, symbols.size() - 1);
        }
    }

    double time_end = omp_get_wtime();

    auto encoded = encode(build_encoding_map(symbols), test);
    cout << encoded.length() << endl;
    // assert(test == decode(build_decoding_map(symbols), encoded));

    // for (const auto &symbol : symbols) {
        // cout << symbol.ch << " " << symbol.freq << " " << symbol.code << endl;
    // }

    // cout << encoded.length() << endl;

    // printf("%g\n", time_end - time_start);
    return 0;
}
