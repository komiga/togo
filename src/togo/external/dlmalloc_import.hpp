/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file external/dlmalloc_import.hpp
@brief dlmalloc configuration and import.
*/

#pragma once

#include <togo/config.hpp>
#include <togo/assert.hpp>

#include <cstddef>

/** @cond INTERNAL */

// dlmalloc configuration
#define USE_LOCKS 1
#define ONLY_MSPACES 1
#define MSPACES 1
#define MALLINFO_FIELD_TYPE std::size_t
#define ABORT (::togo::error_abort(__LINE__, __FILE__, "dlmalloc: invalid operation"))

extern "C" {
// Yanked from dlmalloc to shut clang up.
#define _STRUCT_MALLINFO
#define STRUCT_MALLINFO_DECLARED 1
struct mallinfo {
  MALLINFO_FIELD_TYPE arena;    /* non-mmapped space allocated from system */
  MALLINFO_FIELD_TYPE ordblks;  /* number of free chunks */
  MALLINFO_FIELD_TYPE smblks;   /* always 0 */
  MALLINFO_FIELD_TYPE hblks;    /* always 0 */
  MALLINFO_FIELD_TYPE hblkhd;   /* space in mmapped regions */
  MALLINFO_FIELD_TYPE usmblks;  /* maximum total allocated space */
  MALLINFO_FIELD_TYPE fsmblks;  /* always 0 */
  MALLINFO_FIELD_TYPE uordblks; /* total allocated space */
  MALLINFO_FIELD_TYPE fordblks; /* total free space */
  MALLINFO_FIELD_TYPE keepcost; /* releasable (via malloc_trim) space */
};
} // extern "C"

#include <togo/external/dlmalloc.hpp>

/** @endcond */ // INTERNAL
