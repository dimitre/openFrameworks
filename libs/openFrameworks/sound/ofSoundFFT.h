#pragma once

#include <kissfft/kiss_fft.h>
#include <kissfft/kiss_fftr.h>
#include <vector>

class ofSoundFFT {
public:
	ofSoundFFT();
	~ofSoundFFT();

	void setup(int bands);
	void process(const float* input, int size);
	const std::vector<float>& getSpectrum() const { return spectrum; }
	
	static std::vector<float> createWindow(int size, int type = 0);
	static void applyWindow(std::vector<float> &signal, const std::vector<float> &window);

private:
	kiss_fftr_cfg fftCfg = nullptr;
	int fftSize = 0;
	std::vector<float> windowedSignal;
	std::vector<kiss_fft_cpx> cx_out;
	std::vector<float> spectrum;
	std::vector<float> window;
	float windowSum = 0.0f;
	
	float computeWindowSum(const std::vector<float>& window);
};
