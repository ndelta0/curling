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
endif()
if(CURLING_ENABLE_HTTP2)
	list(APPEND CURL_COMPONENTS HTTP2)
endif()
if(CURLING_ENABLE_HTTP3)
	list(APPEND CURL_COMPONENTS HTTP3)
endif()
if(CURLING_ENABLE_IPV6)
	list(APPEND CURL_COMPONENTS IPv6)
endif()
if(CURLING_ENABLE_LIBZ)
	list(APPEND CURL_COMPONENTS libz)
endif()
if(CURLING_ENABLE_BROTLI)
	list(APPEND CURL_COMPONENTS brotli)
endif()
if(CURLING_ENABLE_ZSTD)
	list(APPEND CURL_COMPONENTS zstd)
endif()
if(CURLING_ENABLE_ASYNC_DNS)
	list(APPEND CURL_COMPONENTS AsynchDNS)
endif()
if(CURLING_ENABLE_LARGE_FILE)
	list(APPEND CURL_COMPONENTS Largefile)
endif()
if(WIN32)
	if(CURLING_ENABLE_UNICODE)
		list(APPEND CURL_COMPONENTS Unicode)
	endif()
elseif(UNIX)
	if(CURLING_ENABLE_UNIX_SOCKETS)
		list(APPEND CURL_COMPONENTS UnixSockets)
	endif()
endif()

include(FindCURL)
find_package(CURL 7 COMPONENTS "${CURL_COMPONENTS}")
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

option(CURLING_ENABLE_JSON "Enable JSON support" ON)
if(CURLING_ENABLE_JSON)
	find_package(nlohmann_json CONFIG REQUIRED)
	list(APPEND COMPILE_DEFS CURLING_HAS_JSON)
endif()

option(CURLING_ENABLE_TRACING "Enable tracing support" OFF)
if(CURLING_ENABLE_TRACING)
	find_package(opentelemetry-cpp CONFIG REQUIRED COMPONENTS api)
	list(APPEND COMPILE_DEFS CURLING_HAS_TRACING)
endif()

configure_file("${curling_SOURCE_DIR}/include/generated/version.hpp.in" "${curling_BINARY_DIR}/include/curling/version.hpp" @ONLY)

file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "${curling_SOURCE_DIR}/include/*.hpp" "${curling_BINARY_DIR}/include/*.hpp")

set(SOURCE_LIST http_factory.cpp http_client.cpp uri.cpp context.cpp http_response_message.cpp http_headers.cpp http_request_headers.cpp http_response_headers.cpp http_content.cpp utils.cpp utils.hpp http_request_message.cpp tracing.cpp tracing.hpp)

add_library(curling STATIC ${HEADER_LIST} ${SOURCE_LIST})

target_compile_definitions(curling PUBLIC ${COMPILE_DEFS})

target_include_directories(curling PUBLIC
	$<BUILD_INTERFACE:${curling_SOURCE_DIR}/include>
	$<BUILD_INTERFACE:${curling_BINARY_DIR}/include>
	$<INSTALL_INTERFACE:include>
	PRIVATE ${curling_SOURCE_DIR}/include/curling ${curling_BINARY_DIR}/include/curling)

target_compile_features(curling PUBLIC cxx_std_17)

target_link_libraries(curling PUBLIC CURL::libcurl)

if(CURLING_ENABLE_JSON)
	target_link_libraries(curling PUBLIC nlohmann_json::nlohmann_json)
endif()

if(CURLING_ENABLE_TRACING)
	target_link_libraries(curling PUBLIC opentelemetry-cpp::api)
endif()

add_library(curling::curling ALIAS curling)

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
configure_package_config_file(${curling_SOURCE_DIR}/cmake/curlingConfig.cmake.in
		${curling_BINARY_DIR}/curlingConfig.cmake
		INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/curling
		PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR)
write_basic_package_version_file(
	"${curling_BINARY_DIR}/curlingConfigVersion.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion)
install(FILES
	"${curling_BINARY_DIR}/curlingConfig.cmake"
	"${curling_BINARY_DIR}/curlingConfigVersion.cmake"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/curling)

install(EXPORT curlingTargets
		NAMESPACE curling::
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/curling)
