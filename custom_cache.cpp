#include "custom_cache.h"
#include <iostream>
#include <unordered_map>
#include <unistd.h>
#include <assert.h>

CustomCache::CustomCache(int _capacity) {
    minfreq = 0;
    capacity = _capacity;
    key_table.clear();
    freq_table.clear();
}

int CustomCache::get(int key) {

    auto it = key_table.find(key);
    if (it == key_table.end()) return -1;
    list<Node>::iterator node = it -> second;
    int val = node -> val, freq = node -> freq;
    freq_table[freq].erase(node);

    // If freq_table[freq] is empty, we need to erase it and update minFreq
    if (freq_table[freq].size() == 0) {
        freq_table.erase(freq);
        if (minfreq == freq) minfreq += 1;
    }
    
    // insert into freq + 1 
    freq_table[freq + 1].push_front(Node(key, val, freq + 1));
    key_table[key] = freq_table[freq + 1].begin();
    return val;
}


bool CustomCache::put(int key, int value) {


        auto it = key_table.find(key);
        if (it == key_table.end()) {
            // no free frames
            if (key_table.size() == capacity) {
               //evict_minFreq();
               return false;
            } 
            freq_table[1].push_front(Node(key, value, 1));
            key_table[key] = freq_table[1].begin();
            minfreq = 1;
        } else {
            // same as get, need to update table
            list<Node>::iterator node = it -> second;
            int freq = node -> freq;
            freq_table[freq].erase(node);
            if (freq_table[freq].size() == 0) {
                freq_table.erase(freq);
                if (minfreq == freq) minfreq += 1;
            }
            freq_table[freq + 1].push_front(Node(key, value, freq + 1));
            key_table[key] = freq_table[freq + 1].begin();
        }

    return true;
}

pair<int, int> CustomCache::evict(){

    // get the last node from freq_table[minFreq] 
    auto it2 = freq_table[minfreq].back();
    key_table.erase(it2.key);

    freq_table[minfreq].pop_back();
    if (freq_table[minfreq].size() == 0) {
        freq_table.erase(minfreq);
    }
    return {it2.key, it2.val};
}


