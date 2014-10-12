#line 2 "togo/args.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Argument parsing.
@ingroup utility
*/

#pragma once

#include <togo/config.hpp>
#include <togo/types.hpp>
#include <togo/kvs_types.hpp>

namespace togo {

/**
	@addtogroup utility
	@{
*/

/// Parse program arguments.
///
/// Returns true if a command was parsed.
bool parse_args(
	KVS& options,
	KVS& command,
	signed const argc,
	char const* const argv[]
);

/** @} */ // end of doc-group utility

} // namespace togo
