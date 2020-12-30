#include "pch.h"
#include "AppDelegate.h"

int main() {
    gAppDelegate.Emplace();
    gAppDelegate->setTitle("HelloCpp");
    gAppDelegate->setFrameSize({ 1280, 720 });
    gAppDelegate->setFrameZoomFactor(1);
    gAppDelegate->setDesignResolutionSize({ 1280, 720 }, ResolutionPolicy::NoBorder);
    gAppDelegate->setAnimationInterval(1.0 / 60);

    gAppDelegate->onDidFinishLaunching = []()->bool {
        // create scene ?
        return true;
    };
    gAppDelegate->onMainLoop = []() {
        // frame logic?
    };

    if (int r = gAppDelegate->initWindow()) return r;
    return gAppDelegate->run();
}
