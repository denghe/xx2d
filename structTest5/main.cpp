#include "xx_webm.h"

int main(int argc, char* argv[]) {
	(void)argc;	(void)argv;

	xx::Webm wm;
	if (int r = wm.LoadFromWebm("res/zhangyu.webm")) return r;
	if (int r = wm.SaveToXxmv("res/zhangyu.xxmv")) return r;
	if (int r = wm.SaveToPngs("res", "zhangyu")) return r;
	if (int r = wm.LoadFromWebm("res/xiaoshuimu.webm")) return r;
	if (int r = wm.SaveToXxmv("res/xiaoshuimu.xxmv")) return r;
	if (int r = wm.LoadFromWebm("res/seaturtle.webm")) return r;
	if (int r = wm.SaveToXxmv("res/seaturtle.xxmv")) return r;
	if (int r = wm.LoadFromWebm("res/judaxiaochouyu.webm")) return r;
	if (int r = wm.SaveToXxmv("res/judaxiaochouyu.xxmv")) return r;
	if (int r = wm.LoadFromWebm("res/judadieyu.webm")) return r;
	if (int r = wm.SaveToXxmv("res/judadieyu.xxmv")) return r;
	if (int r = wm.LoadFromWebm("res/qiyu.webm")) return r;
	if (int r = wm.SaveToXxmv("res/qiyu.xxmv")) return r;

	//if (int r = wm.LoadFromXxmv("res/a.xxmv")) return r;
	//if (int r = wm.SaveToPngs("res/", "a")) return r;

	// 性能评测结果：下面这行代码 9900k cpu 大约执行 0.17x 秒. 如果不写磁盘 0.04x 秒
	//if (int r = wm.SaveToPackedPngs("res/", "a")) return r;

	std::cout << "done." << std::endl;
	return 0;
}
