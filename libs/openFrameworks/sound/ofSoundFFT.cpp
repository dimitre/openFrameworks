// ofSoundFFT.cpp
#include "ofSoundFFT.h"
#include <glm/gtc/constants.hpp>
#include <algorithm>
#include <cmath>
#include <numeric>

ofSoundFFT::ofSoundFFT() = default;

ofSoundFFT::~ofSoundFFT() {
	if (fftCfg) kiss_fft_free(fftCfg);
}

void ofSoundFFT::setup(int bands) {
	if (fftCfg) kiss_fft_free(fftCfg);
	fftSize = bands;
	fftCfg = kiss_fftr_alloc(fftSize, 0, nullptr, nullptr);
	
	windowedSignal.resize(fftSize);
	cx_out.resize(fftSize / 2 + 1);
	spectrum.resize(fftSize / 2 + 1);
	
	window = createWindow(fftSize, 0); // Hann default
	windowSum = computeWindowSum(window);
}

void ofSoundFFT::process(const float* input, int size) {
	if (!fftCfg || size != fftSize) {
		std::fill(spectrum.begin(), spectrum.end(), 0.0f);
		return;
	}
	
	for (int i = 0; i < fftSize; ++i) {
		windowedSignal[i] = input[i] * window[i];
	}
	
	kiss_fftr(fftCfg, windowedSignal.data(), cx_out.data());
	
	const float norm = 1.0f / (fftSize * 0.5f * windowSum);
	for (size_t i = 0; i < spectrum.size(); ++i) {
		spectrum[i] = std::sqrt(cx_out[i].r * cx_out[i].r + cx_out[i].i * cx_out[i].i) * norm;
	}
}

std::vector<float> ofSoundFFT::createWindow(int size, int type) {
	if (size <= 0) return {};
	std::vector<float> w(size);
	const float pi = glm::pi<float>();
	switch (type) {
		case 0: for (int i = 0; i < size; ++i) w[i] = 0.5f * (1.0f - std::cos(2.0f * pi * i / (size - 1))); break;
		case 1: for (int i = 0; i < size; ++i) w[i] = 0.54f - 0.46f * std::cos(2.0f * pi * i / (size - 1)); break;
		case 2: { const float a = 0.16f; for (int i = 0; i < size; ++i) {
			float x = 2.0f * pi * i / (size - 1);
			w[i] = (1.0f - a)/2.0f - 0.5f * std::cos(x) + a/2.0f * std::cos(2.0f * x);
		} break; }
		default: std::fill(w.begin(), w.end(), 1.0f); break;
	}
	return w;
}

void ofSoundFFT::applyWindow(std::vector<float> &signal, const std::vector<float> &window) {
	const size_t n = std::min(signal.size(), window.size());
	for (size_t i = 0; i < n; ++i) signal[i] *= window[i];
}

float ofSoundFFT::computeWindowSum(const std::vector<float>& window) {
	return std::accumulate(window.begin(), window.end(), 0.0f);
}
