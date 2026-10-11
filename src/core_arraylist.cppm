module;

#include "core_debug.h"

/**
 * @file core_arraylist.cppm
 * @brief Arena-backed auto-expanding array list and premitives
 */

export module core_arraylist;

import core_types;
import core_memory;
import core_result;
import core_option;

using namespace base;

export namespace base::arraylist {
  enum struct ArraylistE : u8 {
    /** @brief No error. */
    None = 0,
    OutOfMemory,
    CapacityOverflow
  };

  template <typename T>
  using ArraylistR = Result<ArraylistE, T>;

  namespace detail {

  }

  template <typename T>
  struct Arraylist {

  };

}
