#include <cstdint>
#include <chrono>

struct ofTime {
	uint64_t seconds = 0;
	uint64_t nanoseconds = 0;

	enum Mode {
		System,
		FixedRate,
	} mode
		= System;

	uint64_t getAsMilliseconds() const;
	uint64_t getAsMicroseconds() const;
	uint64_t getAsNanoseconds() const;
	double getAsSeconds() const;
#ifndef TARGET_WIN32
	timespec getAsTimespec() const;
#endif

	std::chrono::time_point<std::chrono::nanoseconds> getAsTimePoint() const;
	std::chrono::nanoseconds operator-(const ofTime &) const;
	bool operator<(const ofTime &) const;
	bool operator>(const ofTime &) const;
	bool operator<=(const ofTime &) const;
	bool operator>=(const ofTime &) const;

	template <typename rep, typename ratio>
	ofTime operator+(const std::chrono::duration<rep, ratio> & duration) const {
		constexpr uint64_t NANOS_PER_SEC = 1000000000ll;
		auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
		ofTime t = *this;
		t.nanoseconds += ns.count();
		if (t.nanoseconds >= NANOS_PER_SEC) {
			uint64_t secs = t.nanoseconds / NANOS_PER_SEC;
			t.nanoseconds -= NANOS_PER_SEC * secs;
			t.seconds += secs;
		}
		return t;
	}

	template <typename rep, typename ratio>
	ofTime & operator+=(const std::chrono::duration<rep, ratio> & duration) {
		constexpr uint64_t NANOS_PER_SEC = 1000000000ll;
		auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
		this->nanoseconds += ns.count();
		if (this->nanoseconds >= NANOS_PER_SEC) {
			uint64_t secs = this->nanoseconds / NANOS_PER_SEC;
			this->nanoseconds -= NANOS_PER_SEC * secs;
			this->seconds += secs;
		}
		return *this;
	}
};
