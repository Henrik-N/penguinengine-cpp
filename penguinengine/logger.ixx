module;
#include "defines.h"
#include <format>
#include <string>

#include <cstdio>

#include <source_location>
#include <iostream>

export module logger;

import platform;

// todo add line & filename to some log types
#define SOURCE_LOC_PARAM std::source_location& sourceLoc = std::source_location::current()

export {

	template <class... Args>
	void FATAL(const stringv fmt, const Args&... args);

	template <class... Args>
	void ERROR(const stringv fmt, const Args&... args) OK;

	template <class... Args>
	void WARNING(const stringv fmt, const Args&... args) OK;

	template <class... Args>
	void INFO(const stringv fmt, const Args&... args) OK;

	template <class... Args>
	void DEBUG(const stringv fmt, const Args&... args) OK;

	template <class... Args>
	void TRACE(const stringv fmt, const Args&... args) OK;
}



enum LogLevel : u8 {
    Fatal = 0,
    Error = 1,
    Warning = 2,
    Info = 3,
    Debug = 4,
    Trace = 5,
};

/// Set the level for the logger to use here
constexpr LogLevel currentLogLevel = LogLevel::Trace;


stringv asText(LogLevel level) OK {
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


void output(const LogLevel level, const stringv msgTag, const stringv msg) OK {
    if (level >= Error) {
        platform::writeConsoleError(msgTag, msg, level);
    }
    else {
        platform::writeConsoleMessage(msgTag, msg, level);
    }
}



template <class... Args>
void FATAL(const stringv fmt, const Args&... args) {
    const std::string msg = std::format(fmt, args...);
    stringv tag = asText(Fatal);
    output(Fatal, tag, msg);
    throw new std::runtime_error(std::format("{} {}", tag, msg)); // also throw on fatal
}

template <class... Args>
void ERROR(const stringv fmt, const Args&... args) OK {
    if constexpr (currentLogLevel >= Error) {
        const std::string msg = std::format(fmt, args...);
        stringv tag = asText(Error);
        output(Error, tag, msg);
    }
}

template <class... Args>
void WARNING(const stringv fmt, const Args&... args) OK {
    if constexpr (currentLogLevel >= Warning) {
        const std::string msg = std::format(fmt, args...);
        stringv tag = asText(Warning);
        output(Warning, tag, msg);
    }
}

template <class... Args>
void INFO(const stringv fmt, const Args&... args) OK {
    if constexpr (currentLogLevel >= Info) {
        const std::string msg = std::format(fmt, args...);
        const stringv tag = asText(Info);
        output(Info, tag, msg);
    }
}

template <class... Args>
void DEBUG(const stringv fmt, const Args&... args) OK {
    if constexpr (currentLogLevel >= Debug) {
        const std::string msg = std::format(fmt, args...);
        const stringv tag = asText(Debug);
        output(Debug, tag, msg);
    }
}

template <class... Args>
void TRACE(const stringv fmt, const Args&... args) OK {
    if constexpr (currentLogLevel >= Trace) {
        const std::string msg = std::format(fmt, args...);
        const stringv tag = asText(Trace);
        output(Trace, tag, msg);
    }
}

