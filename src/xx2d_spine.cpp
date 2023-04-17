#include "xx2d.h"
#include "xx2d_spine.h"

namespace spine {

	SpineExtension* getDefaultExtension() {
		return new DefaultSpineExtension();
	}

	// normal, additive, multiply, screen
	inline static const constexpr std::array<std::pair<uint32_t, uint32_t>, 4> nonpmaBlendFuncs{ std::pair<uint32_t, uint32_t>
	{ GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA }, { GL_SRC_ALPHA, GL_ONE }, { GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA }, { GL_ONE, GL_ONE_MINUS_SRC_COLOR } };

	// pma: normal, additive, multiply, screen
	inline static const constexpr std::array<std::pair<uint32_t, uint32_t>, 4> pmaBlendFuncs{ std::pair<uint32_t, uint32_t>
	{ GL_ONE, GL_ONE_MINUS_SRC_ALPHA }, { GL_ONE, GL_ONE }, { GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA }, { GL_ONE, GL_ONE_MINUS_SRC_COLOR } };

	XxTextureLoader xxTextureLoader;

	XxSkeleton::XxSkeleton(SkeletonData* skeletonData, AnimationStateData* stateData) {

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

	XxSkeleton::~XxSkeleton() {
		if (ownsAnimationStateData) delete state->getData();
		delete state;
		delete skeleton;
	}

	void XxSkeleton::Update(float delta) {
		state->update(delta * timeScale);
		state->apply(*skeleton);
		skeleton->updateWorldTransform();
	}

	void XxSkeleton::Draw() {

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
			xx::XY size(texture->Width(), texture->Height());
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


	void XxTextureLoader::load(AtlasPage& page, const String& path) {

		std::string_view fn(path.buffer(), path.length());
		auto tex = xx::engine.LoadTextureFromCache(fn);

		tex->SetGLTexParm(
			page.magFilter == TextureFilter_Linear ? GL_LINEAR : GL_NEAREST, 
			(page.uWrap == TextureWrap_Repeat && page.vWrap == TextureWrap_Repeat) ? GL_REPEAT : GL_CLAMP_TO_EDGE
		);

		page.width = tex->Width();
		page.height = tex->Height();
		page.texture = tex.pointer;	// move to
		tex.pointer = nullptr;
	}

	void XxTextureLoader::unload(void* texture) {
		xx::Shared<xx::GLTexture> tex;
		tex.pointer = (xx::GLTexture*)texture;	// move back
	}

	extern XxTextureLoader xxTextureLoader;


	void XxListener::callback(AnimationState* state, EventType type, TrackEntry* entry, Event* e) {
		h(state, type, entry, e);
	}



	XxPlayer& XxPlayer::Init1(std::shared_ptr<Atlas> atlas) {
		this->atlas = atlas;
		return *this;
	}
	XxPlayer& XxPlayer::Init1(const String& atlasName) {
		atlas.reset(new Atlas(atlasName, &xxTextureLoader));
		return *this;
	}

	XxPlayer& XxPlayer::Init2_Json(const String& filename, float scale) {
		SkeletonJson parser(atlas.get());
		parser.setScale(scale);
		skData.reset(parser.readSkeletonDataFile(filename));	// todo: read  buf + len
		if (!skData) throw std::logic_error(xx::ToString("Init2_Json failed. fn = ", filename));
		return *this;
	}

	XxPlayer& XxPlayer::Init2_Skel(const String& filename, float scale) {
		SkeletonBinary parser(atlas.get());
		parser.setScale(scale);
		skData.reset(parser.readSkeletonDataFile(filename));	// todo: read  buf + len
		if (!skData) throw std::logic_error(xx::ToString("Init2_Skel failed. fn = ", filename));
		return *this;
	}

	XxPlayer& XxPlayer::Init2(std::shared_ptr<SkeletonData> skData) {
		this->skData = skData;
		return *this;
	}

	XxPlayer& XxPlayer::Init3() {
		assert(atlas);
		assert(skData);
		asData.emplace(skData.get());
		xxSkel.emplace(skData.get(), &*asData);
		xxSkel->skeleton->setToSetupPose();
		return *this;
	}

	XxPlayer& XxPlayer::SetMix(const String& fromName, const String& toName, float duration) {
		asData->setMix(fromName, toName, duration);
		return *this;
	}

	XxPlayer& XxPlayer::SetTimeScale(float t) {
		xxSkel->timeScale = t;
		return *this;
	}

	XxPlayer& XxPlayer::SetUsePremultipliedAlpha(bool b) {
		xxSkel->usePremultipliedAlpha = b;
		return *this;
	}

	XxPlayer& XxPlayer::SetPosition(float x, float y) {
		xxSkel->skeleton->setPosition(x, -y);
		xxSkel->skeleton->updateWorldTransform();
		return *this;
	}

	XxPlayer& XxPlayer::SetListener(XxListener::HandlerType h) {
		xxListener.emplace(std::move(h));
		xxSkel->state->setListener(&*xxListener);
		return *this;
	}

	XxPlayer& XxPlayer::AddAnimation(size_t trackIndex, const String& animationName, bool loop, float delay) {
		xxSkel->state->addAnimation(trackIndex, animationName, loop, delay);
		return *this;
	}

	XxPlayer& XxPlayer::Update(float delta) {
		xxSkel->Update(delta);
		return *this;
	}

	void XxPlayer::Draw() {
		xxSkel->Draw();
	}
}
