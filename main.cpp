#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "BPlusTree.h"
#include "StorageManager.h"

using namespace std;

int main() {
    ifstream input("in.txt");
    ofstream output("out.txt");

    if (!input.is_open()) {
        cerr << "Erro ao abrir in.txt" << endl;
        return 1;
    }

    string line;
    int ordem = 0;
    getline(input, line);
    if (line.rfind("FLH/", 0) == 0) {
        ordem = stoi(line.substr(4));
        output << line << endl;
    } else {
        cerr << "Primeira linha do in.txt deve ser FLH/n" << endl;
        return 1;
    }

    BPlusTree arvore(ordem);
    StorageManager storage("vinhos.csv", "dados_arvore.txt");

    while (getline(input, line)) {
        if (line.rfind("INC:", 0) == 0) {
            int x = stoi(line.substr(4));
            int count = storage.insertTuple(x);
            arvore.insert(x);
            output << "INC:" << x << "/" << count << endl;
        } else if (line.rfind("BUS=:", 0) == 0) {
            int x = stoi(line.substr(5));
            vector<string> tuplas = arvore.search(x);
            output << "BUS=:" << x << "/" << tuplas.size() << endl;
        }
    }

    arvore.printTreeToFile("indice.txt");

    output << "H/" << arvore.getHeight() << endl;
    return 0;
}
