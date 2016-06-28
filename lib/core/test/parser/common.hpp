
#pragma once

#if defined(TOGO_DEBUG)

#define DEBUG_PRINT_PARSER(p_) do { \
	parser::debug_print_tree(p_); \
	TOGO_LOG("\n"); \
} while (false)

#else
#define DEBUG_PRINT_PARSER(p_) (void(0))
#endif

#define PARSE(p_, s_) parse(p_, s_, &error, nullptr)
#define TEST(p_, s_) test(p_, s_, &error, nullptr)
#define TEST_WHOLE(p_, s_) test_whole(p_, s_, &error, nullptr)
