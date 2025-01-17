#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>
#include <omp.h>
using namespace std;

// Структура узла дерева
struct Node {
    char ch;
    int freq;
    Node *left, *right;
};

// Функция для создания нового узла дерева
Node* getNode(char ch, int freq, Node* left, Node* right) {
    Node* node = new Node();
    node->ch = ch;
    node->freq = freq;
    node->left = left;
    node->right = right;
    return node;
}

// Объект сравнения для упорядочивания очереди
struct comp {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq;
    }
};

// Кодирование Хаффмана
void encode(Node* root, string str, unordered_map<char, string> &huffmanCode) {
    if (root == nullptr)
        return;

    if (!root->left && !root->right) {
        huffmanCode[root->ch] = str;
    }

    encode(root->left, str + "0", huffmanCode);
    encode(root->right, str + "1", huffmanCode);
}

// Раскодирование строки
void decode(Node* root, int &index, string str) {
    if (root == nullptr)
        return;

    if (!root->left && !root->right) {
        cout << root->ch;
        return;
    }

    index++;

    if (str[index] == '0')
        decode(root->left, index, str);
    else
        decode(root->right, index, str);
}

// Построение дерева Хаффмана и вывод результатов
unordered_map<char, string> buildHuffmanTree(string text) {
    // Подсчёт частот символов
    unordered_map<char, int> freq;

    #pragma omp parallel
    {
        unordered_map<char, int> local_freq;
        #pragma omp for
        for (int i = 0; i < text.size(); i++) {
            local_freq[text[i]]++;
        }

        #pragma omp critical
        for (const auto &pair : local_freq) {
            freq[pair.first] += pair.second;
        }
    }

    // Создание приоритетной очереди
    priority_queue<Node*, vector<Node*>, comp> pq;

    for (auto pair : freq) {
        pq.push(getNode(pair.first, pair.second, nullptr, nullptr));
    }

    // Построение дерева
    while (pq.size() != 1) {
        Node *left = pq.top(); pq.pop();
        Node *right = pq.top(); pq.pop();
        int sum = left->freq + right->freq;
        pq.push(getNode('\0', sum, left, right));
    }

    Node* root = pq.top();

    // Кодирование и вывод кодов Хаффмана
    unordered_map<char, string> huffmanCode;
    encode(root, "", huffmanCode);
    return huffmanCode;
}

// Основная программа
int main(int argc, char *argv[]) {
    const unsigned int threads = strtoul(argv[1], NULL, 10);
    const unsigned int N = 20000000;

    omp_set_num_threads(threads);

    string test;
    test.resize(N);
    cin.read(&test[0], N);
    
    double time_start = omp_get_wtime();
    auto table = buildHuffmanTree(test);
    double time_end = omp_get_wtime();

    size_t result = 0;
    for (auto const &c: test) {
        result += table[c].size();
    }
    cout << result << endl;
    return 0;
}
