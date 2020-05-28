#pragma once

#include <initializer_list>
#include <cstring>

namespace cym {

    template<typename T, size_t dimension>
    class static_multi_dimension_array {
    private:
        size_t _total;
        size_t *_size_arr;
        T *_val_arr;
        using arr_ty = size_t (&)[dimension];

        static void copy_to_array(std::initializer_list<size_t>
                                  &list, arr_ty container) {
            int index = 0;
            for (const auto &l:list) {
                container[index++] = l;
                if (index == dimension) {
                    break;
                }
            }
            for (; index < dimension; ++index) {
                container[index] = 0;
            }
        }

        void delegate_init(arr_ty size_array) {
            _size_arr = new size_t[dimension];
            memcpy(_size_arr, size_array, sizeof(size_t) * dimension);
            _total = _size_arr[0];
            for (size_t i = 1; i < dimension; ++i) {
                if (_size_arr[i] <= 0) {
                    continue;
                }
                _total *= _size_arr[i];
            }
            _val_arr = new T[_total];
        }

    public:
        explicit static_multi_dimension_array(arr_ty sizes) {
            _size_arr = new size_t[dimension];
            memcpy(_size_arr, sizes, sizeof(size_t) * dimension);
            _total = _size_arr[0];
            for (size_t i = 1; i < dimension; ++i) {
                if (_size_arr[i] <= 0) {
                    continue;
                }
                _total *= _size_arr[i];
            }
            _val_arr = new T[_total];
        }

        explicit static_multi_dimension_array(arr_ty sizes, T init_value)
                : static_multi_dimension_array(sizes) {
            for (int i = 0; i < _total; ++i) {
                _val_arr[i] = init_value;
            }
        }

        static_multi_dimension_array(std::initializer_list<size_t> &&size_list) {
            size_t arr[dimension];
            copy_to_array(size_list, arr);
            delegate_init(arr);

        }

        static_multi_dimension_array(std::initializer_list<size_t> &&size_list, T init_value) {
            size_t arr[dimension];
            copy_to_array(size_list, arr);
            delegate_init(arr);
            for (int i = 0; i < _total; ++i) {
                _val_arr[i] = init_value;
            }
        }

        static_multi_dimension_array(const static_multi_dimension_array &&rhs) noexcept {
            _total = rhs._total;
            _size_arr = new size_t[dimension];
            memcpy(_size_arr, rhs._size_arr, sizeof(size_t) * dimension);
            _val_arr = new T[_total];
            memcpy(_val_arr, rhs._val_arr, sizeof(T) * _total);
        }

        static_multi_dimension_array(const static_multi_dimension_array &rhs) : static_multi_dimension_array(
                std::move(rhs)) {}

        T &visit(arr_ty locations) {
            size_t ind = locations[dimension - 1];
            for (int i = 0; i < dimension - 1; ++i) {
                ind += locations[i] * _size_arr[i + 1];
            }
            return _val_arr[ind];
        }

        T &visit(std::initializer_list<size_t> &&locations) {
            size_t arr[dimension];
            copy_to_array(locations, arr);
            return visit(arr);

        }
    };
}