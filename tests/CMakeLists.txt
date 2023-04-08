find_package(Catch2 QUIET)

if(NOT Catch2_FOUND)
	message(STATUS "Catch2 not found, downloading...")
	include(FetchContent)
	FetchContent_Declare(
			Catch2
			GIT_REPOSITORY https://github.com/catchorg/Catch2.git
			GIT_TAG v3.3.2
	)
	FetchContent_MakeAvailable(Catch2)
	list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
endif()

add_executable(tests http_factory_tests.cpp uri_tests.cpp http_client_tests.cpp)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(tests PRIVATE curling)
