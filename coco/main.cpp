#include "pch.h"
#include "AppDelegate.h"

int main() {
    gAppDelegate.Emplace();
    gAppDelegate->setTitle("HelloCpp");
    gAppDelegate->setFrameSize(1280, 720);
    gAppDelegate->setFrameZoomFactor(1.f);

    gAppDelegate->onDidFinishLaunching = []()->bool {
        // create scene ?
        return true;
    };
    gAppDelegate->onMainLoop = []() {
        // frame logic?
    };

    if (int r = gAppDelegate->initWindow()) return r;
    if (int r = gAppDelegate->initGL()) return r;
    return gAppDelegate->run();
}
