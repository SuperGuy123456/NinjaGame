#ifndef NINJAGAME_REFERENCEPOOL_H
#define NINJAGAME_REFERENCEPOOL_H

#include <string>
#include <unordered_map>

class ReferencePool {
public:
    // Store a pointer under a name
    static void Add(const std::string& name, void* ptr);

    // Retrieve a pointer by name (returns nullptr if not found)
    static void* Get(const std::string& name);

    // Remove a reference (does NOT delete the object)
    static void Remove(const std::string& name);

private:
    static std::unordered_map<std::string, void*> refs;
};

#endif
