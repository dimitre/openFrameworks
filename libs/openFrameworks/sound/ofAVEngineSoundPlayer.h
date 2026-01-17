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
class ofEventArgs;

// FIXME: some can be moved to .mm
#ifdef __OBJC__
	#import <Foundation/Foundation.h>
	#import <AVFoundation/AVFoundation.h>
	#import <Accelerate/Accelerate.h>
#endif

#include "ofSoundFFT.h"

class ofAVEngineSoundPlayer : public ofBaseSoundPlayer {
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
	
	bool load(const fs::path& fileName, bool stream = false);
	void unload();
	void play();
	void stop();

	void setVolume(float vol);
	void setPan(float vol);
	void setSpeed(float spd);
	void setPaused(bool bP);
	void setLoop(bool bLp);
	void setMultiPlay(bool bMp);
	void setPosition(float pct);
	void setPositionMS(int ms);

	float getPosition() const;
	int getPositionMS() const;
	bool isPlaying() const;
	float getSpeed() const;
	float getPan() const;
	bool isLoaded() const;
	float getVolume() const;

	float getDuration() const;
	unsigned int getDurationMS() const;

	void * getAVEnginePlayer();

private:
	ofSoundFFT fft;
	
	// Static members for system-wide FFT
	static ofSoundFFT systemFFT;
	static std::vector<float> systemBins;
	static bool systemFFTInstalled;
	
protected:

	void updateFunction(ofEventArgs & args);
	bool bAddedUpdate = false;

	void cleanupMultiplayers();
	static bool removeMultiPlayer(void * aPlayer);
	ObjectType soundPlayer;
	std::vector <ObjectType> mMultiplayerSoundPlayers;

};

#endif
