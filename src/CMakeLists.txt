# Find CURL version 7
set(CURL_COMPONENTS HTTP)
set(CURL_OPTIONAL_COMPONENTS)

option(CURLING_ENABLE_HTTPS "Enable HTTPS support" ON)
option(CURLING_ENABLE_HTTP2 "Enable HTTP2 support" ON)
option(CURLING_ENABLE_HTTP3 "Enable HTTP3 support" OFF)
option(CURLING_ENABLE_IPV6 "Enable IPv6 support" ON)
option(CURLING_ENABLE_LIBZ "Enable LibZ support" ON)
option(CURLING_ENABLE_BROTLI "Enable Brotli support" OFF)
option(CURLING_ENABLE_ZSTD "Enable Zstd support" OFF)
option(CURLING_ENABLE_ASYNC_DNS "Enable Async DNS support" ON)
option(CURLING_ENABLE_LARGE_FILE "Enable Large File support" ON)
if(WIN32)
	option(CURLING_ENABLE_UNICODE "Enable Unicode support" ON)
elseif(UNIX)
	option(CURLING_ENABLE_UNIX_SOCKETS "Enable Unix Sockets support" OFF)
endif()

if(CURLING_ENABLE_HTTPS)
	list(APPEND CURL_COMPONENTS HTTPS SSL)
else()
	list(APPEND CURL_OPTIONAL_COMPONENTS HTTPS SSL)
endif()
if(CURLING_ENABLE_HTTP2)
	list(APPEND CURL_COMPONENTS HTTP2)
else()
	list(APPEND CURL_OPTIONAL_COMPONENTS HTTP2)
endif()
if(CURLING_ENABLE_HTTP3)
	list(APPEND CURL_COMPONENTS HTTP3)
endif()
if(CURLING_ENABLE_IPV6)
	list(APPEND CURL_COMPONENTS IPv6)
else()
	list(APPEND CURL_OPTIONAL_COMPONENTS IPv6)
endif()
if(CURLING_ENABLE_LIBZ)
	list(APPEND CURL_COMPONENTS libz)
else()
	list(APPEND CURL_OPTIONAL_COMPONENTS libz)
endif()
if(CURLING_ENABLE_BROTLI)
	list(APPEND CURL_COMPONENTS brotli)
else()
	list(APPEND CURL_OPTIONAL_COMPONENTS brotli)
endif()
if(CURLING_ENABLE_ZSTD)
	list(APPEND CURL_COMPONENTS zstd)
else()
	list(APPEND CURL_OPTIONAL_COMPONENTS zstd)
endif()
if(CURLING_ENABLE_ASYNC_DNS)
	list(APPEND CURL_COMPONENTS AsynchDNS)
else()
	list(APPEND CURL_OPTIONAL_COMPONENTS AsynchDNS)
endif()
if(CURLING_ENABLE_LARGE_FILE)
	list(APPEND CURL_COMPONENTS Largefile)
else()
	list(APPEND CURL_OPTIONAL_COMPONENTS Largefile)
endif()
if(WIN32)
	if(CURLING_ENABLE_UNICODE)
		list(APPEND CURL_COMPONENTS Unicode)
	else()
		list(APPEND CURL_OPTIONAL_COMPONENTS Unicode)
	endif()
elseif(UNIX)
	if(CURLING_ENABLE_UNIX_SOCKETS)
		list(APPEND CURL_COMPONENTS UnixSockets)
	else()
		list(APPEND CURL_OPTIONAL_COMPONENTS UnixSockets)
	endif()
endif()

include(FindCURL)
find_package(CURL 7 COMPONENTS "${CURL_COMPONENTS}" OPTIONAL_COMPONENTS "${CURL_OPTIONAL_COMPONENTS}")
if(NOT CURL_FOUND)
	# List components that were not found
	set(CURL_MISSING_COMPONENTS)
	foreach(COMPONENT ${CURL_COMPONENTS})
		if(NOT CURL_${COMPONENT}_FOUND)
			list(APPEND CURL_MISSING_COMPONENTS ${COMPONENT})
		endif()
	endforeach()

	message(FATAL_ERROR "CURL version 7 with components ${CURL_MISSING_COMPONENTS} not found")
endif()

set(COMPILE_DEFS)
foreach(CURL_COMPONENT ${CURL_COMPONENTS})
	list(APPEND COMPILE_DEFS CURLING_HAS_${CURL_COMPONENT})
endforeach()
foreach(CURL_OPTIONAL_COMPONENT ${CURL_OPTIONAL_COMPONENTS})
	if(CURL_${CURL_OPTIONAL_COMPONENT}_FOUND)
		list(APPEND COMPILE_DEFS CURLING_HAS_${CURL_OPTIONAL_COMPONENT})
	endif()
endforeach()

option(CURLING_ENABLE_JSON "Enable JSON support" ON)
if(CURLING_ENABLE_JSON)
	find_package(nlohmann_json CONFIG REQUIRED)
	list(APPEND COMPILE_DEFS CURLING_HAS_JSON)
endif()

configure_file("${curling_SOURCE_DIR}/include/generated/version.hpp.in" "${curling_BINARY_DIR}/include/curling/version.hpp" @ONLY)

file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "${curling_SOURCE_DIR}/include/*.hpp" "${curling_BINARY_DIR}/include/*.hpp")

set(SOURCE_LIST http_factory.cpp http_client.cpp uri.cpp context.cpp http_response_message.cpp http_headers.cpp http_request_headers.cpp http_response_headers.cpp http_content.cpp utils.cpp utils.hpp http_request_message.cpp)

add_library(curling STATIC ${HEADER_LIST} ${SOURCE_LIST})

target_compile_definitions(curling PUBLIC ${COMPILE_DEFS})

target_include_directories(curling INTERFACE ${curling_SOURCE_DIR}/include ${curling_BINARY_DIR}/include)
target_include_directories(curling PRIVATE ${curling_SOURCE_DIR}/include/curling ${curling_BINARY_DIR}/include/curling)

target_compile_features(curling PUBLIC cxx_std_17)

target_link_libraries(curling PUBLIC CURL::libcurl)

if(CURLING_ENABLE_JSON)
	target_link_libraries(curling PUBLIC nlohmann_json::nlohmann_json)
endif()

include(GNUInstallDirs)

install(TARGETS curling
	EXPORT curlingTargets
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES "${curling_SOURCE_DIR}/include/curling.hpp" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY "${curling_SOURCE_DIR}/include/curling" "${curling_BINARY_DIR}/include/curling" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	"${curling_BINARY_DIR}/curlingConfigVersion.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion)

configure_file("${curling_SOURCE_DIR}/cmake/curlingConfig.cmake.in" "${curling_BINARY_DIR}/curlingConfig.cmake" @ONLY)

install(FILES
	"${curling_BINARY_DIR}/curlingConfig.cmake"
	"${curling_BINARY_DIR}/curlingConfigVersion.cmake"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/curling)
