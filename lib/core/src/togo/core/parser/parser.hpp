#line 2 "togo/core/parser/parser.hpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.

@file
@brief Parser interface.
@ingroup lib_core_parser
*/

#pragma once

#include <togo/core/config.hpp>
#include <togo/core/types.hpp>
#include <togo/core/utility/traits.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/hash/hash.hpp>
#include <togo/core/parser/types.hpp>
#include <togo/core/parser/parser.gen_interface>

namespace togo {
namespace parser {

#if defined(TOGO_DEBUG)
extern bool s_debug_trace;
#endif

namespace {

template<ParserType T>
inline void series_add_parser(ParserData<T>&, Allocator&, unsigned) {}

template<ParserType T, class... P>
inline void series_add_parser(ParserData<T>& d, Allocator& a, unsigned i, Parser&& p, P&&... rest) {
	d.p[i] = TOGO_CONSTRUCT(a, Parser, rvalue_ref(p));
	TOGO_DEBUG_ASSERTE(d.p[i]->type <= ParserType::Close);
	series_add_parser(d, a, i + 1, forward<P>(rest)...);
}

template<ParserType T, class... P>
inline void series_add_parser(ParserData<T>& d, Allocator& a, unsigned i, Parser const& p, P&&... rest) {
	d.p[i] = &p;
	TOGO_DEBUG_ASSERTE(d.p[i]->type <= ParserType::Close);
	series_add_parser(d, a, i + 1, forward<P>(rest)...);
}

} // anonymous namespace

template<class... P>
inline Any::ParserData(Allocator& a, P&&... p)
	: num(num_params(p...))
	, p(TOGO_ALLOCATE_N(a, Parser const*, num))
{
	series_add_parser(*this, a, 0, forward<P>(p)...);
}

template<class... P>
inline All::ParserData(Allocator& a, P&&... p)
	: num(num_params(p...))
	, p(TOGO_ALLOCATE_N(a, Parser const*, num))
{
	series_add_parser(*this, a, 0, forward<P>(p)...);
}

inline Maybe::ParserData(Parser const& p)
	: p(&p)
{}

inline Maybe::ParserData(Allocator& a, Parser&& p)
	: p(TOGO_CONSTRUCT(a, Parser, rvalue_ref(p)))
{}

inline Close::ParserData(close_func_type* f)
	: f(f)
	, p(nullptr)
{}

inline Close::ParserData(close_func_type* f, Parser const& p)
	: f(f)
	, p(&p)
{}

inline Close::ParserData(Allocator& a, close_func_type* f, Parser&& p)
	: f(f)
	, p(TOGO_CONSTRUCT(a, Parser, rvalue_ref(p)))
{
	TOGO_DEBUG_ASSERTE(this->p->type < ParserType::Close);
}

inline Parser::Parser(StringRef name)
	: type(ParserType::Undefined)
	, name_hash(hash::calc32(name))
	, name(name)
	, s(no_init_tag{})
{}

inline Parser::Parser()
	: Parser(StringRef{})
{}

template<ParserType T>
inline Parser::Parser(StringRef name, ParserData<T>&& d)
	: type(T)
	, name_hash(hash::calc32(name))
	, name(name)
	, s(forward<ParserData<T>&&>(d))
{}

template<ParserType T>
inline Parser::Parser(ParserData<T>&& d)
	: Parser(StringRef{}, forward<ParserData<T>&&>(d))
{}

/// Type name of parser.
inline StringRef type_name(Parser const& p) {
	return parser::type_name(p.type);
}

} // namespace parser
} // namespace togo
