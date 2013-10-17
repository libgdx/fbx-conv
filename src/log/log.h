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
		static const int LOG_STATUS		= 0x01;
		static const int LOG_PROGRESS	= 0x02;
		static const int LOG_DEBUG		= 0x04;
		static const int LOG_INFO		= 0x08;
		static const int LOG_WARNING	= 0x10;
		static const int LOG_ERROR		= 0x20;

		const int filter;
		LogMessages * const &messages;

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

		const char *format(int code, ...) {
			va_list vl;
			va_start(vl, code);
			const char *result = vformat(code, vl);
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
				case LOG_WARNING:	printf("WARNING:  %s\n", s); break;
				case LOG_ERROR:		printf("ERROR:    %s\n", s); break;
				default: break;
				}
			}
		}

		virtual void vlog(const int &type, const int &code, va_list vl) {
			log(type, vformat(code, vl));
		}

		virtual void log(const int &type, const int &code, ...) {
			va_list vl; va_start(vl, code); vlog(type, code, vl); va_end(vl);
		}

		virtual void status(const char *s) {
			log(Log::LOG_STATUS, s);
		}

		virtual void status(int code, ...) {
			va_list vl; va_start(vl, code); vlog(Log::LOG_STATUS, code, vl); va_end(vl);
		}

		virtual void progress(const char *s) {
			log(LOG_PROGRESS, s);
		}

		virtual void progress(int code, ...) {
			va_list vl; va_start(vl, code); vlog(LOG_PROGRESS, code, vl); va_end(vl);
		}

		virtual void debug(const char *s) {
			log(LOG_DEBUG, s);
		}

		virtual void debug(int code, ...) {
			va_list vl; va_start(vl, code); vlog(LOG_DEBUG, code, vl); va_end(vl);
		}

		virtual void info(const char *s) {
			log(LOG_INFO, s);
		}

		virtual void info(int code, ...) {
			va_list vl; va_start(vl, code); vlog(LOG_INFO, code, vl); va_end(vl);
		}

		virtual void warning(const char *s) {
			log(LOG_WARNING, s);
		}

		virtual void warning(int code, ...) {
			va_list vl; va_start(vl, code); vlog(LOG_WARNING, code, vl); va_end(vl);
		}

		virtual void error(const char *s) {
			log(LOG_ERROR, s);
		}

		virtual void error(int code, ...) {
			va_list vl; va_start(vl, code); vlog(LOG_ERROR, code, vl); va_end(vl);
		}
	};
} }

#define LOG_START_MESSAGES(clazz) \
	struct clazz : LogMessages { \
		clazz() {
#define LOG_SET_MSG(code, msg) (*this)[code] = msg;
#define LOG_END_MESSAGES() \
		} \
	};

#endif //FBXCONV_LOG_LOG_H