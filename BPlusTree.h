#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>  // Adicione esta linha

class BPlusNode {
public:
    bool isLeaf;
    std::vector<int> keys;
    std::vector<std::shared_ptr<BPlusNode>> children;
    std::shared_ptr<BPlusNode> next;

    BPlusNode(bool leaf);
    std::string toString(bool isRoot) const;
};

class BPlusTree {
private:
    int ordem;
    std::shared_ptr<BPlusNode> root;
    int height;

    void insertInternal(std::shared_ptr<BPlusNode>& node, int key);
    void printNode(const std::shared_ptr<BPlusNode>& node, std::ofstream& out, 
                 int nodeId, int parentId, bool isRoot,
                 const std::unordered_map<int, std::vector<int>>& yearToWineIds) const;

public:
    BPlusTree(int ordem);
    void insert(int key);
    std::vector<std::string> search(int key);
    int getHeight() const;
    void printTreeToFile(const std::string& filename) const;
};

#endif