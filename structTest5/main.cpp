#include "xx_webm.h"

int main(int argc, char* argv[]) {
	(void)argc;	(void)argv;

	xx::Webm wm;
	if (int r = wm.LoadFromWebm("res/a.webm")) return r;
	if (int r = wm.SaveToXxmv("res/a.xxmv")) return r;

	if (int r = wm.LoadFromXxmv("res/a.xxmv")) return r;
	//if (int r = wm.SaveToPngs("res/", "a")) return r;

	// 性能评测结果：下面这行代码 9900k cpu 大约执行 0.17x 秒. 如果不写磁盘 0.04x 秒. 手机上性能如何，需要实测。有可能慢 10 倍+ ?
	if (int r = wm.SaveToPackedPngs("res/", "a")) return r;

	std::cout << "done." << std::endl;
	return 0;
}
