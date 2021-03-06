
#pragma once

#if defined(TOGO_DEBUG)

#define DEBUG_PRINT_PARSER(p_) do { \
	TOGO_LOGF("\n%s @ %4d:\n", __FILE__, __LINE__); \
	parser::debug_print_tree(p_); \
	TOGO_LOG("\n"); \
} while (false)

#else
#define DEBUG_PRINT_PARSER(p_) (void(0))
#endif

#define PARSE(p_, s_) parser::parse(p_, s_, &error, nullptr)
#define PARSE_S(p_, s_) do { \
	parse_state::init(state); \
	parse_state::set_data(state, s_); \
	parser::parse(p_, state, &error); \
} while (false)

#define TEST(p_, s_) parser::test(p_, s_, &error, nullptr)
#define TEST_WHOLE(p_, s_) parser::test_whole(p_, s_, &error, nullptr)
