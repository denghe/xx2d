#ifndef __VMATH_HH__
#define __VMATH_HH__

#include <iostream>
#include <cmath>
#include <algorithm>

//FIXME: mat4 inverse

namespace vmath {

#define PI (4.0 * std::atan(1.0))

template<class T>
class vec2 {
public:
	vec2(T x, T y) : x(x), y(y) {}

	vec2(const vec2 &v) : x(v.x), y(v.y) {}

	vec2() : x(0), y(0) {}

	T mag() const {
		return sqrt(x * x + y * y);
	}

	vec2 norm() const {
		T _mag = mag();
		T _x = static_cast<T>(x / _mag);
		T _y = static_cast<T>(y / _mag);
		return vec2(_x, _y);
	}

	vec2 reflect(const vec2 &v2) const {
		return v2 - v2 * 2 * (dot(v2));
	}

	T cross2d(const vec2 &v2) const {
		return x * v2.y - y * v2.x;
	}

	vec2 ortho(const vec2 &v2) const {
		return vec2(-v2.y, v2.x);
	}

	vec2 ortho2(const vec2 &v2) const {
		return vec2(v2.y, -v2.x);
	}

	vec2 operator+(const vec2 &v2) const {
		return vec2(x + v2.x, y + v2.y);
	}

	vec2 operator-(const vec2 &v2) const {
		return vec2(x - v2.x, y - v2.y);
	}

	vec2 operator*(const double &d) const {
		return vec2(x * d, y * d);
	}

	T dot(const vec2 &v2) const {
		return x * v2.x + y * v2.y;
	}

	T angle(const vec2 &v2) const {
		return acos(dot(v2) / (mag() * v2.mag()));
	}

	inline const T& X() const {
		return x;
	}

	inline const T& Y() const {
		return y;
	}

	friend std::ostream &operator<<(std::ostream &stream, vec2 const &v) {
		stream << "(" << v.x << "," << v.y << ")";
		return stream;
	}

protected:
	T x;
	T y;
};

template<class T>
class vec3 {
public:
	vec3(T x, T y, T z) : x(x), y(y), z(z) {}

	vec3(const vec3 &v) : x(v.x), y(v.y), z(v.z) {}

	vec3() : x(0), y(0), z(0) {}

	T mag() const {
		return sqrt(x * x + y * y + z * z);
	}

	vec3 norm() const {
		T _mag = mag();
		T _x = static_cast<T>(x / _mag);
		T _y = static_cast<T>(y / _mag);
		T _z = static_cast<T>(z / _mag);
		return vec3(_x, _y, _z);
	}

	vec3 cross(const vec3 &v2) const {
		T x1, y1, z1;
		x1 = y * v2.z - z * v2.y;
		y1 = z * v2.x - x * v2.z;
		z1 = x * v2.y -y * v2.x;
		return vec3(x1, y1, z1);
	}

	vec3 operator+(const vec3 &v2) const {
		return vec3(x + v2.x, y + v2.y, z + v2.z);
	}

	vec3 operator-(const vec3 &v2) const {
		return vec3(x - v2.x, y - v2.y, z - v2.z);
	}

	vec3 operator*(const double &d) const {
		return vec3(x * d, y * d, z * d);
	}

	T dot(const vec3 &v2) const {
		return x * v2.x + y * v2.y + z * v2.z;
	}

	T angle(const vec3 &v2) const {
		return acos(dot(v2) / (mag() * v2.mag()));
	}

	inline const T& X() const {
		return x;
	}

	inline const T& Y() const {
		return y;
	}

	inline const T& Z() const {
		return z;
	}

	vec3 reflect(const vec3 &v2) const {
		return v2 - v2 * 2 * (dot(v2));
	}

	friend std::ostream &operator<<(std::ostream &stream, vec3 const &v) {
		stream << "(" << v.x << "," << v.y << "," << v.z << ")";
		return stream;
	}

protected:
	T x;
	T y;
	T z;
};

template <class T>
class mat2 {
public:
	static const int items = 4;

	mat2(const vec2<T> &v1, const vec2<T> &v2, bool columns) {
		std::fill(data, data + items, 0.0);
		if (!columns) {
			data[0] = v1.X();
			data[2] = v1.Y();
			data[1] = v2.X();
			data[3] = v2.Y();
		}
		else {
			data[0] = v1.X();
			data[1] = v1.Y();
			data[2] = v2.X();
			data[3] = v2.Y();
		}
	}

	mat2() {
		std::fill(data, data + items, 0.0);
	}

	mat2(const T* raw_data, bool transposed) {
		init(raw_data, transposed);
	}

	mat2(mat2 &m2) {
		init(m2.data, true);
	}

	static mat2 identity() {
		mat2 ret = mat2();
		ret.data[0] = ret.data[3] = 1;
		return ret;
	}

	mat2 transposed() {
		mat2 ret(data);
		transpose(data);
		return ret;
	}

	mat2 inverse() {
		mat2 ret;
		ret.data[0] = data[3];
		ret.data[3] = data[0];
		ret.data[1] = (-data[1]);
		ret.data[2] = (-data[2]);
		return ret;
	}

	static mat2 rotate(T theta) {
		mat2 ret;
		T c = std::cos(theta);
		T s = std::sin(theta);
		ret.data[0] = c;
		ret.data[3] = c;
		ret.data[1] = -s;
		ret.data[2] = s;
		return ret;
	}

	static mat2 scale(T sx, T sy) {
		mat2 ret = mat2();
		ret.data[0] = sx;
		ret.data[3] = sy;
		return ret;
	}

	mat2 operator+(const mat2 &m2) const {
		mat2 ret;
		for (int i = 0; i < items; i++) {
			ret.data[i] = data[i] + m2.data[i];
		}
		return ret;
	}

	mat2 operator-(const mat2 &m2) const {
		mat2 ret;
		for (int i = 0; i < items; i++) {
			ret.data[i] = data[i] - m2.data[i];
		}
		return ret;
	}

	mat2 operator*(const mat2 &m2) const {
		mat2 ret;
		ret.data[0] = data[0] * m2.data[0] + data[2] * m2.data[1];
		ret.data[2] = data[0] * m2.data[2] + data[2] * m2.data[3];
		ret.data[1] = data[1] * m2.data[0] + data[3] * m2.data[1];
		ret.data[3] = data[1] * m2.data[2] + data[3] * m2.data[3];
		return ret;
	}

	vec2<T> operator*(const vec2<T> &v2) const {
		T x, y;
		x = data[0] * v2.X() + data[2] * v2.Y();
		y = data[1] * v2.X() + data[3] * v2.Y();
		return vec2<T>(x, y);
	}

	T* getData() {
		return data;
	}

	T det() {
		return data[0] * data[3] - data[1] * data[2];
	}

	friend std::ostream &operator<<(std::ostream &stream, mat2 const &m) {
		stream << "[" << m.data[0] << " " << m.data[2] << std::endl
			<< m.data[1] << " " << m.data[3] << "]";
		return stream;
	}

protected:
	T data[items];

	void init(const T* raw_data, bool transposed) {
		std::copy(raw_data, raw_data + items, data);
		if (!transposed) {
			transpose(data);
		}
	}

	static void transpose(T* data) {
		std::swap(data[1], data[2]);
	}
};

template <class T>
class mat3 {
public:
	static const int items = 9;

	mat3(const vec3<T> &v1, const vec3<T> &v2, const vec3<T> &v3,
		bool columns)
	{
		std::fill(data, data + items, 0.0);
		if (!columns) {
			data[0] = v1.X();
			data[3] = v1.Y();
			data[6] = v1.Z();

			data[1] = v2.X();
			data[4] = v2.Y();
			data[7] = v2.Z();

			data[2] = v3.X();
			data[5] = v3.Y();
			data[8] = v3.Z();
		}
		else {
			data[0] = v1.X();
			data[1] = v1.Y();
			data[2] = v1.Z();

			data[3] = v2.X();
			data[4] = v2.Y();
			data[5] = v2.Z();

			data[6] = v3.X();
			data[7] = v3.Y();
			data[8] = v3.Z();
		}
	}

	mat3() {
		std::fill(data, data + items, (T)0.0);
	}

	mat3(const mat3 &m2) {
		init(m2.data, false);
	}

	mat3(const T* raw_data, bool transposed) {
		init(raw_data, transposed);
	}

	static mat3 identity() {
		mat3 ret = mat3();
		ret.data[0] = ret.data[4] = ret.data[8] = 1;
		return ret;
	}

	mat3 transposed() {
		mat3 ret(data, true);
		transpose(ret.data);
		return ret;
	}

	mat3 inverse() {
		mat3 ret = transposed();
		T determinant = det();
		for (int i = 0; i < items; i++) {
			ret.data[i] /= determinant;
		}
		return ret;
	}

	mat3 operator+(const mat3 &m2) const {
		mat3 ret;
		for (int i = 0; i < items; i++) {
			ret.data[i] = data[i] + m2.data[i];
		}
		return ret;
	}

	mat3 operator-(const mat3 &m2) const {
		mat3 ret;
		for (int i = 0; i < items; i++) {
			ret.data[i] = data[i] - m2.data[i];
		}
		return ret;
	}

	mat3 operator*(const mat3 &m2) const {
		mat3 ret = mat3();
		//FIXME: i am lazy to unroll it
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					ret.data[3 * j + i] += data[3 * k + i] * m2.data[3 * j + k];
				}
			}
		}

		return ret;
	}

	static mat3 rotateX(T theta) {
		mat3 ret = identity();
		T c = std::cos(theta);
		T s = std::sin(theta);
		ret.data[4] = c;
		ret.data[5] = s;
		ret.data[7] = -s;
		ret.data[8] = c;
		return ret;
	}

	static mat3 rotateY(T theta) {
		mat3 ret = identity();
		T c = std::cos(theta);
		T s = std::sin(theta);
		ret.data[0] = c;
		ret.data[6] = s;
		ret.data[2] = -s;
		ret.data[8] = c;
		return ret;
	}

	static mat3 rotateZ(T theta) {
		mat3 ret = identity();
		T c = std::cos(theta);
		T s = std::sin(theta);
		ret.data[0] = c;
		ret.data[1] = s;
		ret.data[3] = -s;
		ret.data[4] = c;
		return ret;
	}

	static mat3 scale(T sx, T sy, T sz) {
		mat3 ret = mat3();
		ret.data[0] = sx;
		ret.data[4] = sy;
		ret.data[8] = sz;
		return ret;
	}

	vec3<T> operator*(const vec3<T> &v3) const {
		T x, y, z;
		x = data[0] * v3.X() + data[3] * v3.Y() + data[6] * v3.Z();
		y = data[1] * v3.X() + data[4] * v3.Y() + data[7] * v3.Z();
		z = data[2] * v3.X() + data[5] * v3.Y() + data[8] * v3.Z();
		return vec3<T>(x, y, z);
	}

	T det() {
		T r1 = data[0] * (data[4] * data[8] - data[7] * data[5]);
		T r2 = (-data[3]) * (data[1] * data[8] - data[7] * data[2]);
		T r3 = data[6] * (data[1] * data[5] - data[4] * data[2]);
		return r1 + r2 + r3;
	}

	T* getData() {
		return data;
	}

	friend std::ostream &operator<<(std::ostream &stream, mat3 const &m) {
		stream << "[";
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				stream << m.data[3 * j + i] << " ";
			}
			stream << std::endl;
		}
		stream << "]";
		return stream;
	}

protected:
	T data[items];

	void init(const T* raw_data, bool transposed) {
		std::copy(raw_data, raw_data + items, data);
		if (!transposed) {
			transpose(data);
		}
	}

	static void transpose(T* data) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < i; j++) {
				std::swap(data[3 * i + j], data[3 * j + i]);
			}
		}
	}
};

template <class T>
class mat4 {
public:
	static const int items = 16;

	mat4() {
		initZeroes();
	}

	mat4(const mat4 &m2) {
		init(m2.data, false);
	}

	mat4(const T* raw_data, bool transposed) {
		init(raw_data, transposed);
	}

	mat4(mat3<T> &rs) {
		initZeroes();
		fromMat3(rs);
	}

	mat4(mat3<T> &rs, const vec3<T> &t) {
		initZeroes();
		fromMat3(rs);
		setTranslation(t);
	}

	mat4(const vec3<T> &t) {
		initZeroes();
		initIdentity();
		setTranslation(t);
	}

	mat4 transposed() {
		mat4 ret(data, true);
		transpose(ret.data);
		return ret;
	}

	mat4 inverse() {
		mat4 ret = transposed();
		return ret;
	}

	mat4 operator+(const mat4 &m2) const {
		mat4 ret;
		for (int i = 0; i < items; i++) {
			ret.data[i] = data[i] + m2.data[i];
		}
		return ret;
	}

	mat4 operator-(const mat4 &m2) const {
		mat4 ret;
		for (int i = 0; i < items; i++) {
			ret.data[i] = data[i] - m2.data[i];
		}
		return ret;
	}

	mat4 operator*(const mat4 &m2) const {
		mat4 ret = mat4();
		//FIXME: i am lazy to unroll it
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				for (int k = 0; k < 4; k++) {
					ret.data[4 * j + i] += data[4 * k + i] * m2.data[4 * j + k];
				}
			}
		}

		return ret;
	}

	static mat4 rotateX(T theta) {
		mat3<T> rx = mat3<T>::rotateX(theta);
		return mat4(rx);
	}

	static mat4 rotateY(T theta) {
		mat3<T> ry = mat3<T>::rotateX(theta);
		return mat4(ry);
	}

	static mat4 rotateZ(T theta) {
		mat3<T> rz = mat3<T>::rotateX(theta);
		return mat4(rz);
	}

	static mat4 identity() {
		mat4 ret = mat4();
		ret.initIdentity();
		return ret;
	}

	static mat4 projection(T fovy, T aspect, T z_near, T z_far) {
		mat4 ret = mat4();

		T ymax = z_near * (T)std::tan(fovy * PI / 360.0);
		T width = 2 * ymax;

		T depth = z_far - z_near;
		T d = -(z_far + z_near) / depth;
		T dn = -2 * (z_far * z_near) / depth;

		T w = 2 * z_near / width;
		T h = w * aspect;

		ret.data[0] = w;
		ret.data[5] = h;
		ret.data[10] = d;
		ret.data[11] = -1;
		ret.data[14] = dn;

		return ret;
	}

	static mat4 lookAt(const vec3<T> &eye, const vec3<T> &to,
		const vec3<T> &up) {
		mat4 ret = identity();

		vec3<T> view = to - eye;
		vec3<T> normal = (view.cross(up)).norm();
		vec3<T> new_up = (normal.cross(view)).norm();

		ret.data[0] = normal.X();
		ret.data[1] = normal.Y();
		ret.data[2] = normal.Z();

		ret.data[4] = new_up.X();
		ret.data[5] = new_up.Y();
		ret.data[6] = new_up.Z();

		ret.data[8] = -(view.X());
		ret.data[9] = -(view.Y());
		ret.data[10] = -(view.Z());

		return ret;
	}

	T* getData() {
		return data;
	}

	friend std::ostream &operator<<(std::ostream &stream, mat4 const &m) {
		stream << "[";
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				stream << m.data[4 * j + i] << " ";
			}
			stream << std::endl;
		}
		stream << "]";
		return stream;
	}

protected:
	T data[items];

	void initZeroes(void) {
		std::fill(data, data + items, (T)0.0);
	}

	void initIdentity(void) {
		data[0] = data[5] = data[10] = data[15] = 1;
	}

	void init(const T* raw_data, bool transposed) {
		std::copy(raw_data, raw_data + items, data);
		if (!transposed) {
			transpose(data);
		}
	}

	void setTranslation(const vec3<T> &t) {
		data[12] = t.X();
		data[13] = t.Y();
		data[14] = t.Z();
	}

	void fromMat3(mat3<T> &rs) {
		T* d = rs.getData();
		std::copy(d, d + 3, data);
		std::copy(d + 3, d + 6, data + 4);
		std::copy(d + 6, d + 9, data + 8);
		data[15] = 1;
	}

	static void transpose(T* data) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < i; j++) {
				std::swap(data[4 * i + j], data[4 * j + i]);
			}
		}
	}
};

typedef vec2<float> vec2f;
typedef vec3<float> vec3f;

typedef mat2<float> mat2f;
typedef mat3<float> mat3f;
typedef mat4<float> mat4f;

} //namespace vmath

#endif //__VMATH_HH__
