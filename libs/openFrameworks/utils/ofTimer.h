#pragma once

#include "ofTime.h"
#include "ofEvents.h"

/// \brief Get the system time.
/// \returns the system time, which is the time since the system booted (uptime).
ofTime ofGetCurrentTime();

uint64_t ofGetFixedStepForFps(double fps);

ofTimeMode ofGetTimeMode();

void ofSetTimeModeSystem();

void ofSetTimeModeFixedRate(uint64_t stepNanos = ofGetFixedStepForFps(60)); //default nanos for 1 frame at 60fps
void ofSetTimeModeFiltered(float alpha = 0.9);


/// \section Elapsed Time
/// \brief Reset the elapsed time counter.
///
/// This method resets the times returned by ofGetElapsedTimef(),
/// ofGetElapsedTimeMillis() and ofGetElapsedTimeMicros() to zero.
///
/// \note This is called on the first frame during app startup.
void ofResetElapsedTimeCounter();

/// \brief Get the system time in milliseconds (system uptime).
/// \returns the system time in milliseconds.
//[[deprecated("Use ofGetSystemTimeMillis()")]]
//uint64_t ofGetSystemTime();

/// \brief Get the system time in milliseconds (system uptime).
/// \returns the system time in milliseconds.
uint64_t ofGetSystemTimeMillis();

/// \brief Get the system time in microseconds (system uptime).
/// \returns the system time in microseconds.
uint64_t ofGetSystemTimeMicros();

/// \brief Get the number of seconds since Midnight, January 1, 1970.
///
/// Resolution is in seconds.
///
/// \returns the number of seconds since Midnight, January 1, 1970 (epoch time).
uint64_t ofGetUnixTime();

/// \brief Get the Unix Time in milliseconds.
///
/// This returns the milliseconds since Midnight, January 1, 1970.
///
/// \returns the milliseconds since Midnight, January 1, 1970.
uint64_t ofGetUnixTimeMillis();

class ofTimer {
public:
	
	/// \brief Constructs the default ofTimer().
	ofTimer();

	/// \brief  Reset the starting instant of the periodic event to now.
	void reset();
	
	/// \brief Sets the frequency in \p nanoseconds that events happen.
	/// 
	/// The timer will begin calculating the events beginning the moment
	/// the function is called.
	/// \param nanoseconds The number of nanoseconds between events.
	void setPeriodicEvent(uint64_t nanoseconds);
	
	/// \brief Sleep this thread until the next periodic event.
	void waitNext();
private:
	void calculateNextPeriod();
	std::chrono::nanoseconds nanosPerPeriod;
#if defined(TARGET_WIN32)
	LARGE_INTEGER nextWakeTime;
	HANDLE hTimer;
#else
	ofTime nextWakeTime;
#endif
};

