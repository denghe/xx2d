#include "main.h"
#include <spine/spine.h>

namespace spine {

	SpineExtension* getDefaultExtension() {
		return new DefaultSpineExtension();
	}

	struct XxSkeleton {

		Skeleton* skeleton{};
		AnimationState* state{};
		float timeScale{ 1 };
		mutable bool usePremultipliedAlpha{};

		// normal, additive, multiply, screen
		inline static const constexpr std::array<std::pair<uint32_t, uint32_t>, 4> nonpmaBlendFuncs{ std::pair<uint32_t, uint32_t>
		{ GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA }, { GL_SRC_ALPHA, GL_ONE }, { GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA }, { GL_ONE, GL_ONE_MINUS_SRC_COLOR } };

		// pma: normal, additive, multiply, screen
		inline static const constexpr std::array<std::pair<uint32_t, uint32_t>, 4> pmaBlendFuncs{ std::pair<uint32_t, uint32_t>
		{ GL_ONE, GL_ONE_MINUS_SRC_ALPHA }, { GL_ONE, GL_ONE }, { GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA }, { GL_ONE, GL_ONE_MINUS_SRC_COLOR } };


		XxSkeleton(SkeletonData* skeletonData, AnimationStateData* stateData = 0) {

			Bone::setYDown(true);

			worldVertices.ensureCapacity(1000);
			skeleton = new (__FILE__, __LINE__) Skeleton(skeletonData);
			tempUvs.ensureCapacity(16);
			tempColors.ensureCapacity(16);

			ownsAnimationStateData = stateData == 0;
			if (ownsAnimationStateData) stateData = new (__FILE__, __LINE__) AnimationStateData(skeletonData);

			state = new (__FILE__, __LINE__) AnimationState(stateData);

			quadIndices.add(0);
			quadIndices.add(1);
			quadIndices.add(2);
			quadIndices.add(2);
			quadIndices.add(3);
			quadIndices.add(0);
		}

		~XxSkeleton() {
			if (ownsAnimationStateData) delete state->getData();
			delete state;
			delete skeleton;
		}

		void Update(float delta) {
			state->update(delta * timeScale);
			state->apply(*skeleton);
			skeleton->updateWorldTransform();
		}

		void Draw() {

			auto&& shader = xx::engine.sm.GetShader<xx::Shader_Spine>();

			// Early out if skeleton is invisible
			if (skeleton->getColor().a == 0) return;

			xx::RGBA8 c{};
			xx::GLTexture* texture;

			for (size_t i = 0, e = skeleton->getSlots().size(); i < e; ++i) {

				auto&& slot = *skeleton->getDrawOrder()[i];
				auto&& attachment = slot.getAttachment();
				if (!attachment) continue;

				if (slot.getColor().a == 0 || !slot.getBone().isActive()) {
					clipper.clipEnd(slot);
					continue;
				}

				Vector<float>* vertices = &worldVertices;
				Vector<float>* uvs{};
				Vector<unsigned short>* indices{};
				int indicesCount{};
				Color* attachmentColor{};

				auto&& attachmentRTTI = attachment->getRTTI();
				if (attachmentRTTI.isExactly(RegionAttachment::rtti)) {

					auto&& regionAttachment = (RegionAttachment*)attachment;
					attachmentColor = &regionAttachment->getColor();

					if (attachmentColor->a == 0) {
						clipper.clipEnd(slot);
						continue;
					}

					worldVertices.setSize(8, 0);
					regionAttachment->computeWorldVertices(slot, worldVertices, 0, 2);
					uvs = &regionAttachment->getUVs();
					indices = &quadIndices;
					indicesCount = 6;
					texture = &*(xx::Shared<xx::GLTexture>&)((AtlasRegion*)regionAttachment->getRegion())->page->texture;

				} else if (attachmentRTTI.isExactly(MeshAttachment::rtti)) {

					auto&& mesh = (MeshAttachment*)attachment;
					attachmentColor = &mesh->getColor();

					if (attachmentColor->a == 0) {
						clipper.clipEnd(slot);
						continue;
					}

					worldVertices.setSize(mesh->getWorldVerticesLength(), 0);
					mesh->computeWorldVertices(slot, 0, mesh->getWorldVerticesLength(), worldVertices.buffer(), 0, 2);
					uvs = &mesh->getUVs();
					indices = &mesh->getTriangles();
					indicesCount = mesh->getTriangles().size();
					texture = &*(xx::Shared<xx::GLTexture>&)((AtlasRegion*)mesh->getRegion())->page->texture;

				} else if (attachmentRTTI.isExactly(ClippingAttachment::rtti)) {

					ClippingAttachment* clip = (ClippingAttachment*)slot.getAttachment();
					clipper.clipStart(slot, clip);
					continue;

				} else
					continue;

				auto&& skc = skeleton->getColor();
				auto&& slc = slot.getColor();
				c.r = (uint8_t)(skc.r * slc.r * attachmentColor->r * 255);
				c.g = (uint8_t)(skc.g * slc.g * attachmentColor->g * 255);
				c.b = (uint8_t)(skc.b * slc.b * attachmentColor->b * 255);
				c.a = (uint8_t)(skc.a * slc.a * attachmentColor->a * 255);

				std::pair<uint32_t, uint32_t> blend;
				if (!usePremultipliedAlpha) {
					blend = nonpmaBlendFuncs[slot.getData().getBlendMode()];
				} else {
					blend = pmaBlendFuncs[slot.getData().getBlendMode()];
				}

				if (xx::engine.blendFuncs != blend) {
					xx::engine.sm.End();
					xx::engine.GLBlendFunc(blend);
				}

				if (clipper.isClipping()) {
					clipper.clipTriangles(worldVertices, *indices, *uvs, 2);
					vertices = &clipper.getClippedVertices();
					uvs = &clipper.getClippedUVs();
					indices = &clipper.getClippedTriangles();
					indicesCount = clipper.getClippedTriangles().size();
				}

				auto vp = shader.Draw(*texture, indicesCount);
				xx::XY size(std::get<1>(texture->vs), std::get<2>(texture->vs));
				for (size_t ii = 0; ii < indicesCount; ++ii) {
					auto index = (*indices)[ii] << 1;
					auto&& vertex = vp[ii];
					vertex.x = (*vertices)[index];
					vertex.y = (*vertices)[index + 1];
					vertex.u = (*uvs)[index] * size.x;
					vertex.v = (*uvs)[index + 1] * size.y;
					memcpy(&vertex.r, &c, sizeof(c));
				}

				clipper.clipEnd(slot);
			}

			clipper.clipEnd();
		}

	private:
		mutable bool ownsAnimationStateData{};
		mutable Vector<float> worldVertices;
		mutable Vector<float> tempUvs;
		mutable Vector<Color> tempColors;
		mutable Vector<unsigned short> quadIndices;
		mutable SkeletonClipping clipper;
	};

	struct XxTextureLoader : public TextureLoader {
		virtual void load(AtlasPage& page, const String& path) {

			std::string_view fn(path.buffer(), path.length());
			auto tex = xx::engine.LoadTextureFromCache(fn);

			xx::GLTexParm(*tex
				, page.magFilter == TextureFilter_Linear ? GL_LINEAR : GL_NEAREST
				, (page.uWrap == TextureWrap_Repeat && page.vWrap == TextureWrap_Repeat) ? GL_REPEAT : GL_CLAMP_TO_EDGE
			);

			page.width = std::get<1>(tex->vs);
			page.height = std::get<2>(tex->vs);
			page.texture = tex.pointer;	// move to
			tex.pointer = nullptr;
		}

		virtual void unload(void* texture) {
			xx::Shared<xx::GLTexture> tex;
			tex.pointer = (xx::GLTexture*)texture;	// move back
		}
	};

	void XxCallback(AnimationState* state, EventType type, TrackEntry* entry, Event* event) {
#if 0
		SP_UNUSED(state);
		const String& animationName = (entry && entry->getAnimation()) ? entry->getAnimation()->getName() : String("");

		switch (type) {
		case EventType_Start:
			printf("%d start: %s\n", entry->getTrackIndex(), animationName.buffer());
			break;
		case EventType_Interrupt:
			printf("%d interrupt: %s\n", entry->getTrackIndex(), animationName.buffer());
			break;
		case EventType_End:
			printf("%d end: %s\n", entry->getTrackIndex(), animationName.buffer());
			break;
		case EventType_Complete:
			printf("%d complete: %s\n", entry->getTrackIndex(), animationName.buffer());
			break;
		case EventType_Dispose:
			printf("%d dispose: %s\n", entry->getTrackIndex(), animationName.buffer());
			break;
		case EventType_Event:
			printf("%d event: %s, %s: %d, %f, %s %f %f\n", entry->getTrackIndex(), animationName.buffer(), event->getData().getName().buffer(), event->getIntValue(), event->getFloatValue(),
				event->getStringValue().buffer(), event->getVolume(), event->getBalance());
			break;
		}
		fflush(stdout);
#endif
	}

	std::shared_ptr<SkeletonData> XxReadSkeletonJsonData(const String& filename, Atlas* atlas, float scale) {
		SkeletonJson json(atlas);
		json.setScale(scale);
		auto skeletonData = json.readSkeletonDataFile(filename);
		if (!skeletonData) {
			printf("%s\n", json.getError().buffer());
			exit(0);
		}
		return std::shared_ptr<SkeletonData>(skeletonData);
	}

	std::shared_ptr<SkeletonData> XxReadSkeletonBinaryData(const char* filename, Atlas* atlas, float scale) {
		SkeletonBinary binary(atlas);
		binary.setScale(scale);
		auto skeletonData = binary.readSkeletonDataFile(filename);
		if (!skeletonData) {
			printf("%s\n", binary.getError().buffer());
			exit(0);
		}
		return std::shared_ptr<SkeletonData>(skeletonData);
	}

	xx::Coro GoSpineBoy(const char* binaryName, const char* atlasName, float scale) {
		XxTextureLoader textureLoader;
		auto atlas = std::make_unique<Atlas>(atlasName, &textureLoader);
		auto skeletonData = XxReadSkeletonBinaryData(binaryName, atlas.get(), scale);

		// Configure mixing.
		AnimationStateData stateData(skeletonData.get());
		stateData.setMix("walk", "jump", 0.2f);
		stateData.setMix("jump", "run", 0.2f);

		XxSkeleton drawable(skeletonData.get(), &stateData);
		drawable.timeScale = 1;
		drawable.usePremultipliedAlpha = true;

		Skeleton* skeleton = drawable.skeleton;
		skeleton->setToSetupPose();

		skeleton->setPosition(320, 590);
		skeleton->updateWorldTransform();

		Slot* headSlot = skeleton->findSlot("head");

		drawable.state->setListener(XxCallback);
		drawable.state->addAnimation(0, "walk", true, 0);
		drawable.state->addAnimation(0, "jump", false, 3);
		drawable.state->addAnimation(0, "run", true, 0);

		SkeletonBounds bounds;
		while (true) {
			CoYield;

			bounds.update(*skeleton, true);
			auto pos = xx::engine.mousePosition;
			if (bounds.containsPoint(pos.x, pos.y)) {
				headSlot->getColor().g = 0;
				headSlot->getColor().b = 0;
			} else {
				headSlot->getColor().g = 1;
				headSlot->getColor().b = 1;
			}

			drawable.Update(xx::engine.delta);
			drawable.Draw();
		}
	}
}


void GameLooper::Init() {
	fontBase = xx::engine.LoadBMFont("res/font/coderscrux.fnt"sv);
	font3500 = xx::engine.LoadBMFont("res/font/3500+.fnt"sv);
	fpsViewer.Init(fontBase);

	auto skelFN = xx::engine.GetFullPath("spine-runtimes/spine-sfml/cpp/data/spineboy-pro.skel");
	auto atlasFN = xx::engine.GetFullPath("spine-runtimes/spine-sfml/cpp/data/spineboy-pma.atlas");

	for (size_t i = 0; i < 1000; i++) {
		coros.Add(spine::GoSpineBoy(skelFN.c_str(), atlasFN.c_str(), 0.5f));
	}
}

int GameLooper::Update() {
	coros();

	fpsViewer.Update();
	return 0;
}

int main() {
	auto g = std::make_unique<GameLooper>();
	return g->Run("xx2d's examples");
}
