#pragma once

// MARK: this is to define the constant OF_SOUND_PLAYER_FMOD
#include "ofConstants.h"

#ifdef OF_SOUND_PLAYER_FMOD

#include "ofSoundBaseTypes.h"


extern "C" {
#include <fmod.h>
#include <fmod_errors.h>
}

//		TO DO :
//		---------------------------
// 		-fft via fmod, as in the last time...
// 		-close fmod if it's up
//		-loadSoundForStreaming(char * fileName);
//		---------------------------

// 		interesting:
//		http://www.compuphase.com/mp3/mp3loops.htm


// ---------------------------------------------------------------------------- SOUND SYSTEM FMOD

// --------------------- global functions:
void ofFmodSoundStopAll();
void ofFmodSoundSetVolume(float vol);
void ofFmodSoundUpdate();						// calls FMOD update.
float * ofFmodSoundGetSpectrum(int nBands);		// max 512...
void ofFmodSetBuffersize(unsigned int bs);


// --------------------- player functions:
class ofFmodSoundPlayer final : public ofBaseSoundPlayer {

	public:

		ofFmodSoundPlayer();
		~ofFmodSoundPlayer();

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
		void setPosition(float pct) override; // 0 = start, 1 = end;
		void setPositionMS(int ms) override;

		float getPosition() const override;
		int getPositionMS() const override;
		bool isPlaying() const override;
		bool isPaused() const override;
		float getSpeed() const override;
		float getPan() const override;
		float getVolume() const override;
		bool isLoaded() const override;

		float getDuration() const override;
		unsigned int getDurationMS() const override;

		static void initializeFmod();
		static void closeFmod();


		bool isStreaming;
		bool bMultiPlay;
		bool bLoop;
		bool bLoadedOk;
		bool bPaused;
		float pan; // -1 to 1
		float volume; // 0 - 1
		float internalFreq; // 44100 ?
		float speed; // -n to n, 1 = normal, -1 backwards
		unsigned int length; // in samples;
		unsigned int durationMS; // duration in milliseconds

		FMOD_RESULT result;
		FMOD_CHANNEL * channel = NULL;
		FMOD_SOUND * sound = NULL;
};

#endif //OF_SOUND_PLAYER_FMOD
