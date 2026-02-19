//
//  ofAVEngineSoundPlayer.hpp
//  soundPlayerExample
//
//  Created by Theo Watson on 3/24/21.
//

#pragma once

// MARK: Review later
#include "ofConstants.h"

#ifdef OF_SOUND_PLAYER_AV_ENGINE

#include "ofSoundBaseTypes.h"
#include <mutex>
#include <vector>
class ofEventArgs;

// FIXME: some can be moved to .mm
#ifdef __OBJC__
	#import <Foundation/Foundation.h>
	#import <AVFoundation/AVFoundation.h>
	#import <Accelerate/Accelerate.h>
#endif

#include "ofSoundFFT.h"

class ofAVEngineSoundPlayer final : public ofBaseSoundPlayer {
public:

//thanks to @bangnoise for this trick
#ifdef __OBJC__
	using ObjectType = id<NSObject>;
#else
	using ObjectType = void *;
#endif

	ofAVEngineSoundPlayer();
	~ofAVEngineSoundPlayer();

	const std::vector<float>& getSpectrum(int bands) const; // per-player
	void installFFTOnMixer(); // called after engine starts
	void installSystemFFTOnPlayerMixer(); // temp: for testing
	
	// System-wide spectrum analysis
	static float* getSystemSpectrum(int bands);
	static void installSystemFFT();
	static void removeSystemFFT();
	
	bool load(const fs::path& fileName, bool stream = false) override;
	void unload() override;
	void play() override;
	void stop() override;

	void setVolume(float vol) override;
	void setPan(float vol) override;
	void setSpeed(float spd) override;
	void setPaused(bool bP) override;
	void setLoop(bool bLp) override;
	void setMultiPlay(bool bMp) override;
	void setPosition(float pct) override;
	void setPositionMS(int ms) override;

	float getPosition() const override;
	int getPositionMS() const override;
	bool isPlaying() const override;
	bool isPaused() const override;
	float getSpeed() const override;
	float getPan() const override;
	bool isLoaded() const override;
	float getVolume() const override;

	float getDuration() const override;
	unsigned int getDurationMS() const override;
	
	bool getCurrentBuffer(std::vector<float>& buffer) override;

	void * getAVEnginePlayer();

private:
	ofSoundFFT fft;
	
	// Current audio buffer storage for external access
	std::vector<float> currentBuffer;
	std::mutex bufferMutex;
	bool bufferTapInstalled = false;
	
	// Static members for system-wide FFT
	static ofSoundFFT systemFFT;
	static std::vector<float> systemBins;
	static bool systemFFTInstalled;
	
protected:

	void updateFunction(ofEventArgs & args);
	bool bAddedUpdate = false;
	bool bPaused = false;

	void cleanupMultiplayers();
	static bool removeMultiPlayer(void * aPlayer);
	ObjectType soundPlayer;
	std::vector <ObjectType> mMultiplayerSoundPlayers;

};

#endif
