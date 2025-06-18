// CustomHashTable.h
#ifndef CUSTOMHASHTABLE_H
#define CUSTOMHASHTABLE_H

#include <vector>
#include <list>
#include <utility>
#include <functional>
#include <string>
#include <fstream>
#include <iostream>

template <typename K, typename V>
class CustomHashTable {
private:
    std::vector<std::list<std::pair<K, V>>> table;
    const std::string filename;

public:
    CustomHashTable(size_t size = 100, const std::string& file = "") : table(size), filename(file) {}

    V* find(const K& key) {
        size_t index = std::hash<K>{}(key) % table.size();
        for (auto& node : table[index]) {
            if (node.first == key) {
                return &node.second;
            }
        }
        return nullptr;
    }

    const V* find(const K& key) const { // Const overload for const objects
        size_t index = std::hash<K>{}(key) % table.size();
        for (const auto& node : table[index]) {
            if (node.first == key) {
                return &node.second;
            }
        }
        return nullptr;
    }

    void insert(const K& key, const V& value) {
        size_t index = std::hash<K>{}(key) % table.size();
        for (auto& node : table[index]) {
            if (node.first == key) {
                node.second = value;
                return;
            }
        }
        table[index].emplace_back(key, value);
    }

    bool remove(const K& key) {
        size_t index = std::hash<K>{}(key) % table.size();
        auto it = std::find_if(table[index].begin(), table[index].end(),
                              [&](const std::pair<K, V>& p) { return p.first == key; });
        if (it != table[index].end()) {
            table[index].erase(it);
            return true;
        }
        return false;
    }

    std::vector<V> getAll() const {
        std::vector<V> result;
        for (const auto& bucket : table) {
            for (const auto& node : bucket) {
                result.push_back(node.second);
            }
        }
        return result;
    }

    bool isEmpty() const { return table.empty(); }

    void save(const std::string& filename) const {
        std::ofstream ofs(filename);
        if (ofs.is_open()) {
            for (const auto& bucket : table) {
                for (const auto& node : bucket) {
                    ofs << node.first << "," << node.second << "\n";
                }
            }
            ofs.close();
        } else {
            std::cerr << "Error saving to " << filename << std::endl;
        }
    }

    void load(const std::string& filename) {
        std::ifstream ifs(filename);
        if (ifs.is_open()) {
            std::string line;
            while (std::getline(ifs, line)) {
                std::stringstream ss(line);
                K key;
                V value;
                std::getline(ss, key, ',');
                std::getline(ss, value);
                insert(key, value);
            }
            ifs.close();
        }
    }
};

#endif
