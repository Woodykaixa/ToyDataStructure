#pragma once

#include <cmath>
#include "list.h"
#include <type_traits>

namespace cym {
    template<typename T>
    inline size_t hash_value(const T &key) noexcept {
        if (std::is_pointer_v<T> || std::is_null_pointer_v<T>) {
            return reinterpret_cast<size_t>(key) ^ 0xdeadbeefu;
        } else {
            return static_cast<size_t>(key) ^ 0xdeadbeefu;
        }
    }

    template<typename K, typename V>
    class pair {
        K _key;
        V _val;
        pair *_next;
        unsigned int _hash;
    public:
        pair(K k, V v, pair *n) : _key(k), _val(v), _next(n) {
            _hash = get_hash(k);
        }

        pair() : _key(K{}), _val(V{}), _next(nullptr) {}

        pair(const pair &rhs) noexcept {
            _key = rhs._key;
            _val = rhs._val;
            _hash = rhs._hash;
            _next = nullptr;
            if (rhs._next != nullptr) {
                _next = new pair(*rhs._next);
            }
        }

        static unsigned int get_hash(K key) {
            size_t h = hash_value(key);
            return h ^ (h >> 16);
        }

        K key() const { return _key; }

        void set_key(const K &key) { _key = key; }

        V val() const { return _val; }

        void set_val(const V &val) { _val = val; }

        pair<K, V> *next() const { return _next; }

        void set_next(pair<K, V> *next) { _next = next; }

        unsigned hash() const { return _hash; }

        void set_hash(const unsigned hash) { _hash = hash; }
    };

    /**
     * 一个简单的map实现，参考了java 1.7的HashMap.
     */
    template<typename K, typename V>
    class map {

    protected:
        using pair_t = pair<K, V>;
        size_t _size;
        pair_t **_el;
        unsigned int _used;
        double _factor;

    private:
        static size_t size_for_map(size_t size) {
            double i = log2(size);
            double f = floor(i);
            if (f < i) {
                f++;
            }
            return static_cast<size_t>(pow(2, i));
        }

    public:
        explicit map(const size_t size = 16, const double factor = 0.75) : _used(0), _factor(factor) {
            _size = size_for_map(size);
            _el = new pair_t *[_size];
            for (size_t i = 0; i < _size; ++i) {
                _el[i] = nullptr;
            }
        }

        map(map &rhs) {
            _size = rhs._size;
            _used = rhs._used;
            _factor = rhs._factor;
            _el = new pair_t *[_size];
            for (size_t i = 0; i < _size; ++i) {
                _el[i] = new pair_t(*rhs._el[i]);
            }

        }


        ~map() {
            for (size_t i = 0; i < _size; ++i) {
                pair_t *e = _el[i];
                if (e != nullptr) {
                    while (e != nullptr) {
                        pair_t *tmp = e;
                        e = e->next();
                        delete tmp;
                    }
                }
            }
            delete[] _el;
        }

    private:
        void rearrange_pairs(pair_t **nt, const size_t new_size) {
            for (int i = 0; i < _size; ++i) {
                pair_t *e = _el[i];
                if (e == nullptr) {
                    continue;
                }
                while (e != nullptr) {
                    pair_t *next_pair = e->next();

                    unsigned int new_index = e->hash() & (new_size - 1);
                    e->set_next(nt[new_index]);
                    nt[new_index] = e;
                    e = next_pair;
                }
            }
            int new_used = 0;
            for (size_t i = 0; i < new_size; i++) {
                if (nt[i] != nullptr) {
                    new_used++;
                }
            }
            _used = new_used;
        }

    public:

        void resize() {
            const size_t new_size = _size * 2;
            pair_t **new_table = new pair_t *[new_size];
            for (int i = 0; i < new_size; i++) {
                new_table[i] = nullptr;
            }
            rearrange_pairs(new_table, new_size);
            _size = new_size;
            pair_t **old = _el;
            _el = new_table;
            delete[] old;
        }

        void put(K key, V v) {
            pair_t *p = new pair_t(key, v, nullptr);

            const double rate = static_cast<double>(_used) / _size;
            if (_factor <= rate) {
                resize();
            }

            int i = p->hash() & (_size - 1);
            pair_t *e = _el[i];

            if (e == nullptr) {
                _el[i] = p;
                _used++;
                return;
            }
            if (e->next() == nullptr) {
                e->set_next(p);
                return;
            }
            for (pair_t *node = _el[i]; node->next() != nullptr; node = node->next()) {
                node->set_next(p);
            }
        }

        V get(K key, V default_value) {

            const unsigned int hash = pair_t::get_hash(key);

            int i = hash & (_size - 1);

            pair_t *e = _el[i];
            if (e == nullptr) {
                return default_value;
            }
            for (; e != nullptr; e = e->next()) {
                if (e->key() == key) {
                    return e->val();
                }
            }
            return default_value;
        }

        bool has(K key) const {
            const unsigned int hash = pair_t::get_hash(key);
            int i = hash & (_size - 1);
            return _el[i] != nullptr;
        }

        void remove(K key) {
            const unsigned int hash = pair_t::get_hash(key);
            int i = hash & (_size - 1);
            pair_t *e = _el[i];
            if (e == nullptr) {
                return;
            }
            if (e->key() == key) {
                _el[i] = e->next();
                delete e;
                return;
            }
            pair_t *pre = e;
            e = e->next();
            for (; e->next() != nullptr; e = e->next()) {
                if (e->key() == key) {
                    pre->set_next(e->next());
                    delete e;
                }
            }

        }

        list<pair_t> get_pairs() {
            list<pair_t> pairs(_size);
            for (int i = 0; i < _size; i++) {
                if (_el[i] != nullptr) {
                    for (pair_t *j = _el[i]; j != nullptr; j = j->next()) {
                        pairs.append(pair<K, V>(j->key(), j->val(), nullptr));
                    }
                }
            }
            return pairs;
        }

        size_t get_used_count() const {
            return _used;
        }

        size_t size() const {
            return _size;
        }

        bool empty() const {
            return _used == 0;
        }
    };

}
