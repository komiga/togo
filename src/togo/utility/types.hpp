#line 2 "togo/utility/types.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Utility types.
@ingroup utility
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>

namespace togo {

/**
	@addtogroup utility
	@{
*/

/// Array reference.
template<class T>
struct ArrayRef {
	T* _begin;
	T* _end;
};

/** @} */ // end of doc-group utility

} // namespace togo
