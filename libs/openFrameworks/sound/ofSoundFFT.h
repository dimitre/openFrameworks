#pragma once

#include <kissfft/kiss_fft.h>
#include <kissfft/kiss_fftr.h>
#include <vector>

class ofSoundFFT {
public:
	ofSoundFFT();
	~ofSoundFFT();
	static void initSystemFFT(int bands);

	void initFFT(int bands);
	virtual float * getCurrentBufferSum(int size) {};
	static void createWindow(int size);
	static void runWindow(std::vector<float> & signal);
	
	float * getSpectrum(int bands);

//	static float * getSystemSpectrum(int bands);
	float * getSystemSpectrum(int bands);
	
	// fft structures
private:
	std::vector<std::vector<float> > fftBuffers;
	kiss_fftr_cfg fftCfg;
	std::vector<float> windowedSignal;
	std::vector<float> bins;
	std::vector<kiss_fft_cpx> cx_out;

	static kiss_fftr_cfg systemFftCfg;
	static std::vector<float> systemWindowedSignal;
	static std::vector<float> systemBins;
	static std::vector<kiss_fft_cpx> systemCx_out;
	
	static std::vector<float> window;
	static float windowSum;
};
