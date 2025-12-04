#include "ofTimer.h"
#include "ofUtils.h" //ofGetCurrentTime
#include "ofAppRunner.h" //ofCore
#include "ofAppBaseWindow.h" //ofCore
#include "ofLog.h" 

#include "ofEvents.h"
#include "ofMainLoop.h"

#if defined(TARGET_OF_IOS) || defined(TARGET_OSX) || defined(TARGET_LINUX) || defined(TARGET_EMSCRIPTEN)
	#include <sys/time.h>
#endif

#ifdef TARGET_OSX
	#ifndef TARGET_OF_IOS
		#include <mach-o/dyld.h>
		#include <sys/param.h> // for MAXPATHLEN
	#endif
	#include <mach/clock.h>
	#include <mach/mach.h>
#endif



#define NANOS_PER_SEC 1000000000ll


namespace of {
namespace priv {
//void initutils() {
//	ofResetElapsedTimeCounter();
//	of::random::Engine::construct();
//}


class Clock {
public:
	Clock() {
#ifdef TARGET_OSX
		host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cs);
#endif
	}

	//--------------------------------------
	void setTimeModeSystem() {
		mode = ofTime::System;
		loopListener.unsubscribe();
	}

	//--------------------------------------
	void setTimeModeFixedRate(uint64_t stepNanos, ofMainLoop & mainLoop) {
		fixedRateTime = getMonotonicTimeForMode(ofTime::System);
		mode = ofTime::FixedRate;
		fixedRateStep = stepNanos;
		loopListener = mainLoop.loopEvent.newListener([this] {
			fixedRateTime.nanoseconds += fixedRateStep;
			while (fixedRateTime.nanoseconds > 1000000000) {
				fixedRateTime.nanoseconds -= 1000000000;
				fixedRateTime.seconds += 1;
			}
		});
	}

	//--------------------------------------
	ofTime getCurrentTime() {
		return getMonotonicTimeForMode(mode);
	}

	//--------------------------------------
	std::chrono::nanoseconds getElapsedTime() {
		return getCurrentTime() - startTime;
	}

	//--------------------------------------
	void resetElapsedTimeCounter() {
		startTime = getMonotonicTimeForMode(ofTime::System);
	}

private:
	//--------------------------------------
	ofTime getMonotonicTimeForMode(ofTime::Mode mode) {
		ofTime t;
		t.mode = mode;
		if (mode == ofTime::System) {
#if (defined(TARGET_LINUX) && !defined(TARGET_RASPBERRY_PI_LEGACY)) || defined(TARGET_EMSCRIPTEN)
			struct timespec now;
			clock_gettime(CLOCK_MONOTONIC, &now);
			t.seconds = now.tv_sec;
			t.nanoseconds = now.tv_nsec;
#elif defined(TARGET_OSX)
			mach_timespec_t now;
			clock_get_time(cs, &now);
			t.seconds = now.tv_sec;
			t.nanoseconds = now.tv_nsec;
#elif defined(TARGET_WIN32)
			LARGE_INTEGER freq;
			LARGE_INTEGER counter;
			QueryPerformanceFrequency(&freq);
			QueryPerformanceCounter(&counter);
			t.seconds = counter.QuadPart / freq.QuadPart;
			t.nanoseconds = (counter.QuadPart % freq.QuadPart) * 1000000000 / freq.QuadPart;
#else
			struct timeval now;
			gettimeofday(&now, nullptr);
			t.seconds = now.tv_sec;
			t.nanoseconds = now.tv_usec * 1000;
#endif
		} else {
			t = fixedRateTime;
		}
		return t;
	}
	uint64_t fixedRateStep = 1666667;
	ofTime fixedRateTime;
	ofTime startTime;
	ofTime::Mode mode = ofTime::System;
	ofEventListener loopListener;
#ifdef TARGET_OSX
	clock_serv_t cs;
#endif
};

Clock & getClock() {
	static Clock * clock = new Clock;
	return *clock;
}
}
}



//--------------------------------------
ofTime ofGetCurrentTime() {
	return of::priv::getClock().getCurrentTime();
}

void ofGetMonotonicTime(uint64_t & seconds, uint64_t & nanoseconds);

ofTimer::ofTimer()
:nanosPerPeriod(0)
#ifdef TARGET_WIN32
,hTimer(CreateWaitableTimer(nullptr, TRUE, nullptr))
#endif
{

}

void ofTimer::reset(){
#if defined(TARGET_WIN32)
	GetSystemTimeAsFileTime((LPFILETIME)&nextWakeTime);
#else
	nextWakeTime = ofGetCurrentTime();
#endif
	calculateNextPeriod();
}

void ofTimer::setPeriodicEvent(uint64_t nanoseconds){
	nanosPerPeriod = std::chrono::nanoseconds(nanoseconds);
	reset();
}

void ofTimer::waitNext(){
#if (defined(TARGET_LINUX) && !defined(TARGET_RASPBERRY_PI_LEGACY))
	timespec remainder = {0,0};
	timespec wakeTime = nextWakeTime.getAsTimespec();
	clock_nanosleep(CLOCK_MONOTONIC,TIMER_ABSTIME,&wakeTime,&remainder);
#elif defined(TARGET_WIN32)
	WaitForSingleObject(hTimer, INFINITE);
#else
	auto now = ofGetCurrentTime();
	if(now < nextWakeTime){
		auto waitNanos = nextWakeTime - now;
		timespec waittime = (ofTime() + waitNanos).getAsTimespec();
		timespec remainder = {0,0};
		nanosleep(&waittime, &remainder);
	}
#endif
	calculateNextPeriod();
}


void ofTimer::calculateNextPeriod(){
#if defined(TARGET_WIN32)
	nextWakeTime.QuadPart += nanosPerPeriod.count()/100;
    LARGE_INTEGER now;
    GetSystemTimeAsFileTime((LPFILETIME)&now);
	if(nextWakeTime.QuadPart<now.QuadPart){
	    reset();
	}else{
	    SetWaitableTimer(hTimer, &nextWakeTime, 0, nullptr, nullptr, 0);
	}
#else
	nextWakeTime += nanosPerPeriod;
	auto now = ofGetCurrentTime();
    if(nextWakeTime<now){
        reset();
    }
#endif
}






//--------------------------------------
uint64_t ofGetFixedStepForFps(double fps) {
	return 1'000'000'000 / fps;
}

ofTimeMode ofGetTimeMode() {
	if (auto window = ofCore.mainLoop.getCurrentWindow()) {
		return window->events().getTimeMode();
	}
//	if (auto mainLoop = ofGetMainLoop()) {
//		if (auto window = mainLoop->getCurrentWindow()) {
//			return window->events().getTimeMode();
//		}
//	}
	return ofTimeMode(0);
}

//--------------------------------------
void ofSetTimeModeSystem() {
//	auto mainLoop = ofGetMainLoop();
//	if (!mainLoop) {
//		ofLogError("ofSetSystemTimeMode") << "ofMainLoop is not initialized yet, can't set time mode";
//		return;
//	}
//	auto window = mainLoop->getCurrentWindow();
//	if (!window) {
//		ofLogError("ofSetSystemTimeMode") << "No window setup yet can't set time mode";
//		return;
//	}
	if (auto window = ofCore.mainLoop.getCurrentWindow()) {
		window->events().setTimeModeSystem();
		of::priv::getClock().setTimeModeSystem();
	} else {
		ofLogError("ofSetSystemTimeMode") << "No window setup yet can't set time mode";
	}
}

//--------------------------------------
void ofSetTimeModeFixedRate(uint64_t stepNanos) {
//	auto mainLoop = ofGetMainLoop();
//	if (!mainLoop) {
//		ofLogError("ofSetSystemTimeMode") << "ofMainLoop is not initialized yet, can't set time mode";
//		return;
//	}
//	auto window = mainLoop->getCurrentWindow();
//	if (!window) {
//		ofLogError("ofSetSystemTimeMode") << "No window setup yet can't set time mode";
//		return;
//	}
	if (auto window = ofCore.mainLoop.getCurrentWindow()) {
		window->events().setTimeModeFixedRate(stepNanos);
//		of::priv::getClock().setTimeModeFixedRate(stepNanos, *mainLoop);
		of::priv::getClock().setTimeModeFixedRate(stepNanos, ofCore.mainLoop);
	}
}

//--------------------------------------
void ofSetTimeModeFiltered(float alpha) {
//	auto mainLoop = ofGetMainLoop();
//	if (!mainLoop) {
//		ofLogError("ofSetSystemTimeMode") << "ofMainLoop is not initialized yet, can't set time mode";
//		return;
//	}
//	auto window = mainLoop->getCurrentWindow();
//	if (!window) {
//		ofLogError("ofSetSystemTimeMode") << "No window setup yet can't set time mode";
//		return;
//	}
	if (auto window = ofCore.mainLoop.getCurrentWindow()) {

		window->events().setTimeModeFiltered(alpha);
		of::priv::getClock().setTimeModeSystem();
	}
}

//--------------------------------------
void ofResetElapsedTimeCounter() {
	ofCore.clock.resetElapsedTimeCounter();
	of::priv::getClock().resetElapsedTimeCounter();
}

////--------------------------------------
//uint64_t ofGetSystemTime() {
//	return of::priv::getClock().getCurrentTime().getAsMilliseconds();
//}

//--------------------------------------
uint64_t ofGetSystemTimeMillis() {
	return of::priv::getClock().getCurrentTime().getAsMilliseconds();
}

//--------------------------------------
uint64_t ofGetSystemTimeMicros() {
	return of::priv::getClock().getCurrentTime().getAsMicroseconds();
}

//--------------------------------------------------
uint64_t ofGetUnixTime() {
	return static_cast<uint64_t>(time(nullptr));
}

uint64_t ofGetUnixTimeMillis() {
	auto elapsed = std::chrono::system_clock::now().time_since_epoch();
	return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
}




