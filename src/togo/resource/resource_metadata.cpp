#line 2 "togo/resource/resource_metadata.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/collection/fixed_array.hpp>
#include <togo/string/string.hpp>
#include <togo/resource/types.hpp>
#include <togo/resource/resource_metadata.hpp>

#include <cstdio>

namespace togo {

void resource_metadata::compiled_path(
	ResourceMetadata const& metadata,
	FixedArray<char, 24>& str
) {
	string::copy(str, ".compiled/");
	unsigned const size = string::size(str);
	// TODO: string::append_integer()
	signed const id_size = std::snprintf(
		fixed_array::begin(str) + size,
		fixed_array::capacity(str) - size,
		"%u", metadata.id
	);
	TOGO_ASSERTE(id_size > 0);
	fixed_array::resize(str, size + id_size + 1);
	// Don't Trust the Standard Library: A Pessimist's Guide
	fixed_array::back(str) = '\0';
}

} // namespace togo
