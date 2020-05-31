#pragma once
#include "heap.h"

namespace cym {

    template <typename T>
    void swap(T& t1, T& t2) {
        T tmp = t1;
        t1 = t2;
        t2 = tmp;
    }

    class sort {
      public:
        template <typename T>
        static void quick_sort(T arr[], const int first, const int last) {
            swap(arr[first], arr[(first + last) / 2]);
            int lb = first + 1;
            int hb = last;

            T pivot = arr[first];

            while (lb <= hb) {
                while (arr[lb] < pivot) {
                    lb++;
                }
                while (pivot < arr[hb]) {
                    hb--;
                }
                if (lb < hb) {
                    swap(arr[lb], arr[hb]);
                    lb++;
                    hb--;
                } else {
                    lb++;
                }
            }
            swap(arr[first], arr[hb]);
            if (1 < hb - first) {
                quick_sort(arr, first, hb - 1);
            }
            if (1 < last - hb) {
                quick_sort(arr, hb + 1, last);
            }
        }

        template <typename T>
        static void quick_sort_s(T arr[], const size_t size,
                                 const bool inverse = false) {
            int max = 0;
            if (size < 2) {
                return;
            }
            for (int i = 1; i < size; ++i) {
                if (arr[max] < arr[i]) {
                    max = i;
                }
            }
            if (size - max != 1) {
                swap(arr[max], arr[size - 1]);
            }
            quick_sort(arr, 0, size - 2);
            if (!inverse) {
                return;
            }
            for (int i = 0; i <= size / 2; ++i) {
                if (2 * i != size - 1) {
                    swap(arr[i], arr[size - i - 1]);
                }
            }
        }

        template <typename T>
        static void heap_sort(T arr[], const size_t size,
                              const bool inverse = false) {
            heap_type type = heap_type::min;
            if (inverse) {
                type = heap_type::max;
            }
            heap<T> heap(arr, size, type);
            for (size_t i = 0; i < size; ++i) {
                arr[i] = heap.top();
                heap.delete_top();
            }
        }

      private:
        template <typename T>
        struct element_wrapper {
            size_t val;
            int next;

            int get_nth_bit(const size_t bit, const size_t radix) const {
                size_t tmp = val;
                size_t counter = 0;
                while (tmp != 0) {
                    if (counter == bit - 1) {
                        break;
                    }
                    tmp /= radix;
                    counter++;
                }
                return static_cast<int>(tmp % radix);
            }
        };

        struct radix_sort_helper_queue {
            int head = -1;
            int tail = -1;
        };

        template <typename T>
        static void _wrapper_radix_sort(element_wrapper<T> arr[],
                                        const size_t size, const size_t radix,
                                        size_t max_bit) {
            radix_sort_helper_queue* queue = new radix_sort_helper_queue[radix];
            int first = 0;
            for (size_t i = 0; i < max_bit; ++i) {
                int current = first;
                for (int j = 0; j < radix; ++j) {
                    queue[j].head = -1;
                }
                while (current != -1) {
                    const int queue_index =
                        arr[current].get_nth_bit(i + 1, radix);
                    if (queue[queue_index].head == -1) {
                        queue[queue_index].head = current;
                    } else {
                        arr[queue[queue_index].tail].next = current;
                    }
                    queue[queue_index].tail = current;
                    current = arr[current].next;
                }

                size_t k = 0;
                while (queue[k].head == -1) {
                    k++;
                }
                first = queue[k].head;
                int last = queue[k].tail;
                while (k < radix - 1) {
                    k++;
                    while (k < radix - 1 && queue[k].head == -1) {
                        k++;
                    }
                    if (queue[k].head != -1) {
                        arr[last].next = queue[k].head;
                        last = queue[k].tail;
                    }
                }
                arr[last].next = -1;
            }

            int j = first;

            for (int i = 0; i < size; ++i) {
                element_wrapper<T> tmp = arr[j];
                arr[j] = arr[i];
                arr[i] = tmp;
                arr[i].next = j;
                j = tmp.next;
                while (j <= i) {
                    j = arr[j].next;
                }
            }
            delete[] queue;
        }

      public:
        template <typename T>
        static void radix_sort(T arr[], const size_t size, const size_t radix,
                               const bool inverse = false) {
            size_t max_index = 0;
            for (size_t i = 1; i < size; ++i) {
                if (arr[max_index] < arr[i]) {
                    max_index = i;
                }
            }
            size_t tmp = static_cast<size_t>(arr[max_index]);
            size_t max_bit = 0;
            while (tmp != 0) {
                tmp /= radix;
                max_bit++;
            }

            if (max_index != size - 1) {
                swap(arr[max_index], arr[size - 1]);
            }
            element_wrapper<T>* wrappers = new element_wrapper<T>[size - 1];

            for (size_t i = 0; i < size - 1; ++i) {
                wrappers[i].val = static_cast<size_t>(arr[i]);
                wrappers[i].next = i + 1;
            }
            wrappers[size - 1].next = -1;
            _wrapper_radix_sort(wrappers, size - 1, radix, max_bit);

            for (size_t i = 0; i < size - 1; ++i) {
                arr[i] = static_cast<T>(wrappers[i].val);
            }

            if (inverse) {
                for (int i = 0; i <= size / 2; ++i) {
                    if (2 * i != size - 1) {
                        swap(arr[i], arr[size - i - 1]);
                    }
                }
            }
        }
    };
} // namespace cym
