/**
 * @file core_algorithm.cppm
 * @brief Small generic algorithm primitives used across core modules.
 */

/**
 * @module core_algorithm
 * @brief Sorting and utility helpers that operate on caller-owned buffers.
 */
export module core_algorithm;

import core_types;

// Non-exported forward declarations of implementation details for this 
// module interface unit.
namespace base::algorithm_detail {
  template <typename T, typename Compare>
  void sort_quicksort_internal(T* items, i64 low, i64 high, Compare comp);
}

/**
 * @namespace base
 * @brief Foundational low-level utilities for reusable core modules.
 */
export namespace base {

  /**
   * @brief Default strict-weak ordering predicate (`a < b`).
   */
  struct Less {
    template <typename T>
    constexpr bool operator()(const T& a, const T& b) const {
      return a < b;
    }
  };

  /**
   * @brief Swaps two values using copy assignment semantics.
   * @tparam T Value type.
   * @param a First value.
   * @param b Second value.
   */
  template <typename T>
  inline void swap(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
  }

  /**
   * @brief Sorts a buffer in-place using insertion sort.
   * @tparam T Element type.
   * @tparam Compare Comparator type that returns true when lhs should come before rhs.
   * @param items Buffer to sort.
   * @param count Number of elements in the buffer.
   * @param comp Comparator instance.
   * @note Best for small ranges; used as the small-partition fallback for quicksort.
   */
  template <typename T, typename Compare = Less>
  void sort_insertion(T* items, u64 count, Compare comp = Compare{}) {
    if (count <= 1 || items == nullptr) return;
    for (u64 i = 1; i < count; ++i) {
      T temp = items[i];
      u64 j = i;
      while (j > 0 && comp(temp, items[j - 1])) {
        items[j] = items[j - 1];
        --j;
      }
      items[j] = temp;
    }
  }
  /**
   * @brief Sorts a buffer in-place using quicksort.
   * @tparam T Element type.
   * @tparam Compare Comparator type.
   * @param items Buffer to sort.
   * @param count Number of elements in the buffer.
   * @param comp Comparator instance.
   */
  template <typename T, typename Compare = Less>
  void sort_quicksort(T* items, u64 count, Compare comp = Compare{}) {
    if (count <= 1 || items == nullptr) return;
    algorithm_detail::sort_quicksort_internal(
      items,
      0,
      static_cast<i64>(count - 1),
      comp
    );
  }

}   // namespace base

// Non-exported namespace with internal details of algorithms.
namespace base::algorithm_detail {

  template <typename T, typename Compare>
  void sort_quicksort_internal(T* items, i64 low, i64 high, Compare comp) {
    constexpr i64 INSERTION_THRESHOLD = 16;
    while (low < high) {
      if ((high - low + 1) <= INSERTION_THRESHOLD) {
        base::sort_insertion(items + low, static_cast<u64>(high - low + 1), comp);
        break;
      }

      i64 mid = low + (high - low) / 2;
      if (comp(items[mid], items[low]))   swap(items[low], items[mid]);
      if (comp(items[high], items[low]))  swap(items[low], items[high]);
      if (comp(items[high], items[mid]))  swap(items[mid], items[high]);

      swap(items[mid], items[high - 1]);
      T pivot = items[high - 1];

      i64 i = low;
      i64 j = high - 1;

      for (;;) {
        while (comp(items[++i], pivot)) {}
        while (comp(pivot, items[--j])) {}
        if (i >= j) break;
        swap(items[i], items[j]);
      }

      swap(items[i], items[high - 1]);

      if ((i - 1 - low) < (high - (i + 1))) {
        sort_quicksort_internal(items, low, i - 1, comp);
        low = i + 1;
      } else {
        sort_quicksort_internal(items, i + 1, high, comp);
        high = i - 1;
      }
    }
  }

} // namespace base::algorithm_detail