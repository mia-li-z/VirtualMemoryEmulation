#include "lru_cache.h"
#include <iostream>
#include <unordered_map>
#include <unistd.h>
#include <assert.h>

LRUCache::LRUCache(int capacity) : cap(capacity) {
}

int LRUCache::get(int key) {
    if (map.find(key) == map.end()) return -1;
    auto key_value = *map[key];
    cache.erase(map[key]);
    cache.push_front(key_value);
    map[key] = cache.begin();
    return key_value.second;
}


bool LRUCache::put(int key, int value) {
    if (map.find(key) == map.end()) {
        if (cache.size() == cap) {
            return false; // cannot put in cache
        }
    }
    else {
        cache.erase(map[key]);
    }
    cache.push_front({key, value});
    map[key] = cache.begin();
    return true;
}
// return the least recent used page
pair<int, int> LRUCache::evict(){

    assert(cache.size() == cap);

    pair<int, int> victim;
    victim = cache.back(); // the page to be evicted
    // cout << "the victim page=" << cache.back().first << 
    // ", frame="<<cache.back().second << endl; 
    map.erase(cache.back().first);
    cache.pop_back();

    assert(cache.size() < cap);

    return victim;
}