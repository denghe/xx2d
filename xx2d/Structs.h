#pragma once

struct Vec2 {
	float x, y;
};
struct Vec3 : Vec2 {
	float z;
};
struct Vec4 : Vec3 {
	float w;
};
struct Size {
	float width, height;
};
struct Rect {
	Vec2 origin;
	Size size;
};

struct Color4B {
	GLubyte r = 0;
	GLubyte g = 0;
	GLubyte b = 0;
	GLubyte a = 0;
};

struct Tex2F {
	GLfloat u = 0;
	GLfloat v = 0;
};

struct V3F_C4B_T2F {
	Vec3		vertices;            // 12 bytes
	Color4B     colors;              // 4 bytes
	Tex2F       texCoords;           // 8 bytes
};

struct Mat4 {
	float m[16];

	Mat4() = default;
	Mat4(std::initializer_list<float> fs) {
		for (auto i = 0; i < fs.size(); ++i) {
			m[i] = *(fs.begin() + i);
		}
	}

	inline static void createOrthographicOffCenter(float left, float right, float bottom, float top,
		float zNearPlane, float zFarPlane, Mat4* dst)
	{
		assert(dst);
		assert(right != left);
		assert(top != bottom);
		assert(zFarPlane != zNearPlane);

		memset(dst, 0, sizeof(Mat4));
		dst->m[0] = 2 / (right - left);
		dst->m[5] = 2 / (top - bottom);
		dst->m[10] = 2 / (zNearPlane - zFarPlane);

		dst->m[12] = (left + right) / (left - right);
		dst->m[13] = (top + bottom) / (bottom - top);
		dst->m[14] = (zNearPlane + zFarPlane) / (zNearPlane - zFarPlane);
		dst->m[15] = 1;
	}
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
