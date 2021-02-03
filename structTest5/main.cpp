#include "xx_webm.h"

int main(int argc, char* argv[]) {
	(void)argc;	(void)argv;

	xx::Webm wm;
	if (int r = wm.LoadFromWebm("res/a.webm")) return r;
	if (int r = wm.SaveToXxmv("res/a.xxmv")) return r;

	if (int r = wm.LoadFromXxmv("res/a.xxmv")) return r;
	//if (int r = wm.SaveToPngs("res/", "a")) return r;
	if (int r = wm.SaveToPackedPngs("res/", "a")) return r;

	std::cout << "done." << std::endl;
	return 0;
}
