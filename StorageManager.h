#ifndef STORAGEMANAGER_H
#define STORAGEMANAGER_H

#include <string>
#include <fstream>

class StorageManager {
private:
    std::string dataFile;
    std::string indexFile;
    std::ifstream vinhosIn;
    std::ofstream indexOut;

public:
    StorageManager(const std::string& dataFile, const std::string& indexFile);
    ~StorageManager();
    int insertTuple(int ano);
};

#endif
