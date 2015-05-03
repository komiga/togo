
.PHONY: lib_core_tests lib_window_tests lib_game_tests tests

lib_core_tests: | lib_core
	@${MAKE} --no-print-directory -C lib/core/test -f Makefile

lib_window_tests: | lib_window
	@${MAKE} --no-print-directory -C lib/window/test -f Makefile

lib_game_tests: | lib_game
	@${MAKE} --no-print-directory -C lib/game/test -f Makefile

tests: | lib_core_tests lib_game_tests

clean::
	@${MAKE} --no-print-directory -C lib/core/test -f Makefile clean
	@${MAKE} --no-print-directory -C lib/window/test -f Makefile clean
	@${MAKE} --no-print-directory -C lib/game/test -f Makefile clean
