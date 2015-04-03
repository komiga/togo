#line 2 "togo/core/io/object_buffer_type.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Object buffer type.
@ingroup lib_core_types
@ingroup lib_core_io
@ingroup lib_core_io_object_buffer
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/memory/types.hpp>
#include <togo/core/io/memory_stream.hpp>

namespace togo {

/**
	@addtogroup lib_core_io_object_buffer
	@{
*/

/// Object buffer.
///
/// @tparam T Integral type to use for object types.
/// @tparam S Integral type to use for object sizes.
template<class T, class S>
struct ObjectBuffer {
	TOGO_CONSTRAIN_INTEGRAL(T);
	TOGO_CONSTRAIN_INTEGRAL(S);

	MemoryStream _stream;
	unsigned _num_objects;
	bool _consume_mode;

	~ObjectBuffer() = default;

	ObjectBuffer() = delete;
	ObjectBuffer(ObjectBuffer const&) = delete;
	ObjectBuffer(ObjectBuffer&&) = delete;
	ObjectBuffer& operator=(ObjectBuffer const&) = delete;
	ObjectBuffer& operator=(ObjectBuffer&&) = delete;

	ObjectBuffer(Allocator& allocator, u32 const init_capacity);
};

/** @} */ // end of doc-group lib_core_io_object_buffer

} // namespace togo
