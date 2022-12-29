#pragma once
#include <iostream>

class Timestamp {
public:
	Timestamp(time_t SecondSinceEpoch = 0);
	static Timestamp now();
	std::string toString() const;
private:
	time_t SecondSinceEpoch_;
};