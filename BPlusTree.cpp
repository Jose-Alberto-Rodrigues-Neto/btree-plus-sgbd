#include "BPlusTree.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <locale>
#include <queue>
#include <unordered_map>

BPlusNode::BPlusNode(bool leaf) : isLeaf(leaf), next(nullptr) {}

BPlusTree::BPlusTree(int ordem) : ordem(ordem), height(1) {
    root = std::make_shared<BPlusNode>(true);
}

static inline std::string trim(const std::string& s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(*start)) {
        start++;
    }

    auto end = s.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));
    return std::string(start, end + 1);
}

void BPlusTree::insert(int key) {
    insertInternal(root, key);

    if (root->keys.size() >= ordem) {
        auto newRoot = std::make_shared<BPlusNode>(false);
        newRoot->children.push_back(root);

        auto sibling = std::make_shared<BPlusNode>(root->isLeaf);
        int mid = ordem / 2;

        if (mid < root->keys.size()) {
            sibling->keys.assign(root->keys.begin() + mid, root->keys.end());
            root->keys.resize(mid);

            if (!root->isLeaf) {
                if (mid + 1 <= root->children.size()) {
                    sibling->children.assign(root->children.begin() + mid + 1, root->children.end());
                    root->children.resize(mid + 1);
                }
            } else {
                sibling->next = root->next;
                root->next = sibling;
            }

            if (!sibling->keys.empty()) {
                newRoot->keys.push_back(sibling->keys[0]);
                newRoot->children.push_back(sibling);
                root = newRoot;
                height++;
            } else {
                std::cerr << "Erro: irmão vazio após split da raiz!" << std::endl;
            }
        }
    }
}

void BPlusTree::insertInternal(std::shared_ptr<BPlusNode>& node, int key) {
    if (node->isLeaf) {
        auto it = std::lower_bound(node->keys.begin(), node->keys.end(), key);
        node->keys.insert(it, key);
    } else {
        int idx = 0;
        while (idx < node->keys.size() && key >= node->keys[idx]) {
            ++idx;
        }

        // Correção: garantir que idx não ultrapasse os limites
        if (idx >= node->children.size()) {
            idx = node->children.size() - 1;  // Vai para o último filho
        }

        insertInternal(node->children[idx], key);

        if (node->children[idx]->keys.size() >= ordem) {
            auto child = node->children[idx];
            auto sibling = std::make_shared<BPlusNode>(child->isLeaf);
            int mid = ordem / 2;

            if (mid < child->keys.size()) {
                sibling->keys.assign(child->keys.begin() + mid, child->keys.end());
                child->keys.resize(mid);

                if (!child->isLeaf) {
                    if (mid + 1 <= child->children.size()) {
                        sibling->children.assign(child->children.begin() + mid + 1, child->children.end());
                        child->children.resize(mid + 1);
                    }
                } else {
                    sibling->next = child->next;
                    child->next = sibling;
                }

                if (!sibling->keys.empty()) {
                    // Correção: inserir no local correto
                    if (idx < node->keys.size()) {
                        node->keys.insert(node->keys.begin() + idx, sibling->keys[0]);
                    } else {
                        node->keys.push_back(sibling->keys[0]);
                    }
                    node->children.insert(node->children.begin() + idx + 1, sibling);
                }
            }
        }
    }
}

std::vector<std::string> BPlusTree::search(int key) {
    std::shared_ptr<BPlusNode> current = root;
    std::vector<std::string> result;

    // 1. Navegação até a folha correta
    while (!current->isLeaf) {
        auto it = std::lower_bound(current->keys.begin(), current->keys.end(), key);
        int idx = it - current->keys.begin();
        
        // Se key é maior que todas as chaves, vai para o último filho
        if (idx >= current->children.size()) {
            idx = current->children.size() - 1;
        }
        current = current->children[idx];
    }

    // 2. Busca nas folhas encadeadas
    bool found = false;
    while (current != nullptr) {
        // Verifica se a chave pode estar nesta folha
        if (!current->keys.empty() && current->keys.front() > key) {
            break;
        }

        // Procura a chave nesta folha
        if (std::binary_search(current->keys.begin(), current->keys.end(), key)) {
            found = true;
            break;
        }

        current = current->next;
    }

    if (!found) {
        return result;
    }

    // 3. Busca no arquivo apenas se a chave existe
    std::ifstream indexIn("dados_arvore.txt");
    if (!indexIn.is_open()) {
        std::cerr << "Erro ao abrir arquivo indice.txt" << std::endl;
        return result;
    }

    std::string line;
    while (getline(indexIn, line)) {
        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> tokens;

        // Extrai todos os campos da linha
        while (getline(iss, token, ',')) {
            token = trim(token);
            tokens.push_back(token);
        }

        // Verifica se temos pelo menos 4 campos (incluindo o ano)
        if (tokens.size() >= 4) {
            try {
                int ano = std::stoi(tokens[2]); // Assumindo que o ano está na 3ª posição
                if (ano == key) {
                    result.push_back(line);
                }
            } catch (const std::exception& e) {
                std::cerr << "Erro ao converter ano: " << tokens[2] << std::endl;
            }
        }
    }

    return result;
}


int BPlusTree::getHeight() const {
    return height;
}

std::string BPlusNode::toString(bool isRoot) const {
    std::stringstream ss;
    ss << (isRoot ? "RAIZ" : (isLeaf ? "FOLHA" : "NO_INTERNO"));
    ss << " | Chaves: [";
    for (size_t i = 0; i < keys.size(); ++i) {
        if (i != 0) ss << ", ";
        ss << keys[i];
    }
    ss << "] | Filhos: " << children.size();
    ss << " | Next: " << (next ? "SIM" : "NÃO");
    return ss.str();
}

// Função auxiliar para carregar os mapeamentos de ano para vinho_id
std::unordered_map<int, std::vector<int>> mapAnoId(const std::string& filename) {
    std::unordered_map<int, std::vector<int>> yearToWineIds;
    std::ifstream file(filename);
    std::string line;
    
    // Pular cabeçalho
    std::getline(file, line);
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> tokens;
        
        while (std::getline(iss, token, ',')) {
            tokens.push_back(token);
        }
        
        if (tokens.size() >= 4) {
            try {
                int wineId = std::stoi(tokens[0]);
                int year = std::stoi(tokens[2]);
                yearToWineIds[year].push_back(wineId);
            } catch (...) {
                // Ignora linhas com formato inválido
            }
        }
    }
    
    return yearToWineIds;
}

void BPlusTree::printNode(const std::shared_ptr<BPlusNode>& node, std::ofstream& out, 
                        int nodeId, int parentId, bool isRoot,
                        const std::unordered_map<int, std::vector<int>>& yearToWineIds) const {
    out << "ID: " << nodeId << " | ";
    out << "Parent: " << (parentId == -1 ? "NONE" : std::to_string(parentId)) << " | ";
    out << node->toString(isRoot) << " | ";
    
    // Referências (apenas para folhas)
    if (node->isLeaf) {
        out << "Refs: [";
        bool firstKey = true;
        for (int year : node->keys) {
            auto it = yearToWineIds.find(year);
            if (it != yearToWineIds.end()) {
                for (int wineId : it->second) {
                    if (!firstKey) out << ", ";
                    firstKey = false;
                    out << wineId;
                }
            }
        }
        out << "]";
    } else {
        out << "Refs: N/A";
    }
    out << std::endl;
    
    // Imprime os filhos recursivamente
    for (size_t i = 0; i < node->children.size(); ++i) {
        printNode(node->children[i], out, nodeId * 10 + (i + 1), nodeId, false, yearToWineIds);
    }
}

void BPlusTree::printTreeToFile(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Erro ao abrir arquivo " << filename << " para escrita." << std::endl;
        return;
    }
    
    // Carrega os mapeamentos de ano para vinho_id
    std::unordered_map<int, std::vector<int>> yearToWineIds = mapAnoId("vinhos.csv");
    
    out << "ESTRUTURA DA ÁRVORE B+" << std::endl;
    out << "Formato: id - type (folha ou raiz) - keys - children - next - refs (vinho_ids)" << std::endl;
    out << "---------------------------------------------------------------" << std::endl;
    
    if (root) {
        printNode(root, out, 1, -1, true, yearToWineIds);
    } else {
        out << "Árvore vazia." << std::endl;
    }
    
    out.close();
}