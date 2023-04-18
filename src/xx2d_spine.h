#pragma once
#include "xx2d.h"
#include <spine/spine.h>

namespace spine {

	SpineExtension* getDefaultExtension();

	struct XxSkeleton {
		Skeleton* skeleton{};
		AnimationState* state{};
		float timeScale{ 1 };
		mutable bool usePremultipliedAlpha{ true };

		XxSkeleton(SkeletonData* skeletonData, AnimationStateData* stateData = 0);
		~XxSkeleton();

		void Update(float delta);
		void Draw();

	protected:
		mutable bool ownsAnimationStateData{};
		mutable Vector<float> worldVertices;
		mutable Vector<float> tempUvs;
		mutable Vector<Color> tempColors;
		mutable Vector<unsigned short> quadIndices;
		mutable SkeletonClipping clipper;
	};

	struct XxTextureLoader : public TextureLoader {
		void load(AtlasPage& page, const String& path) override;
		void unload(void* texture) override;
	};
	extern XxTextureLoader xxTextureLoader;

	struct XxListener : AnimationStateListenerObject {
		using HandlerType = std::function<void(AnimationState* state, EventType type, TrackEntry* entry, Event* e)>;
		HandlerType h;
		XxListener(HandlerType h) : h(std::move(h)) {}
		void callback(AnimationState* state, EventType type, TrackEntry* entry, Event* e) override;
	};

	// todo: more enhance
	struct XxPlayer {
		std::shared_ptr<Atlas> atlas;	// can shrae?
		std::shared_ptr<SkeletonData> skData;	// can share?

		std::optional<AnimationStateData> asData;
		std::optional<XxSkeleton> xxSkel;
		std::optional<XxListener> xxListener;

		XxPlayer& Init(XxPlayer const& sharedRes);
		XxPlayer& Init1(const String& atlasName);
		XxPlayer& Init2_Json(const String& filename, float scale);
		XxPlayer& Init2_Skel(const String& filename, float scale);
		XxPlayer& Init3();

		XxPlayer& SetMix(const String& fromName, const String& toName, float duration);
		XxPlayer& SetTimeScale(float t);
		XxPlayer& SetUsePremultipliedAlpha(bool b);
		XxPlayer& SetPosition(float x, float y);
		XxPlayer& SetListener(XxListener::HandlerType h);
		XxPlayer& AddAnimation(size_t trackIndex, const String& animationName, bool loop, float delay);
		XxPlayer& Update(float delta);
		void Draw();
	};
}
