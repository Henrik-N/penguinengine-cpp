module;
#include "macros.h"
#include <format>
#include <string>

#include <cstdio>
export module logger;

import penguin_t;
import platform;

namespace logger {
	export enum LogLevel : u8 {
		Fatal,
		Error,
		Warning,
		Info,
		Debug,
		Trace,
	};

	stringv asText(LogLevel level) {
		switch (level) {
			using enum LogLevel;
		case Fatal: return "[FATAL]";
		case Error: return "[ERROR]";
		case Warning: return "[WARNING]";
		case Info: return "[INFO]";
		case Debug: return "[DEBUG]";
		default: return "[TRACE]";
		}
	}

	bool isError(LogLevel level) {
		switch (level) {
			using enum LogLevel;
		case Fatal:
		case Error:
			return true;
		default:
			return false;
		}
	}
	
	export void output(LogLevel level, const stringv msg_tag, const stringv msg) {
		const auto error_tag = asText(level);

		
		if (isError(level)) {
			platform::writeConsoleError(msg_tag, msg, level);
		}
		else {
			platform::writeConsoleMessage(msg_tag, msg, level);
		}
	}
}

using namespace logger;

export {
	template <class... _types>
	void lfatal(const stringv fmt, const _types&... args) {
		const std::string msg = std::format(fmt, args...);
		stringv msg_tag = asText(Fatal);

		output(Fatal, msg_tag, msg);
	}

	template <class... _types>
	void lerror(const stringv fmt, const _types&... args) {
		const std::string msg = std::format(fmt, args...);
		stringv msg_tag = asText(Error);

		output(Error, msg_tag, msg);
	}

	template <class... _types>
	void lwarning(const stringv fmt, const _types&... args) {
		const std::string msg = std::format(fmt, args...);
		stringv msg_tag = asText(Warning);

		output(Warning, msg_tag, msg);
	}

	template <class... _types>
	void linfo(const stringv fmt, const _types&... args) {
		const std::string msg = std::format(fmt, args...);
		const stringv msg_tag = asText(Info);

		output(Info, msg_tag, msg);
	}

	template <class... _types>
	void ldebug(const stringv fmt, const _types&... args) {
		const std::string msg = std::format(fmt, args...);
		stringv msg_tag = asText(Debug);

		output(Debug, msg_tag, msg);
	}

	template <class... _types>
	void ltrace(const stringv fmt, const _types&... args) {
		const std::string msg = std::format(fmt, args...);
		stringv msg_tag = asText(Trace);

		output(Trace, msg_tag, msg);
	}
}

