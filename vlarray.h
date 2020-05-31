#pragma once

#include <cstdlib>
#include <cstring>

namespace cym {
    template <typename T>
    class vlarray {
      public:
        explicit vlarray(const size_t size = 10) {
            size_ = size;
            el_ = new T[size_];
        }

        vlarray(T* elements, const size_t size) : vlarray(size) {
            memcpy(el_, elements, sizeof(T) * size_);
        }

        vlarray(vlarray& v) {
            size_ = v.size_;
            el_ = new T[size_];
            memcpy(el_, v.el_, sizeof(T) * size_);
        }

        vlarray& operator=(const vlarray& v) {
            if (v == *this) {
                return *this;
            }
            size_ = v.size_;
            el_ = new T[size_];
            memcpy(el_, v.el_, sizeof(T) * size_);
            return *this;
        }

        ~vlarray() { delete[] el_; }

        int get_size() const { return size_; }

        /**
         * 如果pos大于0，和普通数组一样返回元素，否则倒序返回元素。
         * @param pos 想要获取的元素的位置
         */
        T& operator[](const int pos) {
            if (0 <= pos && pos < static_cast<int>(size_)) {
                return el_[pos];
            }
            if (-static_cast<int>(size_) < pos && pos < 0) {
                return el_[size_ - pos];
            }
            resize();
            return operator[](pos);
        }

        T& at(const int pos) { return operator[](pos); }

        void resize() {
            size_ *= 2;
            el_ = static_cast<T*>(
                realloc(static_cast<void*>(el_), size_ * sizeof(T)));
        }

      private:
        T* el_;
        size_t size_;
    };
} // namespace cym
