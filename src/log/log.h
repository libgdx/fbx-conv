/*******************************************************************************
 * Copyright 2011 See AUTHORS file.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/
/** @author Xoppa */
#ifdef _MSC_VER
#pragma once
#endif //_MSC_VER
#ifndef FBXCONV_LOG_LOG_H
#define FBXCONV_LOG_LOG_H

#include <vector>
#include <stdarg.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <cassert>
#include "codes.h"

namespace fbxconv {
namespace log {

	struct LogMessages : std::vector<const char*> {
		LogMessages() { resize(num_codes); }
		virtual ~LogMessages() {}
	};

	class Log {
	public:
		static const int LOG_STATUS		= 0x1 << 0;
		static const int LOG_PROGRESS	= 0x1 << 1;
		static const int LOG_DEBUG		= 0x1 << 2;
		static const int LOG_INFO		= 0x1 << 3;
		static const int LOG_VERBOSE	= 0x1 << 4;
		static const int LOG_WARNING	= 0x1 << 5;
		static const int LOG_ERROR		= 0x1 << 6;

		int filter;
		LogMessages * messages;

		Log(LogMessages * const &messages, const int &filter = -1) : messages(messages), filter(filter) {}

		virtual ~Log() {
			delete messages;
		}

		const char *msg(int code) {
			return (*messages)[code];
		}

		const char *vformat(int code, va_list vl) {
			static char buff[1024];
			vsnprintf(buff, 1024, msg(code), vl);
			return buff;
		}

		const char *vformat(const char *m, va_list vl) {
			static char buff[1024];
			vsnprintf(buff, 1024, m, vl);
			return buff;
		}

		const char *format(int code, ...) {
			va_list vl;
			va_start(vl, code);
			const char *result = vformat(code, vl);
			va_end(vl);
			return result;
		}

		const char *format(const char *m, ...) {
			va_list vl;
			va_start(vl, m);
			const char *result = vformat(m, vl);
			va_end(vl);
			return result;
		}

		virtual void log(const int &type, const char *s) {
			static bool inProgress = false;
			assert(!((type == 0) || (type & (type - 1))));
			if (((filter & type) == 0))
				return;
			if (type  == LOG_PROGRESS) {
				inProgress = true;
				printf("PROGRESS: %-79s\r", s);
			} else {
				if (inProgress) {
					printf("\n");
					inProgress = false;
				}
				switch(type) {
				case LOG_STATUS:	printf("STATUS:   %s\n", s); break;
				case LOG_DEBUG:		printf("DEBUG:    %s\n", s); break;
				case LOG_INFO:		printf("INFO:     %s\n", s); break;
				case LOG_VERBOSE:	printf("VERBOSE:  %s\n", s); break;
				case LOG_WARNING:	printf("WARNING:  %s\n", s); break;
				case LOG_ERROR:		printf("ERROR:    %s\n", s); break;
				default: break;
				}
			}
		}

		virtual void vlog(const int &type, const int &code, va_list vl) {
			log(type, vformat(code, vl));
		}

		virtual void vlog(const int &type, const char *m, va_list vl) {
			log(type, vformat(m, vl));
		}

		virtual void log(const int &type, const int &code, ...) {
			va_list vl; va_start(vl, code); vlog(type, code, vl); va_end(vl);
		}

		virtual void status(const char *s, ...) {
			va_list vl; va_start(vl, s); vlog(LOG_STATUS, s, vl); va_end(vl);
		}

		virtual void status(int code, ...) {
			va_list vl; va_start(vl, code); vlog(LOG_STATUS, code, vl); va_end(vl);
		}

		virtual void progress(const char *s, ...) {
			va_list vl; va_start(vl, s); vlog(LOG_PROGRESS, s, vl); va_end(vl);
		}

		virtual void progress(int code, ...) {
			va_list vl; va_start(vl, code); vlog(LOG_PROGRESS, code, vl); va_end(vl);
		}

 		virtual void debug(const char *s, ...) {
			va_list vl; va_start(vl, s); vlog(LOG_DEBUG, s, vl); va_end(vl);
		}

		virtual void debug(int code, ...) {
			va_list vl; va_start(vl, code); vlog(LOG_DEBUG, code, vl); va_end(vl);
		}

		virtual void info(const char *s, ...) {
			va_list vl; va_start(vl, s); vlog(LOG_INFO, s, vl); va_end(vl);
		}

		virtual void info(int code, ...) {
			va_list vl; va_start(vl, code); vlog(LOG_INFO, code, vl); va_end(vl);
		}

		virtual void verbose(const char *s, ...) {
			va_list vl; va_start(vl, s); vlog(LOG_VERBOSE, s, vl); va_end(vl);
		}

		virtual void verbose(int code, ...) {
			va_list vl; va_start(vl, code); vlog(LOG_VERBOSE, code, vl); va_end(vl);
		}

		virtual void warning(const char *s, ...) {
			va_list vl; va_start(vl, s); vlog(LOG_WARNING, s, vl); va_end(vl);
		}

		virtual void warning(int code, ...) {
			va_list vl; va_start(vl, code); vlog(LOG_WARNING, code, vl); va_end(vl);
		}

		virtual void error(const char *s, ...) {
			va_list vl; va_start(vl, s); vlog(LOG_ERROR, s, vl); va_end(vl);
		}

		virtual void error(int code, ...) {
			va_list vl; va_start(vl, code); vlog(LOG_ERROR, code, vl); va_end(vl);
		}
	};

	const int Log::LOG_STATUS;
	const int Log::LOG_PROGRESS;
	const int Log::LOG_DEBUG;
	const int Log::LOG_VERBOSE;
	const int Log::LOG_INFO;
	const int Log::LOG_WARNING;
	const int Log::LOG_ERROR;
} }

#define LOG_START_MESSAGES(clazz) \
	struct clazz : LogMessages { \
		clazz() {
#define LOG_SET_MSG(code, msg) (*this)[code] = msg;
#define LOG_END_MESSAGES() \
		} \
	};

#endif //FBXCONV_LOG_LOG_H