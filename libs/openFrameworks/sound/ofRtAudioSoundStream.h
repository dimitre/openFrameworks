#pragma once
#include "ofConstants.h"
#if defined(OF_RTAUDIO)
#include "ofSoundBaseTypes.h"
#include "ofSoundBuffer.h"

typedef unsigned int RtAudioStreamStatus;
//class RtAudio;

namespace rt {
namespace audio {
	class RtAudio;
}
}

class ofRtAudioSoundStream final : public ofBaseSoundStream {
public:
	ofRtAudioSoundStream();
	~ofRtAudioSoundStream();

	std::vector<ofSoundDevice> getDeviceList(ofSoundDevice::Api api) const override;

	void setInput(ofBaseSoundInput * soundInput) override;
	void setOutput(ofBaseSoundOutput * soundOutput) override;
	bool setup(const ofSoundStreamSettings & settings) override;

	void start() override;
	void stop() override;
	void close() override;

	uint64_t getTickCount() const override;

	int getNumInputChannels() const override;
	int getNumOutputChannels() const override;
	int getSampleRate() const override;
	int getBufferSize() const override;
	ofSoundDevice getInDevice() const override;
	ofSoundDevice getOutDevice() const override;


private:
	long unsigned long tickCount;
	std::shared_ptr<rt::audio::RtAudio>	audio;

	ofSoundBuffer inputBuffer;
	ofSoundBuffer outputBuffer;
	ofSoundStreamSettings settings;

	static int rtAudioCallback(void *outputBuffer, void *inputBuffer, unsigned int bufferSize, double streamTime, RtAudioStreamStatus status, void *data);

};
#endif
