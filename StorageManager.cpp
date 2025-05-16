#include "StorageManager.h"
#include <sstream>
#include <iostream>

StorageManager::StorageManager(const std::string& dataFile, const std::string& indexFile)
    : dataFile(dataFile), indexFile(indexFile) {
    vinhosIn.open(dataFile);
    indexOut.open(indexFile, std::ios::out | std::ios::trunc);
    if (!vinhosIn.is_open() || !indexOut.is_open()) {
        std::cerr << "Erro ao abrir arquivos." << std::endl;
    }
    std::string header;
    getline(vinhosIn, header); // Ignora cabeçalho
}

StorageManager::~StorageManager() {
    vinhosIn.close();
    indexOut.close();
}

int StorageManager::insertTuple(int ano) {
    vinhosIn.clear();
    vinhosIn.seekg(0);
    std::string line, header;
    getline(vinhosIn, header); // Ignora cabeçalho
    int count = 0;
    while (getline(vinhosIn, line)) {
        std::stringstream ss(line);
        std::string id, rotulo, ano_str, tipo;
        getline(ss, id, ',');
        getline(ss, rotulo, ',');
        getline(ss, ano_str, ',');
        getline(ss, tipo, ',');
        if (std::stoi(ano_str) == ano) {
            indexOut << line << std::endl;
            count++;
        }
    }
    return count;
}
