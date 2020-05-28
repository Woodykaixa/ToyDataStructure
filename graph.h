#pragma once

#include "multi_dimension_array.h"
#include "heap.h"
#include "set.h"

namespace cym {

    using matrix_t = static_multi_dimension_array<int, 2>;
    using vector_t = static_multi_dimension_array<int, 1>;

    class graph {
    public:
        class edge {
        public:
            edge(unsigned int from, unsigned int to, int weight, bool directed)
                    : _from(from), _to(to), _weight(weight), _directed(directed) {}

            edge(unsigned int from, unsigned int to, bool directed)
                    : _from(from), _to(to), _weight(1), _directed(directed) {}

            bool operator==(const edge &rhs) const {
                if (this->_directed != rhs._directed) {
                    return false;
                }
                if (_directed) {
                    return this->_from == rhs._from
                           && this->_to == rhs._to
                           && this->_weight == rhs._weight;
                }
                if (this->_weight != rhs._weight) {
                    return false;
                }
                int sub1 = static_cast<int>(this->_to) - static_cast<int>(this->_from);
                int sub2 = static_cast<int>(rhs._to) - static_cast<int>(rhs._from);
                return (sub1 + sub2 == 0) || (sub1 - sub2 == 0);
            }

            bool operator!=(const edge &rhs) const {
                return !(*this == rhs);
            }

            unsigned int from() const {
                return _from;
            }

            void set_from(unsigned int from) {
                _from = from;
            }

            unsigned int to() const {
                return _to;
            }

            void set_to(unsigned int to) {
                _to = to;
            }

            int weight() const {
                return _weight;
            }

            void set_weight(int weight) {
                _weight = weight;
            }

            bool directed() const {
                return _directed;
            }

            void set_directed(bool directed) {
                _directed = directed;
            }

        private:
            unsigned int _from;
            unsigned int _to;
            int _weight;
            bool _directed;
        };

        struct dist {
            int len;
            int pre;
            int index;
            static constexpr int infinity = 2147483647; // INT_MAX
            explicit dist(int len = infinity, int pre = -1, int index = -1) : len(len), pre(pre), index(index) {}

            dist(const dist &) = default;

            bool operator<(const dist &rhs) const {
                return len < rhs.len;
            }

            bool operator>(const dist &rhs) const {
                return rhs < *this;
            }

            bool operator<=(const dist &rhs) const {
                return !(rhs < *this);
            }

            bool operator>=(const dist &rhs) const {
                return !(*this < rhs);
            }
        };

        virtual void set_edge(const edge &) = 0;

        virtual void remove_edge(const edge &) = 0;

        virtual edge get_edge(size_t from, size_t to) = 0;

        virtual size_t edge_count() const = 0;

        virtual size_t vertex_count() const = 0;

    };

    class directed_graph : public graph {
    private:
        matrix_t *_connection_matrix;
        size_t _v_count;
        size_t _e_count;
    public:
        explicit directed_graph(size_t v_count) : _v_count(v_count), _e_count(0) {
            _connection_matrix = new matrix_t({v_count, v_count}, -1);
            for (size_t i = 0; i < _v_count; ++i) {
                _connection_matrix->visit({i, i}) = 0;
            }
        }

        directed_graph(const directed_graph &rhs) {
            _connection_matrix = new matrix_t(*rhs._connection_matrix);
            _v_count = rhs._v_count;
            _e_count = rhs._e_count;
        }

        void set_edge(const edge &edge) override {
            unsigned int from = edge.from();
            unsigned int to = edge.to();
            int weight = edge.weight();
            _connection_matrix->visit({from, to}) = weight;
            _e_count++;
        }

        void remove_edge(const edge &edge) override {
            unsigned int from = edge.from();
            unsigned int to = edge.to();
            _connection_matrix->visit({from, to}) = 0;
            _e_count--;
        }

        size_t edge_count() const override {
            return _e_count;
        }

        size_t vertex_count() const override {
            return _v_count;
        }

        edge get_edge(size_t from, size_t to) override {
            return {static_cast<unsigned int>(from), static_cast<unsigned int>(to),
                    _connection_matrix->visit({from, to}), false};
        }

        /**
         * 求最短路径，对于给定的起点`from`，使用Dijkstra算法求出`from`到每个顶点之间的最短路径。
         * @param from 最短路径的起点
         * @return 返回从`from`到个点的最短路径
         */
        vector_t shortest_path(const size_t from) const {
            bool *visited = new bool[_v_count];
            memset(visited, 0, sizeof(int) * _v_count);
            dist *dists = new dist[_v_count];
            for (int i = 0; i < _v_count; ++i) {
                dists[i].len = dist::infinity;
                dists[i].index = i;
                dists[i].pre = from;
            }
            dists[from].len = 0;
            heap<dist> current_dist(heap_type::min);
            current_dist.insert(dists[from]);
            for (int i = 0; i < _v_count; ++i) {
                bool found_min = false;
                dist min;
                while (!current_dist.empty()) {
                    min = current_dist.top();
                    current_dist.delete_top();
                    if (!visited[min.index]) {
                        found_min = true;
                        break;
                    }
                }
                if (!found_min) {
                    break;
                }
                int v = min.index;
                visited[v] = true;
                for (int u = 0; u < _v_count; ++u) {
                    auto begin = static_cast<size_t>(v);
                    auto end = static_cast<size_t>(u);
                    if (_connection_matrix->visit({begin, end}) < 1) {
                        continue;
                    }
                    if (dists[u].len == dist::infinity ||
                        dists[u].len > dists[v].len + _connection_matrix->visit({begin, end})) {
                        dists[u].len = dists[v].len + _connection_matrix->visit({begin, end});
                        dists[u].pre = v;
                        current_dist.insert(dists[u]);
                    }
                }
            }
            vector_t path_vector({_v_count}, 0);
            for (size_t i = 0; i < _v_count; ++i) {
                if (i == from) {
                    continue;
                }
                path_vector.visit({i}) = dists[i].len;

            }
            return path_vector;
        }

    private:
        static void ensure_non_negative_weight(matrix_t &matrix, const size_t from, const size_t to) {
            if (matrix.visit({from, to}) < 0) {
                matrix.visit({from, to}) = dist::infinity;
            }
        }

    public:
        /**
         * 求最短路径，未指定起点，使用Floyd算法求每两个顶点之间的最短路径。
         * @return 返回每两个顶点之间的最短路径
         */
        matrix_t shortest_path() const {
            matrix_t path_matrix(*_connection_matrix);
            for (size_t i = 0; i < _v_count; ++i) {
                for (size_t j = 0; j < _v_count; ++j) {
                    for (size_t k = 0; k < _v_count; ++k) {
                        if (i == j || i == k || j == k) { continue; }
                        ensure_non_negative_weight(path_matrix, i, j);
                        ensure_non_negative_weight(path_matrix, i, k);
                        ensure_non_negative_weight(path_matrix, k, j);
                        const bool ij_is_infinity = path_matrix.visit({i, j}) == dist::infinity;
                        const bool others_are_not_infinity = path_matrix.visit({i, k}) != dist::infinity &&
                                                             path_matrix.visit({k, j}) != dist::infinity;
                        const int ik_kj_weight_sum = path_matrix.visit({i, k}) +
                                                     path_matrix.visit({k, j});
                        const bool has_shorter_path = path_matrix.visit({i, j}) > ik_kj_weight_sum;

                        if (others_are_not_infinity && (ij_is_infinity || has_shorter_path)) {
                            path_matrix.visit({i, j}) = ik_kj_weight_sum;
                        }
                    }
                }
            }
            return path_matrix;
        }

        directed_graph minimum_spanning_tree(const size_t root) {
            directed_graph mst(_v_count);

            set<size_t> vertex_set(_v_count);
            set<size_t> to_add_vertex_set(_v_count);
            for (size_t i = 0; i < _v_count; ++i) {
                if (i == root) {
                    continue;
                }
                to_add_vertex_set.put(i);
            }
            vertex_set.put(root);
            while (vertex_set.count() != _v_count) {
                list<size_t> added = vertex_set.get_all();
                list<size_t> to_add = to_add_vertex_set.get_all();
                int min_weight = dist::infinity;
                int from = -1;
                int to = -1;
                for (size_t i = 0; i < added.size(); i++) {
                    for (size_t j = 0; j < to_add.size(); j++) {
                        int current_weight = _connection_matrix->visit({added[i], to_add[j]});
                        if (0 < current_weight && current_weight < min_weight) {
                            min_weight = current_weight;
                            from = added[i];
                            to = to_add[j];
                        }
                    }
                }
                if (to == -1) {
                    break;
                }
                vertex_set.put(to);
                to_add_vertex_set.remove(to);
                mst.set_edge(edge(from, to, min_weight, true));
            }
            return mst;
        }


    };
}