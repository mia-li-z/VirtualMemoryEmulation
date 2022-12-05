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
    // 如果当前链表为空，我们需要在哈希表中删除，且更新minFreq
    if (freq_table[freq].size() == 0) {
        freq_table.erase(freq);
        if (minfreq == freq) minfreq += 1;
    }
    // 插入到 freq + 1 中
    freq_table[freq + 1].push_front(Node(key, val, freq + 1));
    key_table[key] = freq_table[freq + 1].begin();
    return val;
}


bool CustomCache::put(int key, int value) {

    auto it = key_table.find(key);
    
    if (it == key_table.end()) {
        // 缓存已满，需要进行删除操作
        if (key_table.size() == capacity) {
            //evict_minFreq();
            return false;
        } 
        freq_table[2].push_front(Node(key, value, 2)); //put set freq_to_two
        key_table[key] = freq_table[2].begin(); 
        minfreq = 2;
    } else {
        // 与 get 操作基本一致，除了需要更新缓存的值
        list<Node>::iterator node = it -> second;
        int freq = node -> freq;
        freq_table[freq].erase(node);
        if (freq_table[freq].size() == 0) {
            freq_table.erase(freq);
            if (minfreq == freq) minfreq += 2; //1
        }
        freq_table[freq + 2].push_front(Node(key, value, freq + 2));
        key_table[key] = freq_table[freq + 2].begin();
    }

    return true;
}

pair<int, int> CustomCache::evict(){
    // 通过 minFreq 拿到 freq_table[minFreq] 链表的末尾节点
     auto it2 = freq_table[minfreq].back();
    //auto it2 = freq_table[minfreq].front();
    key_table.erase(it2.key);
    // cout << "victim page="<< it2.key << ", victim frame="<< it2.val<< endl;
     freq_table[minfreq].pop_back();
    // freq_table[minfreq].pop_front();
    if (freq_table[minfreq].size() == 0) {
        freq_table.erase(minfreq);
    }
    return {it2.key, it2.val};
}


