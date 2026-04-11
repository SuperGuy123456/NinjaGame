#include "ReferencePool.h"

std::unordered_map<std::string, void*> ReferencePool::refs;

void ReferencePool::Add(const std::string& name, void* ptr) {
    refs[name] = ptr;
}

void* ReferencePool::Get(const std::string& name) {
    auto it = refs.find(name);
    if (it != refs.end()) return it->second;
    return nullptr;
}

void ReferencePool::Remove(const std::string& name) {
    refs.erase(name);
}
