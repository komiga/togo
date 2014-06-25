#line 2 "togo/math.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file math.hpp
@brief Math interface.
@ingroup math
*/

#pragma once

#include <togo/config.hpp>
#include <togo/math_types.hpp>

#include <am/linear/vector_interface.hpp>
#include <am/linear/vector_operations.hpp>
#include <am/linear/matrix_interface.hpp>
#include <am/linear/matrix_operations.hpp>

namespace togo {

/**
	@addtogroup math
	@{
*/

using am::linear::length;
using am::linear::distance;
using am::linear::dot;
using am::linear::cross;
using am::linear::normalize;
using am::linear::faceforward;
using am::linear::reflect;
using am::linear::refract;

using am::linear::transpose;
using am::linear::determinant;
using am::linear::inverse;

/** @} */ // end of doc-group math

} // namespace togo
