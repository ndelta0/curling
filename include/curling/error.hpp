#pragma once

namespace curling {
enum class Error {
	kCurlInternalError = 1,
	kCurlRequestFailed = 2,
	kCurlSslError = 3,

	kCannotCreateFactory = 11,
	kCannotCreateClient = 12,

	kUriIsRelative = 21,
	kUriMustBeAbsolute = 22,
	kCouldNotResolveHost = 23,
	kCouldNotConnect = 24,
	kHttpError = 25,
	kAborted = 26,
};
}
