#pragma once

#include <string>

#include "noncopyable.h"

#define MUDUO_DEBUG 0

#define LOG_INFO(LogmsgFormat, ...) \
	do { \
		Logger &logger = Logger::instance(); \
		logger.setLevel(INFO); \
		char buf[1024] = {0}; \
		snprintf(buf, 1024, LogmsgFormat, ##__VA_ARGS__); \
		logger.log(buf); \
	} while(0)
#define LOG_ERROR(LogmsgFormat, ...) \
	do { \
		Logger &logger = Logger::instance(); \
		logger.setLevel(ERROR); \
		char buf[1024] = {0}; \
		snprintf(buf, 1024, LogmsgFormat, ##__VA_ARGS__); \
		logger.log(buf); \
	} while(0)
#define LOG_FATAL(LogmsgFormat, ...) \
	do { \
		Logger &logger = Logger::instance(); \
		logger.setLevel(FATAL); \
		char buf[1024] = {0}; \
		snprintf(buf, 1024, LogmsgFormat, ##__VA_ARGS__); \
		logger.log(buf); \
		exit(-1); \
	} while(0)
#ifdef MUDUO_DEBUG
	#define LOG_DEBUG(LogmsgFormat, ...) \
		do { \
			Logger &logger = Logger::instance(); \
			logger.setLevel(DEBUG); \
			char buf[1024] = {0}; \
			snprintf(buf, 1024, LogmsgFormat, ##__VA_ARGS__); \
			logger.log(buf); \
		} while(0)
#else
	#define LOG_DEBUG(LogmsgFormat, ...)
#endif
enum LogLevel {
	INFO,
	ERROR,
	FATAL,
	DEBUG,
};

class Logger : noncopyable {
public:
	static Logger& instance();
		void setLevel(int level);
	void log(std::string msg);
private:
	Logger();

	int logLevel_;
};