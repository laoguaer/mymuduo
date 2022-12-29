#pragma once

class noncopyable {
public:
	void operator= (const noncopyable&) = delete;
	noncopyable(const noncopyable&) = delete;
protected:
	noncopyable() = default;
	~noncopyable() = default;
};