#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <iostream>
#include <iterator>
#include <list>
#include <unordered_map>
#include <unistd.h>
using namespace std;

class LRUCache {
public:
    LRUCache(int capacity);

    int get(int key);

    bool put(int key, int value);

    pair<int, int> evict();

private:
    unsigned int cap;
    list<pair<int, int>> cache;
    unordered_map<int, list<pair<int, int>>::iterator> map;
};

#endif