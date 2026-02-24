#pragma once

#include "ofAppBaseWindow.h"
#include "ofEvents.h"

class ofBaseApp;
class ofBaseRenderer;

class ofAppNoWindow final : public ofAppBaseWindow {

public:
	ofAppNoWindow() noexcept;
	~ofAppNoWindow() noexcept override = default;

	[[nodiscard]] static bool doesLoop() noexcept { return false; }
	[[nodiscard]] static bool allowsMultiWindow() noexcept { return false; }
	static void loop() noexcept {}
	[[nodiscard]] static bool needsPolling() noexcept { return false; }
	static void pollEvents() noexcept {}

	void run(ofBaseApp * appPtr);

	static void exitApp();
	void setup(const ofWindowSettings & settings) override;
	void update() override;
	void draw() override;

	[[nodiscard]] glm::ivec2 getWindowPosition() const override;
	[[nodiscard]] glm::ivec2 getWindowSize() const override;
	[[nodiscard]] glm::ivec2 getScreenSize() const override;

	[[nodiscard]] int getWidth() const noexcept override;
	[[nodiscard]] int getHeight() const noexcept override;

	ofCoreEvents & events() override;
	[[nodiscard]] std::shared_ptr<ofBaseRenderer> & renderer() noexcept override;

private:
	int width = 0;
	int height = 0;

    ofBaseApp * ofAppPtr = nullptr;
	std::unique_ptr<ofCoreEvents> coreEvents;
    std::shared_ptr<ofBaseRenderer> currentRenderer;
};
