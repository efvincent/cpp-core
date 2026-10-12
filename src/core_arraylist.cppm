module;

#include "core_debug.h"
#include "string.h"

/**
 * @file core_arraylist.cppm
 * @brief Arena-backed auto-expanding array list and premitives
 */

export module core_arraylist;

import core_types;
import core_memory;
import core_result;
import core_option;
import core_console;

using namespace base;
using namespace console;

/**
 * @namespace base::arraylist
 * @brief Arena backed, dynamically growable generic array list
 */
export namespace base::arraylist {
  
  /**
   * @brief Failure categories for arena-backed array list functions
   */
  enum struct ArraylistE : u8 {
    /** @brief No error. */
    None = 0,
    OutOfMemory,
    CapacityOverflow
  };

  // Forward declare Arraylist
  template <typename T>
  struct Arraylist;

  template <typename T>
  using ArraylistR = Result<ArraylistE, T>;

  namespace detail {
    template <typename T>
    void extend(Arena& arena, Arraylist<T>& list) {
      u64 current_cap = list.capacity == 0 ? 8 : list.capacity;
      u64 new_cap = 0;

      // 1 Megabyte threshold (adjust based on your typical workloads)
      constexpr u64 THRESHOLD_BYTES = 1024 * 1024; 
      
      u64 current_bytes = current_cap * sizeof(T);

      if (current_bytes < THRESHOLD_BYTES) {
          // Geometric growth: Double it
          new_cap = current_cap * 2;
      } else {
          // Arithmetic growth: Add enough elements to fill another 1MB chunk
          u64 elements_per_megabyte = THRESHOLD_BYTES / sizeof(T);
          // Ensure we add at least 1 element if sizeof(T) > 1MB
          u64 increment = (elements_per_megabyte > 0) ? elements_per_megabyte : 1; 
          new_cap = current_cap + increment;
      }

      printfl(arena, "extending from %li to %li", current_bytes, new_cap * sizeof(T));

      // Allocate new buffer, copy, and abandon the old buffer in the arena
      T* new_data = arena.alloc_array<T>(new_cap);
      if (list.data) {
          // Note: If T is not trivially copyable, you must loop and move/copy
          // Assuming Orthodox C++ standard layout structs here
          memcpy(new_data, list.data, list.len * sizeof(T));
      }

      list.data = new_data;
      list.capacity = new_cap;
    }    
  }

  /**
   * @brief Defines a struct type Arraylist, which is a growable, array based list.
   * elements are appended to the end of the list, and the list manages its own
   * growth when capacity is reached
   * 
   * @tparam T The type of values stored in the list
   */
  template <typename T>
  struct Arraylist { NON_COPYABLE(Arraylist)

    T* data;
    u64 capacity;
    u64 len;

    T& operator[](u64 index) {
        BASE_ASSERT(index < len);
        return data[index];
    }

    // Read-only access: Allows reading from a const Arraylist
    const T& operator[](u64 index) const {
        BASE_ASSERT(index < len);
        return data[index];
    }

    /**
     * @brief push a value of type T to the end of the list.
     * 
     * @param arena Arena from which to allocate should the list need to
     * grow to accomodate the new value 
     * @param value The value to append to the list
     */
    void push_back(auto& arena, T value) {
      if (len >= capacity) {
        detail::extend<T>(arena, *this);        
      }
      this->data[len++] = value;
    }

    /**
     * @brief Create new ArrayList of type T and allocate an initial buffer
     * 
     * @param arena Arena from which to allocate the new buffer
     * @param capacity (Optional) The initial capacity in number of values
     * of type T to allocate for the new array list. Defaults to 1 << 5.
     */
    constexpr Arraylist(Arena& arena, u64 capacity = 1 << 5) {
      auto buffer = arena.alloc_array<T>(capacity);
      this->capacity = capacity;
      this->data = buffer;
      this->len = 0;
    }
  };
  
}
