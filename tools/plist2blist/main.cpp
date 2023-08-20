#include "pch.h"
#include "main.h"

int main() {
	auto&& cp = std::filesystem::current_path();
	std::cout << "tool: *.plist -> *.blist\nworking dir: " << cp.string() << "\npress any key continue...\n";
	std::cin.get();
	int n = 0;
	for (auto&& entry : std::filesystem::/*recursive_*/directory_iterator(cp)) {
		if (!entry.is_regular_file()) continue;
		auto&& p = entry.path();
		if (p.extension() != u8".plist") continue;
		auto fullpath = std::filesystem::absolute(p).u8string();
		auto newPath = fullpath.substr(0, fullpath.size() - 6) + u8".blist";

		xx::TP tp;
		tp.Load((std::string&)p.u8string(), true, false);
		xx::Data d;
		d.Write(tp.premultiplyAlpha);
		d.Write(tp.realTextureFileName);
		d.Write(tp.frames.size());
		for (auto& f : tp.frames) {
			d.Write(f->key);
			d.Write(f->anchor);
			d.Write((xx::RWFloatUInt16&)f->spriteOffset.x, (xx::RWFloatUInt16&)f->spriteOffset.y);
			d.Write((xx::RWFloatUInt16&)f->spriteSize.x, (xx::RWFloatUInt16&)f->spriteSize.y);
			d.Write((xx::RWFloatUInt16&)f->spriteSourceSize.x, (xx::RWFloatUInt16&)f->spriteSourceSize.y);
			d.Write((xx::RWFloatUInt16&)f->textureRect.x, (xx::RWFloatUInt16&)f->textureRect.y);
			d.Write((xx::RWFloatUInt16&)f->textureRect.wh.x, (xx::RWFloatUInt16&)f->textureRect.wh.y);
		}
		xx::WriteAllBytes(newPath, d);
		++n;
	}
	std::cout << "finished. handled " << n << " files! \npress any key continue...\n";
	std::cin.get();

	return 0;
}
