#pragma once

#include "vlarray.h"

namespace cym {

    enum class heap_type { min, max };

    template <typename T>
    class heap {

      private:
        heap_type _ty;
        vlarray<T>* _el;
        size_t _used;

      public:
        explicit heap(const heap_type type) : _ty(type), _used(0) {
            _el = new vlarray<T>;
        }

        heap(T* elements, const size_t length, const heap_type type)
            : heap(type) {
            for (size_t i = 0; i < length; ++i) {
                insert(elements[i]);
            }
        }

        heap_type type() const { return _ty; }

        bool empty() const { return _used == 0; }

      private:
        /**
         * 用于insert()中，寻找新插入元素的位置。判断是否需要将parent移动到child的位置。
         * 对于最大堆，应该判断 parent < item，对于最小堆则是判断 parent >
         * item。
         */
        bool _move_parent_to_child(T& parent, T& item) {
            const bool result = parent < item;
            return _ty == heap_type::max == result;
        }

        static int parent_for(const int child) { return (child - 1) / 2; }

      public:
        void insert(T e) {
            int i = _used;
            _used++;
            for (; _move_parent_to_child(_el->at(parent_for(i)), e) && i != 0;
                 i = parent_for(i)) {
                _el->at(i) = _el->at(parent_for(i));
            }
            _el->at(i) = e;
        }

        T top() const { return _el->at(0); }

      private:
        /**
         * 用于delete_top()中，判断应当移动左子节点还是右子节点。
         */
        bool _use_right_child(T& left, T& right) {
            const bool result = left < right;
            return _ty == heap_type::max == result;
        }

        /**
         * 用于delete_top()中，判断item变量是否找到了适合的位置（即是否可以退出循环）
         */
        bool _item_position_found(T& item, T& child) {
            const bool result = item >= child;
            return _ty == heap_type::max == result;
        }

      public:
        void delete_top() {
            if (_used == 0) {
                return;
            }
            T item = _el->at(_used - 1);
            _used--;
            int parent_index, child_index;
            for (parent_index = 0; parent_index < _used;
                 parent_index = child_index) {
                if (parent_index == 0) {
                    child_index = 1;
                } else {
                    child_index = parent_index * 2;
                }
                if (child_index > _used) {
                    break;
                }
                if (child_index < _used - 1 &&
                    _use_right_child(_el->at(child_index),
                                     _el->at(child_index + 1))) {
                    child_index++;
                }
                if (_item_position_found(item, _el->at(child_index))) {
                    break;
                }
                _el->at(parent_index) = _el->at(child_index);
            }
            _el->at(parent_index) = item;
        }
    };
} // namespace cym
