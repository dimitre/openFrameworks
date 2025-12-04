#include "ofFpsCounter.h"
#include <ofUtils.h>
using namespace std::chrono;

ofFpsCounter::ofFpsCounter()
	: lastFrameTime(std::chrono::duration<long long, std::nano>(0))
	, diff(std::chrono::duration<long long, std::nano>(0))
	, then(std::chrono::steady_clock::now())
	, timeMode(0) {
//		timestamps.clear();
//		timestamps.resize(targetFPS + 7);
		timestamps.clear();
	}

ofFpsCounter::ofFpsCounter(double targetFPS, int mode)
	: targetFPS(targetFPS)
	, lastFrameTime(std::chrono::duration<long long, std::nano>(0))
	, diff(std::chrono::duration<long long, std::nano>(0))
	, then(std::chrono::steady_clock::now())
	, timeMode(mode) {
		timestamps.clear();
//		timestamps.resize(targetFPS + 7);
	}

void ofFpsCounter::newFrame(){
	now = steady_clock::now();
	update(now);
	timestamps.push_back(now);
	lastFrameTime = now - then;
	// std::lerp from c++20 on
	if (timeMode == 2) { // Filtered
		filterAlpha = std::clamp(filterAlpha, 0.0, 1.0);
		filteredTime = filteredTime * filterAlpha + getLastFrameSecs() * (1.0 - filterAlpha);
		filteredTime = std::clamp(filteredTime, 0.0, 1.0 / targetFPS);
	}
	then = now;
	nFrameCount++;
}

void ofFpsCounter::update(){
	now = steady_clock::now();
	update(now);
}

void ofFpsCounter::update(time_point<steady_clock> now) {
	while (!timestamps.empty() && timestamps.front() < now - 2s)
		timestamps.pop_front();

	if (timestamps.size() < static_cast<size_t>(targetFPS)) {   // <--- real seconds
		fps = targetFPS;
		return;
	}

	int64_t diffNs { duration_cast<nanoseconds>(now - timestamps.front()).count() };
	fps = 1'000'000'000.0 * timestamps.size() / diffNs;
}


//void ofFpsCounter::update(time_point<steady_clock> now){
//	while(!timestamps.empty() && timestamps.front() + 2s < now){
//		timestamps.pop_front();
//	}
//	if (timestamps.size() < 2) {
//		fps = targetFPS; // if no sample size then set fps to target until can sample
//		return;
//	}
//	diff = now - timestamps.front();		
//	if (diff > std::chrono::duration<double>(0)) {
//		fps = static_cast<double>(timestamps.size()) / std::chrono::duration<double>(diff).count();
//	} else {
//		fps = timestamps.size();
//	}
//	
//}

double ofFpsCounter::getFps() const{
	return fps;
}

double ofFpsCounter::getFpsRounded(int decimals) const
{
	double scale = std::pow(10.0, decimals);
	return std::round(fps * scale) / scale;
}

uint64_t ofFpsCounter::getNumFrames() const{
	return nFrameCount;
}

uint64_t ofFpsCounter::getLastFrameNanos() const{
	return duration_cast<nanoseconds>(lastFrameTime).count();
}

double ofFpsCounter::getLastFrameSecs() const{
	return std::chrono::duration<double>(lastFrameTime).count();
}

uint64_t ofFpsCounter::getLastFrameFilteredNanos() const{
	return duration_cast<nanoseconds>(lastFrameTime).count();
}

double ofFpsCounter::getLastFrameFilteredSecs() const{
	return std::chrono::duration<double>(filteredTime).count();
}

void ofFpsCounter::setFilterAlpha(float alpha){
	filterAlpha = alpha;
}

void ofFpsCounter::setTimeMode(int mode) {
	timeMode = mode;
}

//void ofFpsCounter::setTargetFPS(double fps) {
//	targetFPS = fps;
//	if (fps > timestamps.max_size()) {
//		timestamps.resize(fps);
//	}
//}

void ofFpsCounter::setTargetFPS(double fps)
{
	targetFPS = fps;
	timestamps.clear();
	nFrameCount = 0;
}
