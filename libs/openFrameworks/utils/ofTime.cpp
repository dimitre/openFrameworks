#include "ofTime.h"


//--------------------------------------
uint64_t ofTime::getAsMilliseconds() const {
	auto seconds = std::chrono::seconds(this->seconds);
	auto nanoseconds = std::chrono::nanoseconds(this->nanoseconds);
	return (std::chrono::duration_cast<std::chrono::milliseconds>(seconds) + std::chrono::duration_cast<std::chrono::milliseconds>(nanoseconds)).count();
}

//--------------------------------------
uint64_t ofTime::getAsMicroseconds() const {
	auto seconds = std::chrono::seconds(this->seconds);
	auto nanoseconds = std::chrono::nanoseconds(this->nanoseconds);
	return (std::chrono::duration_cast<std::chrono::microseconds>(seconds) + std::chrono::duration_cast<std::chrono::microseconds>(nanoseconds)).count();
}

//--------------------------------------
uint64_t ofTime::getAsNanoseconds() const {
	auto seconds = std::chrono::seconds(this->seconds);
	auto nanoseconds = std::chrono::nanoseconds(this->nanoseconds);
	return (std::chrono::duration_cast<std::chrono::nanoseconds>(seconds) + nanoseconds).count();
}

//--------------------------------------
double ofTime::getAsSeconds() const {
	return seconds + nanoseconds / 1000000000.;
}

#ifndef TARGET_WIN32
timespec ofTime::getAsTimespec() const {
	timespec ret;
	ret.tv_sec = seconds;
	ret.tv_nsec = nanoseconds;
	return ret;
}
#endif

//--------------------------------------
std::chrono::time_point<std::chrono::nanoseconds> ofTime::getAsTimePoint() const {
	auto seconds = std::chrono::seconds(this->seconds);
	auto nanoseconds = std::chrono::nanoseconds(this->nanoseconds);
	return std::chrono::time_point<std::chrono::nanoseconds>(
		std::chrono::duration_cast<std::chrono::nanoseconds>(seconds) + nanoseconds);
}

//--------------------------------------
std::chrono::nanoseconds ofTime::operator-(const ofTime & other) const {
	auto seconds = std::chrono::seconds(this->seconds) - std::chrono::seconds(other.seconds);
	auto nanoseconds = std::chrono::nanoseconds(this->nanoseconds) - std::chrono::nanoseconds(other.nanoseconds);
	return std::chrono::duration_cast<std::chrono::nanoseconds>(seconds) + nanoseconds;
}

//--------------------------------------
bool ofTime::operator<(const ofTime & other) const {
	return seconds < other.seconds || (seconds == other.seconds && nanoseconds < other.nanoseconds);
}

//--------------------------------------
bool ofTime::operator>(const ofTime & other) const {
	return seconds > other.seconds || (seconds == other.seconds && nanoseconds > other.nanoseconds);
}

//--------------------------------------
bool ofTime::operator<=(const ofTime & other) const {
	return seconds <= other.seconds || (seconds == other.seconds && nanoseconds <= other.nanoseconds);
}

//--------------------------------------
bool ofTime::operator>=(const ofTime & other) const {
	return seconds >= other.seconds || (seconds == other.seconds && nanoseconds >= other.nanoseconds);
}
