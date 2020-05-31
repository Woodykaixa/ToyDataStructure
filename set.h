#pragma once

#include "list.h"
#include "map.h"

namespace cym {
    template <typename K>
    class set {
      private:
        map<K, char>* _map;
        static constexpr char default_value = 'a';

      public:
        explicit set(const size_t size = 16, const double factor = 0.75) {
            _map = new map<K, char>(size, factor);
        }

        void put(K key) const { _map->put(key, default_value); }

        bool has(K key) const { return _map->has(key); }

        void remove(K key) const { _map->remove(key); }

        bool empty() const { return _map->empty(); }

        size_t count() const { return _map->get_used_count(); }

        list<K> get_all() const {
            list<K> keys(_map->size());
            list<pair<K, char>> pairs = _map->get_pairs();
            for (int i = 0; i < pairs.size(); ++i) {
                keys.append(pairs[i].key());
            }
            return keys;
        }
    };
} // namespace cym