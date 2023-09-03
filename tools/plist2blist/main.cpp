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
		std::cout << "handle file: " << p << std::endl;
		xx::Data d;
		d.Write(tp.premultiplyAlpha);
		d.Write(tp.realTextureFileName);
		d.Write(tp.frames.size());
		for (auto& f : tp.frames) {
			std::cout << "handle frame: " << f->key << std::endl;
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

		//bool premultiplyAlpha;
		//std::string realTextureFileName;
		//size_t numFrames{};
		//if (int r = d.Read(premultiplyAlpha, realTextureFileName, numFrames)) return r;
		//for (size_t i = 0; i < numFrames; ++i) {
		//	std::string frameKey;
		//	std::optional<xx::XY> anchor;
		//	xx::XY spriteOffset, spriteSize, spriteSourceSize;
		//	float x, y, w, h;
		//	if (int r = d.Read(frameKey, anchor)) return r;
		//	if (int r = d.Read((xx::RWFloatUInt16&)spriteOffset.x, (xx::RWFloatUInt16&)spriteOffset.y)) return r;
		//	if (int r = d.Read((xx::RWFloatUInt16&)spriteSize.x, (xx::RWFloatUInt16&)spriteSize.y)) return r;
		//	if (int r = d.Read((xx::RWFloatUInt16&)spriteSourceSize.x, (xx::RWFloatUInt16&)spriteSourceSize.y)) return r;
		//	if (int r = d.Read((xx::RWFloatUInt16&)x, (xx::RWFloatUInt16&)y)) return r;
		//	if (int r = d.Read((xx::RWFloatUInt16&)w, (xx::RWFloatUInt16&)h)) return r;
		//}
	}
	std::cout << "finished. handled " << n << " files! \npress any key continue...\n";
	std::cin.get();

	return 0;
}
