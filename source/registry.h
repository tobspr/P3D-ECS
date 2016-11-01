#pragma once
#ifndef P3ECS_REGISTRY_H
#define P3ECS_REGISTRY_H

#include <map>
#include <functional>
#include <iostream>

template < typename BaseInterface, typename Identifier = uint_fast8_t >
class Registry {
public:
    BaseInterface* construct(Identifier id) const {
        auto it = _entries.find(id);
        if (it == _entries.end())
            return nullptr;
        return it->second();
    };

    bool is_registered(Identifier id) const {
        return _entries.find(id) != _entries.end();
    };

    template < typename T >
    void register_type() {
        if (is_registered(T::class_id)) {
            std::cerr << "Registering type twice!" << std::endl;
            return;
        }

        _entries.emplace(T::class_id, [](){ return new T(); });
    }

private:
    using creator_t = std::function<void()>;
    // using creator_t = void();
    std::map< Identifier, creator_t> _entries;
};

#endif
