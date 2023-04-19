#pragma once

namespace curling {
enum class Error {
	kCurlInternalError = 1,
	kCurlRequestFailed = 2,

	kCannotCreateFactory = 11,
	kCannotCreateClient = 12,

	kUriIsRelative = 21,
	kUriMustBeAbsolute = 22,
};
}
