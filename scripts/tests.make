
TEST_ONLY_RECIPES := \
	lib_core_tests_only \
	lib_image_tests_only \
	lib_window_tests_only \
	lib_platform_tests_only \
	lib_game_tests_only

TEST_RECIPES := \
	lib_core_tests \
	lib_image_tests \
	lib_window_tests \
	lib_platform_tests \
	lib_game_tests

.PHONY: $(TEST_ONLY_RECIPES) tests_only $(TEST_RECIPES) tests clean_tests

lib_core_tests_only:
	@${MAKE} --no-print-directory -C lib/core/test -f Makefile

lib_image_tests_only:
	@${MAKE} --no-print-directory -C lib/image/test -f Makefile

lib_window_tests_only:
	@${MAKE} --no-print-directory -C lib/window/test -f Makefile

lib_platform_tests_only:
	@${MAKE} --no-print-directory -C lib/platform/test -f Makefile

lib_game_tests_only:
	@${MAKE} --no-print-directory -C lib/game/test -f Makefile

tests_only: $(TEST_ONLY_RECIPES)

lib_core_tests: | lib_core
	@${MAKE} --no-print-directory -C lib/core/test -f Makefile

lib_image_tests: | lib_image
	@${MAKE} --no-print-directory -C lib/image/test -f Makefile

lib_window_tests: | lib_window
	@${MAKE} --no-print-directory -C lib/window/test -f Makefile

lib_platform_tests: | lib_platform
	@${MAKE} --no-print-directory -C lib/platform/test -f Makefile

lib_game_tests: | lib_game
	@${MAKE} --no-print-directory -C lib/game/test -f Makefile

tests: $(TEST_RECIPES)

clean_tests:
	@${MAKE} --no-print-directory -C lib/core/test -f Makefile clean
	@${MAKE} --no-print-directory -C lib/image/test -f Makefile clean
	@${MAKE} --no-print-directory -C lib/window/test -f Makefile clean
	@${MAKE} --no-print-directory -C lib/platform/test -f Makefile clean
	@${MAKE} --no-print-directory -C lib/game/test -f Makefile clean

clean:: clean_tests
