#ifndef CUSTOM_CACHE_H
#define CUSTOM_CACHE_H

#include <iostream>
#include <iterator>
#include <list>
#include <unordered_map>
#include <unistd.h>
using namespace std;
struct Node {
    int key, val, freq;
    Node(int _key,int _val,int _freq): key(_key), val(_val), freq(_freq){}
};
class CustomCache {
public:
    CustomCache(int _capacity);

    int get(int key);

    bool put(int key, int value);

    // pair<int, int> evict_minFreq();

    pair<int, int> evict();

private:
    unsigned int capacity;
    int minfreq;
    unordered_map<int, list<Node>::iterator> key_table;
    unordered_map<int, list<Node>> freq_table;
};

#endif