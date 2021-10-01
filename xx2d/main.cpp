#include "AppDelegate.h"
#include <iostream>

int main() {
	gAppDelegate.Emplace();
	int r = gAppDelegate->Run(1280, 720, false);
	if (r) {
		std::cout << gAppDelegate->lastErrorNumber << std::endl;
		std::cout << gAppDelegate->lastErrorMessage << std::endl;
	}
	gAppDelegate.Reset();
	return r;
}
