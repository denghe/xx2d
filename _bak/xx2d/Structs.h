#pragma once
#include "xx_ptr.h"
#include "glew/glew.h"
#include "glfw/glfw3.h"
#include <initializer_list>

struct Vec2 {
	float x = 0, y = 0;

	inline bool isZero() const {
		return x == 0.0f && y == 0.0f;
	}
};
struct Vec3 : Vec2 {
	float z = 0;
};
struct Vec4 : Vec3 {
	float w = 0;
};

struct Size {
	float width = 0, height = 0;
};

struct Rect {
	Vec2 origin;
	Size size;
};

struct Color3B {
	GLubyte r = 0, g = 0, b = 0;
};
struct Color4B : Color3B {
	GLubyte a = 0;
};

struct Tex2F {
	GLfloat u = 0, v = 0;
};



/**Enum the preallocated vertex attribute. */
enum {
	/**Index 0 will be used as Position.*/
	VERTEX_ATTRIB_POSITION,
	/**Index 1 will be used as Color.*/
	VERTEX_ATTRIB_COLOR,
	/**Index 2 will be used as Tex coord unit 0.*/
	VERTEX_ATTRIB_TEX_COORD,
	/**Index 3 will be used as Tex coord unit 1.*/
	VERTEX_ATTRIB_TEX_COORD1,
	/**Index 4 will be used as Tex coord unit 2.*/
	VERTEX_ATTRIB_TEX_COORD2,
	/**Index 5 will be used as Tex coord unit 3.*/
	VERTEX_ATTRIB_TEX_COORD3,
	/**Index 6 will be used as Normal.*/
	VERTEX_ATTRIB_NORMAL,
	/**Index 7 will be used as Blend weight for hardware skin.*/
	VERTEX_ATTRIB_BLEND_WEIGHT,
	/**Index 8 will be used as Blend index.*/
	VERTEX_ATTRIB_BLEND_INDEX,
	/**Index 9 will be used as tangent.*/
	VERTEX_ATTRIB_TANGENT,
	/**Index 10 will be used as Binormal.*/
	VERTEX_ATTRIB_BINORMAL,
	VERTEX_ATTRIB_MAX,

	// backward compatibility
	VERTEX_ATTRIB_TEX_COORDS = VERTEX_ATTRIB_TEX_COORD,
};
struct V3F_C4B_T2F {
	Vec3		vertices;            // 12 bytes
	Color4B     colors;              // 4 bytes
	Tex2F       texCoords;           // 8 bytes
};


struct Quaternion : Vec4 {
};

struct Mat4 {
	float m[16] {};
	float const& operator[](int const& idx) const { return m[idx]; }
	float& operator[](int const& idx) { return m[idx]; }

	Mat4() = default;
	Mat4(std::initializer_list<float> fs) {
		for (auto i = 0; i < fs.size(); ++i) {
			m[i] = *(fs.begin() + i);
		}
	}

	inline static Mat4 Multiply(const Mat4& m1, const Mat4& m2) {
		Mat4 r;
		r[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
		r[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
		r[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
		r[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];

		r[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
		r[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
		r[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
		r[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];

		r[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
		r[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
		r[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
		r[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];

		r[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
		r[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
		r[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
		r[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];
		return r;
	}

	static void createOrthographicOffCenter(float left, float right, float bottom, float top,
		float zNearPlane, float zFarPlane, Mat4* dst);

	static void createTranslation(float xTranslation, float yTranslation, float zTranslation, Mat4* dst);
	static void createRotation(const Quaternion& q, Mat4* dst);
};

inline static const Mat4 Mat4_IDENTITY = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };

inline static const Mat4 Mat4_ZERO = {
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0 };


struct Node;
using Node_s = xx::Shared<Node>;
using Node_w = xx::Weak<Node>;

struct Node {
	std::vector<Node_s> _children;  ///< array of children nodes
	Node_w _parent;                 ///< weak reference to parent node

	Vec2 _position;                 ///< position of the node
	float _positionZ = 0;               ///< OpenGL real Z position

	float _scaleX = 1.0f;           ///< scaling factor on x-axis
	float _scaleY = 1.0f;           ///< scaling factor on y-axis

	Quaternion _rotationQuat;       ///rotation using quaternion, if _rotationZ_X == _rotationZ_Y, _rotationQuat = RotationZ_X * RotationY * RotationX, else _rotationQuat = RotationY * RotationX

	Vec2 _anchorPointInPoints;      ///< anchor point in points
	Vec2 _anchorPoint;              ///< anchor point normalized (NOT in points)

	Size _contentSize;              ///< untransformed size of the node
	bool _contentSizeDirty = true;         ///< whether or not the contentSize is dirty

	bool _visible = true;					///< whether or not the contentSize is dirty

	bool _reorderChildDirty = false;        ///< children order dirty flag
	bool _isTransitionFinished = false;     ///< flag to indicate whether the transition was finished
	Mat4 _modelViewTransform;       ///< ModelView transform of the Node.

	mutable Mat4 _transform = Mat4_IDENTITY;        ///< transform
	mutable bool _transformDirty = true;   ///< transform dirty flag

	mutable Mat4 _inverse = Mat4_IDENTITY;          ///< inverse transform
	mutable bool _inverseDirty = true;     ///< inverse transform dirty flag

	mutable Mat4* _additionalTransform = nullptr; ///< two transforms needed by additional transforms
	mutable bool _additionalTransformDirty; ///< transform dirty ?

	bool _transformUpdated = true;         ///< Whether or not the Transform object was updated since the last frame

	// opacity controls
	Color3B     _displayedColor{ 255,255,255 };
	Color3B     _realColor{ 255,255,255 };
	bool        _cascadeColorEnabled = false;
	bool        _cascadeOpacityEnabled = false;
	GLubyte     _displayedOpacity = 255;
	GLubyte     _realOpacity = 255;


	virtual void NodeToParentTransform() const;
	virtual void Render(const Mat4* eyeTransforms, const Mat4* eyeProjections, uint32_t multiViewCount);
};



struct Viewport {
	float _left = 0, _bottom = 0, _width = 0, _height = 0;
};
struct Plane {
	Vec3 _normal; // the normal line of the plane
	float _dist; // original displacement of the normal
};
struct Frustum {
	Plane _plane[6];             // clip plane, left, right, top, bottom, near, far
	bool _clipZ;                // use near and far clip plane
	bool _initialized;
};

struct Camera : Node {
	inline static Camera* _visitingCamera = nullptr;
	inline static Viewport _defaultViewport;

	//Scene* _scene = nullptr; //Scene camera belongs to
	Mat4 _projection;
	mutable Mat4 _view;
	mutable Mat4 _viewInv;
	mutable Mat4 _viewProjection;

	Vec3 _up;
	//Camera::Type _type;
	float _fieldOfView;
	float _zoom[2];
	float _aspectRatio;
	float _nearPlane;
	float _farPlane;
	mutable bool  _viewProjectionDirty = true;
	bool _viewProjectionUpdated = false; //Whether or not the viewprojection matrix was updated since the last frame.
	//CameraFlag _cameraFlag = CameraFlag::DEFAULT; // camera flag
	mutable Frustum _frustum;   // camera frustum
	mutable bool _frustumDirty = true;
	int8_t  _depth = -1;                 //camera depth, the depth of camera with CameraFlag::DEFAULT flag is 0 by default, a camera with larger depth is drawn on top of camera with smaller depth

	//CameraBackgroundBrush* _clearBrush = nullptr; //brush used to clear the back ground

	Viewport _viewport;
	//FrameBuffer* _fbo = nullptr;
	GLint _oldViewport[4];
};
