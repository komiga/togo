#line 2 "togo/tool_res_build/generator_compiler/support.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#pragma once

#include <togo/tool_res_build/config.hpp>
#include <togo/tool_res_build/types.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/hash/hash.hpp>
#include <togo/game/gfx/types.hpp>
#include <togo/tool_res_build/generator_compiler.hpp>

namespace togo {
namespace tool_res_build {

inline bool rc_find_shared_resource(
	gfx::RenderConfig const& render_config,
	u32 const type,
	hash32 const name_hash,
	u32& index
) {
	index = 0;
	for (auto const& resource : render_config.shared_resources) {
		if (resource.name_hash == name_hash) {
			return resource.type() == type;
		} else if (resource.type() == type) {
			++index;
		}
	}
	index = ~u32{0};
	return false;
}

inline bool rc_get_render_target(
	StringRef const context,
	gfx::RenderConfig const& render_config,
	KVS const& k_var,
	u32& index
) {
	StringRef const name = kvs::string_ref(k_var);
	hash32 const name_hash = hash::calc32(name);
	if (rc_find_shared_resource(
		render_config, gfx::RenderConfigResource::TYPE_RENDER_TARGET, name_hash,
		index
	)) {
		return true;
	} else if (index != ~u32{0}) {
		TOGO_LOG_ERRORF(
			"malformed generator: %.*s: "
			"%.*s '%.*s' is not a render target\n",
			context.size, context.data,
			kvs::name_size(k_var), kvs::name(k_var),
			name.size, name.data
		);
		return false;
	} else {
		TOGO_LOG_ERRORF(
			"malformed generator: %.*s: "
			"%.*s '%.*s' not found\n",
			context.size, context.data,
			kvs::name_size(k_var), kvs::name(k_var),
			name.size, name.data
		);
		return false;
	}
}

} // namespace tool_res_build
} // namespace togo
