#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <string>
#include <math.h>
#include <omp.h>

using namespace std;

struct Symbol {
    char character;
    double probability;
    double b;
    std::string code;
    std::string res_code;
    double log;

    bool operator<(const Symbol& other) const {
        return probability > other.probability;
    }
};

std::string decimalToBinary(double num, int precision = 10) {
    if (num == 0) {
        return "0.000000000";
    }

    std::string binary = "0.";
    while (precision-- > 0) {
        num *= 2;
        int digit = static_cast<int>(num);  
        if (digit == 1) {
            binary += "1";
        } else {
            binary += "0";
        }
        num -= digit; 
        if (num == 0) break;
    }
    return binary;
}

void printSymbols(const std::vector<Symbol>& symbols) {
    std::cout << "Character | Probability | Binary Probability | Code\n";
    std::cout << "------------------------------------------------------\n";
    for (const auto& symbol : symbols) {
        std::cout << symbol.character << "         | "
                  << symbol.probability << "       | "
                  << symbol.b << "         | "
                  << symbol.code << "   | "
                  << symbol.res_code << "    | "
                  << symbol.log << "\n";
    }
}

unordered_map<char, string> encodeText(const std::string& inputText) {
    // 1. Подсчет частот символов с использованием OpenMP.
    std::unordered_map<char, double> frequency;
    unordered_map<char, string> result;

    #pragma omp parallel
    {
        std::unordered_map<char, double> local_frequency;

        #pragma omp for nowait
        for (size_t i = 0; i < inputText.size(); ++i) {
            local_frequency[inputText[i]]++;
        }

        #pragma omp critical
        {
            for (const auto& entry : local_frequency) {
                frequency[entry.first] += entry.second;
            }
        }
    }

    // 2. Создание вектора символов.
    std::vector<Symbol> symbols;
    for (const auto& pair : frequency) {
        symbols.push_back({pair.first, pair.second / inputText.size(), 0, "", "", 0});
    }

    // 3. Сортировка символов по вероятности.
    std::sort(symbols.begin(), symbols.end());

    // 4. Вычисление b для каждого символа.
    double cnt = 0;
    for (size_t i = 1; i < symbols.size(); i++) {
        cnt += symbols[i - 1].probability;
        symbols[i].b = cnt;
    }

    // 5. Вычисление кода каждого символа.
    #pragma omp parallel for
    for (int i = 0; i < symbols.size(); i++) {
        symbols[i].code = decimalToBinary(symbols[i].b);
        symbols[i].log = std::ceil(-1 * std::log2(symbols[i].probability));
        symbols[i].res_code = symbols[i].code.substr(2, symbols[i].log);
        result[symbols[i].character] = symbols[i].res_code;
    }

    // Печать символов.
    // printSymbols(symbols);
    return result;
}

int main(int argc, char *argv[]) {
    const unsigned int threads = strtoul(argv[1], NULL, 10);
    const unsigned int N = 20000000;

    omp_set_num_threads(threads);

    string test;
    test.resize(N);
    cin.read(&test[0], N);

    // double time_start = omp_get_wtime();
    auto table = encodeText(test);
    // double time_end = omp_get_wtime();

    size_t result = 0;
    for (auto const &c: test) {
        result += table[c].size();
    }
    cout << result << endl;

    return 0;
}
