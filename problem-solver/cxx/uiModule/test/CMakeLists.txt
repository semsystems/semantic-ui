file(GLOB TEST_FILES "units/*.cpp")

add_executable(test_starter "ui-translator-tests-starter.cpp" "${TEST_FILES}" FileUtils.cpp FileUtils.hpp)
target_link_libraries(test_starter
		sc-agents-common
		uiModule
		sc-test-framework
		sc-builder-lib)
