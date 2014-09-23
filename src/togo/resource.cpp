#line 2 "togo/resource.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/config.hpp>
#include <togo/utility.hpp>
#include <togo/string.hpp>
#include <togo/fixed_array.hpp>
#include <togo/resource.hpp>

namespace togo {

bool resource::parse_path(
	StringRef const& path,
	ResourcePathParts& pp
) {
	enum : unsigned {
		NOT_FOUND = ~0u,
	};

	pp.type_hash = RESTYPE_NULL;
	pp.name_hash = hash::IDENTITY64;
	pp.type = StringRef{null_tag{}};
	pp.name = StringRef{null_tag{}};
	fixed_array::clear(pp.tags);

	unsigned ext_i = NOT_FOUND;
	unsigned tag_i = NOT_FOUND;
	unsigned tag_first = NOT_FOUND;
	ResourcePathParts::Tag tag;
	for (unsigned i = 0; i < path.size; ++i) {
		if (path.data[i] == '.') {
			if (ext_i == NOT_FOUND) {
				ext_i = i;
			} else {
				return false;
			}
		} else if (path.data[i] == '#') {
			if (tag_i == NOT_FOUND) {
				tag_first = i;
			} else if (tag_i != i) {
				tag.name = {
					path.data + tag_i,
					i - tag_i
				};
				tag.hash = hash::calc32(tag.name);
				fixed_array::push_back(pp.tags, tag);
			}
			tag_i = i + 1;
		}
	}

	unsigned const last_index = path.size - 1;
	unsigned const type_first = ext_i + 1;
	unsigned const type_last = min(tag_first, path.size);
	if (
		ext_i == NOT_FOUND || ext_i == 0 || ext_i == last_index ||
		ext_i > tag_first  || type_last - type_first == 0 ||
		tag_i == path.size ||
		path.data[ext_i - 1] == '/' ||
		path.data[ext_i - 1] == '\\'
	) {
		return false;
	}
	pp.type = StringRef{path.data + type_first, type_last - type_first};
	pp.name = StringRef{path.data, ext_i};
	pp.type_hash = hash::calc_generic<ResourceType>(pp.type);
	pp.name_hash = hash::calc_generic<ResourceNameHash>(pp.name);
	if (tag_i < path.size) {
		tag.name = {
			path.data + tag_i,
			path.size - tag_i
		};
		tag.hash = hash::calc32(tag.name);
		fixed_array::push_back(pp.tags, tag);
	}
	return true;
}

} // namespace togo
