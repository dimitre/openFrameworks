#include "ofTimerFps.h"
#include <thread>

using namespace std::chrono;
using namespace std::chrono_literals;

void ofTimerFps::reset() {
   wakeTime = steady_clock::now();
}

ofTimerFps::ofTimerFps() {
	reset();
	//interval = duration_cast<microseconds>(1s) / currentFPS;
	interval = 1s / currentFPS;
};

void ofTimerFps::setFps(int fps) {
	if (fps <= 0) {
		fps = 60; // fallback
	}
//	interval = std::ratio<1s, fps>;
	currentFPS = fps;

	interval = space(1'000'000'000LL) / fps;   // nanoseconds per frame
//	interval = duration_cast<microseconds>(1s) / currentFPS;
}


//void ofTimerFps::waitNext() {
//   // Lazy wakeup
//   std::this_thread::sleep_until(wakeTime - 36ms); //4ms
////	std::this_thread::sleep_until(wakeTime - 2ms); //4ms
//
//   // Processor Coffee
//   while(steady_clock::now() < (wakeTime)) { // 0.05ms 0.5us // - 0.5us  - 1ns
//	   std::this_thread::yield();
////	   std::this_thread::sleep_for(5us);
//   }
//
//   lastWakeTime = wakeTime;
//   wakeTime += interval;
//}


void ofTimerFps::waitNext() {
	auto now = steady_clock::now();

	auto sleepUntil = wakeTime - 3ms;
	if (sleepUntil > now) {
		std::this_thread::sleep_until(sleepUntil);
	}

	while (steady_clock::now() < wakeTime) {
		std::this_thread::yield();
	}

	lastWakeTime = wakeTime;
	wakeTime += interval;
}
