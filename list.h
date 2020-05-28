#pragma once

#include <initializer_list>

namespace cym {
    template<typename T>
    class list {
    private:
        T *_val;
        size_t _val_arr_size;
        size_t _end;
    public:

        explicit list(T *arr, const size_t count) : _end(0), _val_arr_size(count) {
            _val = new T[_val_arr_size];
            if (arr == nullptr) {
                return;
            }

            for (; _end < _val_arr_size; ++_end) {
                _val[_end] = arr[_end];
            }
        }

        list(const list &rhs) : list(rhs._val, rhs._end) {}

        list() : list(nullptr, 0) {}

        explicit list(const size_t size) : list(nullptr, size) {}

        list(std::initializer_list<T> &&list) {
            _val_arr_size = list.size();
            _val = new T[_val_arr_size];
            _end = 0;
            for (const auto &item : list) {
                _val[_end++] = item;
            }
        }

    private:
        void double_space() {
            T *new_array = new T[_val_arr_size * 2];
            for (int i = 0; i < _val_arr_size; ++i) {
                new_array[i] = _val[i];
            }
            _val = new_array;
            T *tmp = _val;
            delete[]tmp;
            _val_arr_size *= 2;
        }

    public:
        size_t size() const {
            return _end;
        }

        void append(T e) {
            if (_end == _val_arr_size) {
                double_space();
            }
            _val[_end++] = e;
        }

        T pop() {
            return _val[--_end];
        }

    private:
        int index_for(const int position) {
            if (0 <= position && position < _end) {
                return position;
            }
            if (position < 0 && -position < _end) {
                return _end + position;
            }
            if (0 < position) {
                return index_for(position - _end);
            }
            return index_for(position + _end);
        }

    public:
        void remove(const int position) {
            int index = index_for(position);
            for (int i = index; i < _end - 1; ++i) {
                _val[i] = _val[i + 1];
            }
        }

        T &operator[](const int position) {
            return _val[index_for(position)];
        }

        T &at(const int position) {
            return operator[](position);
        }

    };
}