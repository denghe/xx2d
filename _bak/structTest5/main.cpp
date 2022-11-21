#include "xx_webm.h"

int main(int argc, char* argv[]) {
	(void)argc;	(void)argv;
	std::cout << "convert *.webm to *.xxvm. press any key to continue... or Ctrl + C exit" << std::endl;
	std::cin.get();
	std::cout << "working dir:" << std::filesystem::current_path() << std::endl;
	xx::Webm wm;
	for (auto&& entry : std::filesystem::recursive_directory_iterator(std::filesystem::current_path())) {
		if (!entry.is_regular_file()) continue;
		auto&& ext = entry.path().extension().string();
		if (ext != ".webm") continue;
		auto p = entry.path();
		if (int r = wm.LoadFromWebm(p)) {
			std::cout << "load error = " << r << ", path = " << p << std::endl;
		}
		else {
			p = p.replace_extension(".xxmv");
			if (int r = wm.SaveToXxmv(p)) {
				std::cout << "save error = " << r << ", path = " << p << std::endl;
			}
			else {
				std::cout << "save success, path = " << p << std::endl;
			}
		}
	}

	std::cout << "done. press any key to exit..." << std::endl;
	std::cin.get();

	//if (int r = wm.LoadFromWebm("res/st_k10.webm")) return r;
	//if (int r = wm.SaveToXxmv("res/st_k10.xxmv")) return r;
	//if (int r = wm.LoadFromWebm("res/st_k50.webm")) return r;
	//if (int r = wm.SaveToXxmv("res/st_k50.xxmv")) return r;
	//if (int r = wm.LoadFromWebm("res/st_k100.webm")) return r;
	//if (int r = wm.SaveToXxmv("res/st_k100.xxmv")) return r;
	//if (int r = wm.LoadFromWebm("res/st_k200.webm")) return r;
	//if (int r = wm.SaveToXxmv("res/st_k200.xxmv")) return r;

	//if (int r = wm.LoadFromWebm("res/zhangyu.webm")) return r;
	//if (int r = wm.SaveToXxmv("res/zhangyu.xxmv")) return r;
	//if (int r = wm.SaveToPngs("res", "zhangyu")) return r;
	//if (int r = wm.LoadFromWebm("res/xiaoshuimu.webm")) return r;
	//if (int r = wm.SaveToXxmv("res/xiaoshuimu.xxmv")) return r;
	//if (int r = wm.LoadFromWebm("res/seaturtle.webm")) return r;
	//if (int r = wm.SaveToXxmv("res/seaturtle.xxmv")) return r;
	//if (int r = wm.LoadFromWebm("res/judaxiaochouyu.webm")) return r;
	//if (int r = wm.SaveToXxmv("res/judaxiaochouyu.xxmv")) return r;
	//if (int r = wm.LoadFromWebm("res/judadieyu.webm")) return r;
	//if (int r = wm.SaveToXxmv("res/judadieyu.xxmv")) return r;
	//if (int r = wm.LoadFromWebm("res/qiyu.webm")) return r;
	//if (int r = wm.SaveToXxmv("res/qiyu.xxmv")) return r;

	//if (int r = wm.LoadFromXxmv("res/a.xxmv")) return r;
	//if (int r = wm.SaveToPngs("res/", "a")) return r;

	// 性能评测结果：下面这行代码 9900k cpu 大约执行 0.17x 秒. 如果不写磁盘 0.04x 秒
	//if (int r = wm.SaveToPackedPngs("res/", "a")) return r;

	//std::cout << "done." << std::endl;
	return 0;
}
