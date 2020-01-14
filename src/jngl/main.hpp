// Copyright 2015-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#pragma once

#include "Finally.hpp"
#include "other.hpp"
#include "window.hpp"

#if !defined(JNGL_MAIN_BEGIN)
	#if defined(ANDROID)
		#include <android_native_app_glue.h>

		namespace jngl {
			extern android_app* androidApp;
		}
		#define JNGL_MAIN_BEGIN void android_main(android_app* __androidApp) { \
			jngl::androidApp = __androidApp; \
			jngl::setConfigPath(__androidApp->activity->internalDataPath); \
			jngl::Finally _ZtzNg47T5XSjogv(jngl::hideWindow);
		#define JNGL_MAIN_END }
	#else
		#define JNGL_MAIN_BEGIN /* NOLINT */ int main(int argc, char** argv) { \
			{ \
				std::vector<std::string> tmp(argc - 1); \
				for (int i = 1; i < argc; ++i) { \
					tmp[i - 1] = argv[i]; \
				} \
				jngl::setArgs(tmp); \
			} \
			jngl::Finally _ZtzNg47T5XSjogv(jngl::hideWindow);
		#define JNGL_MAIN_END } // NOLINT
	#endif
#endif
