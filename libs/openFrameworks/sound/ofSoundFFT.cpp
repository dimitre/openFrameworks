#include "ofSoundFFT.h"
#include <glm/gtc/constants.hpp>

#include <kissfft/kiss_fft.h>
#include <kissfft/kiss_fftr.h>

kiss_fftr_cfg ofSoundFFT::systemFftCfg = nullptr;
std::vector<float> ofSoundFFT::systemWindowedSignal;
std::vector<float> ofSoundFFT::systemBins;
std::vector<kiss_fft_cpx> ofSoundFFT::systemCx_out;
std::vector<float> ofSoundFFT::window;
float ofSoundFFT::windowSum = 0;

ofSoundFFT::ofSoundFFT() {
	
}

ofSoundFFT::~ofSoundFFT() {
	kiss_fftr_free(fftCfg);

}

void ofSoundFFT::initSystemFFT(int bands){
	if(int(systemBins.size())==bands) return;
	int signalSize = (bands-1)*2;
	if(systemFftCfg!=0) kiss_fftr_free(systemFftCfg);
	systemFftCfg = kiss_fftr_alloc(signalSize, 0, nullptr, nullptr);
	systemCx_out.resize(bands);
	systemBins.resize(bands);
	createWindow(signalSize);
}

void ofSoundFFT::initFFT(int bands){
	if(int(bins.size())==bands) return;
	int signalSize = (bands-1)*2;
	if(fftCfg!=0) kiss_fftr_free(fftCfg);
	fftCfg = kiss_fftr_alloc(signalSize, 0, nullptr, nullptr);
	cx_out.resize(bands);
	bins.resize(bands);
	createWindow(signalSize);
}

void ofSoundFFT::createWindow(int size){
	if(int(window.size())!=size){
		windowSum = 0;
		window.resize(size);
		// hanning window
		for(int i = 0; i < size; i++){
			window[i] = .54f - .46f * std::cos((glm::two_pi<float>() * i) / (size - 1));
			windowSum += window[i];
		}
	}
}


void ofSoundFFT::runWindow(std::vector<float> & signal){
	for(int i = 0; i < (int)signal.size(); i++)
		signal[i] *= window[i];
}




float * ofSoundFFT::getSpectrum(int bands){
	initFFT(bands);
	bins.assign(bins.size(),0);
	
	// FIXME: sources are ofOpenALSoundPlayer object
//	if(sources.empty()) return &bins[0];

	int signalSize = (bands-1)*2;
	getCurrentBufferSum(signalSize);

	float normalizer = 2. / windowSum;
	runWindow(windowedSignal);
	kiss_fftr(fftCfg, &windowedSignal[0], &cx_out[0]);
	for(int i= 0; i < bands; i++) {
		bins[i] += sqrtf(cx_out[i].r * cx_out[i].r + cx_out[i].i * cx_out[i].i) * normalizer;
	}
	return &bins[0];
}

//float * ofSoundFFT::getSystemSpectrum(int bands){
//	initSystemFFT(bands);
//	systemBins.assign(systemBins.size(),0);
//	if(players().empty()) return &systemBins[0];
//
//	int signalSize = (bands-1)*2;
//	if(int(systemWindowedSignal.size())!=signalSize){
//		systemWindowedSignal.resize(signalSize);
//	}
//	systemWindowedSignal.assign(systemWindowedSignal.size(),0);
//
//	std::set<ofOpenALSoundPlayer*>::iterator it;
//	for(it=players().begin();it!=players().end();it++){
//		if(!(*it)->isPlaying()) continue;
//		float * buffer = (*it)->getCurrentBufferSum(signalSize);
//		for(int i=0;i<signalSize;i++){
//			systemWindowedSignal[i]+=buffer[i];
//		}
//	}
//
//	float normalizer = 2. / windowSum;
//	runWindow(systemWindowedSignal);
//	kiss_fftr(systemFftCfg, &systemWindowedSignal[0], &systemCx_out[0]);
//	for(int i= 0; i < bands; i++) {
//		systemBins[i] += sqrtf(systemCx_out[i].r * systemCx_out[i].r + systemCx_out[i].i * systemCx_out[i].i) * normalizer;
//	}
//	return &systemBins[0];
//}
