cmake_minimum_required(VERSION 3.19...3.26)

project(
		curling
		VERSION 0.1.0
		DESCRIPTION "A powerful CURL wrapper so easy, a kid can use it!"
		HOMEPAGE_URL "https://github.com/ndelta0/curling"
		LANGUAGES CXX
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	option(CURLING_BUILD_DOCS "Build documentation" ON)
	find_package(Doxygen)
	if(CURLING_BUILD_DOCS AND Doxygen_FOUND)
		add_subdirectory(docs)
	elseif(CURLING_BUILD_DOCS AND NOT Doxygen_FOUND)
		message(FATAL_ERROR "Doxygen is needed to build the documentation.")
	endif()

	option(CURLING_BUILD_TESTS "Build tests" ON)
	if(CURLING_BUILD_TESTS)
		include(CTest)
		enable_testing()
		add_subdirectory(tests)
	endif()

	option(CURLING_BUILD_EXAMPLES "Build examples" ON)
	if(CURLING_BUILD_EXAMPLES)
		add_subdirectory(examples)
	endif()

	if(MSVC)
		add_compile_options(/W4 /wd4068) # /wd4068: unknown pragma
	else()
		add_compile_options(-Wall -Wextra -pedantic -Wno-unknown-pragmas)
	endif()

endif()

add_subdirectory(src)
