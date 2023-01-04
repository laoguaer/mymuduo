#include <iostream>
#include <mutex>

#include "Timestamp.h"
#include "logger.h"

std::mutex mtx;

Logger& Logger::instance() {
	static Logger logger;
	return logger;
}
void Logger::setLevel(int level) {
	logLevel_ = level;
}
void Logger::log(std::string msg) {
	mtx.lock();
	switch (logLevel_)
	{
	case INFO:
		std::cout << "[INFO]";
		break;
	case ERROR:
		std::cout << "[ERROR";
		break;
	case FATAL:
		std::cout << "[FATAL]";
		break;
	case DEBUG:
		std::cout << "[DEBUF]";
		break;				
	default:
		break;
	}
	std::cout << Timestamp::now().toString() << ": " << msg << std::endl;
	mtx.unlock();
}