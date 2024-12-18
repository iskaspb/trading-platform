find_package(Catch2 QUIET)

add_custom_target(all_tests COMMENT "Build all tests")

function(testSuiteImpl TEST_SUITE_NAME)
	if(NOT TEST_SUITE_NAME)
		message(FATAL_ERROR "Test suite name is not provided")
		return()
	endif()

	set(options EXCLUDE_FROM_ALL)
	set(multiValueArgs SRC DEPENDENCIES)
	cmake_parse_arguments(ARG "${options}" "" "${multiValueArgs}" ${ARGN})

	if(NOT ARG_SRC)
		message(FATAL_ERROR "SRC is not provided for test suite ${TEST_SUITE_NAME}")
		return()
	endif()

	file(GLOB TEST_SRC ${ARG_SRC})
	if(NOT TEST_SRC)
		message(STATUS "Skip test suite ${TEST_SUITE_NAME} : no source files found")
		return()
	endif()

	if(ARG_EXCLUDE_FROM_ALL)
		add_executable(${TEST_SUITE_NAME} EXCLUDE_FROM_ALL ${TEST_SRC})
		message(STATUS "Add standalone test suite : ${TEST_SUITE_NAME}")
	else()
		add_executable(${TEST_SUITE_NAME} ${TEST_SRC})
		add_test(NAME ${TEST_SUITE_NAME} COMMAND ${TEST_SUITE_NAME})
		add_dependencies(all_tests ${TEST_SUITE_NAME})
		message(STATUS "Add test suite : ${TEST_SUITE_NAME}")
	endif()

	separate_arguments(ARG_DEPENDENCIES)
	target_link_libraries(${TEST_SUITE_NAME} ${ARG_DEPENDENCIES})
endfunction()

function(c2test TEST_SUITE_NAME)
	if(NOT TARGET Catch2::Catch2WithMain)
		message(WARNING "Skip test suite ${TEST_SUITE_NAME} : Catch2 is not found")
		return()
	endif()

	testSuiteImpl(${TEST_SUITE_NAME} ${ARGN})
	if(TARGET ${TEST_SUITE_NAME})
		target_link_libraries(${TEST_SUITE_NAME} Catch2::Catch2WithMain)
	endif()
endfunction()

c2test(basic_test SRC basic/*_test.cxx)
c2test(md_test SRC md/*_test.cxx)
c2test(ob_test SRC ob/*_test.cxx)
