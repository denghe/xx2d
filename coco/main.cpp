#include "pch.h"
#include "AppDelegate.h"

int main() {
    AppDelegate ad;
    ad.setTitle("HelloCpp");
    ad.setFrameSize({ 1280, 720 });
    ad.setFrameZoomFactor(1);
    ad.setDesignResolutionSize({ 1280, 720 }, ResolutionPolicy::NoBorder);
    ad.setAnimationInterval(1.0 / 60);
    ad.onDidFinishLaunching = []()->bool {
        // create scene ?
        return true;
    };
    ad.onMainLoop = []() {
        // frame logic?
    };

    return ad.run();
}
