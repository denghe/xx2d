#pragma once
#include "pch.h"

// for CCPoint assignement operator and copy constructor
struct CCSize;
struct CCPoint {
	float x;
	float y;

	CCPoint() : x(0), y(0) {}
	CCPoint(float x, float y) : x(x), y(y) {}
	CCPoint(const CCPoint& other) : x(other.x), y(other.y) {}
	CCPoint(const CCSize& size);
	inline CCPoint& operator=(const CCPoint& other) {
		setPoint(other.x, other.y);
		return *this;
	}
	CCPoint& operator=(const CCSize& size);
	inline CCPoint operator+(const CCPoint& right) const {
		return CCPoint(this->x + right.x, this->y + right.y);
	}
	inline CCPoint operator-(const CCPoint& right) const {
		return CCPoint(this->x - right.x, this->y - right.y);
	}
	inline CCPoint operator-() const {
		return CCPoint(-x, -y);
	}
	inline CCPoint operator*(float a) const {
		return CCPoint(this->x * a, this->y * a);
	}
	inline CCPoint operator/(float a) const {
		return CCPoint(this->x / a, this->y / a);
	}
	inline void setPoint(float x, float y) {
		this->x = x;
		this->y = y;
	}
	inline bool equals(const CCPoint& target) const {
		return (fabs(this->x - target.x) < FLT_EPSILON)
			&& (fabs(this->y - target.y) < FLT_EPSILON);
	}


	// returns if points have fuzzy equality which means equal with some degree of variance.
	inline bool fuzzyEquals(const CCPoint& target, float variance) const {
		return ((x - variance <= target.x && target.x <= x + variance))
			&& ((y - variance <= target.y && target.y <= y + variance));
	}

	// Calculates distance between point an origin
	inline float getLength() const {
		return sqrtf(x * x + y * y);
	};

	// Calculates the square length of a CCPoint (not calling sqrt() )
	inline float getLengthSq() const {
		return dot(*this); //x*x + y*y;
	};

	// Calculates the square distance between two points (not calling sqrt() )
	inline float getDistanceSq(const CCPoint& other) const {
		return (*this - other).getLengthSq();
	};

	// Calculates the distance between two points
	inline float getDistance(const CCPoint& other) const {
		return (*this - other).getLength();
	};

	// returns the angle in radians between this vector and the x axis
	inline float getAngle() const {
		return atan2f(y, x);
	};

	// returns the angle in radians between two vector directions
	inline float getAngle(const CCPoint& other) const {
		CCPoint a2 = normalize();
		CCPoint b2 = other.normalize();
		float angle = atan2f(a2.cross(b2), a2.dot(b2));
		if (fabs(angle) < FLT_EPSILON) return 0.f;
		return angle;
	}

	// Calculates dot product of two points.
	inline float dot(const CCPoint& other) const {
		return x * other.x + y * other.y;
	};

	// Calculates cross product of two points.
	inline float cross(const CCPoint& other) const {
		return x * other.y - y * other.x;
	};

	// Calculates perpendicular of v, rotated 90 degrees counter-clockwise -- cross(v, perp(v)) >= 0
	inline CCPoint getPerp() const {
		return CCPoint(-y, x);
	};

	// Calculates perpendicular of v, rotated 90 degrees clockwise -- cross(v, rperp(v)) <= 0
	inline CCPoint getRPerp() const {
		return CCPoint(y, -x);
	};

	// Calculates the projection of this over other.
	inline CCPoint project(const CCPoint& other) const {
		return other * (dot(other) / other.dot(other));
	};

	// Complex multiplication of two points ("rotates" two points).
	// return CCPoint vector with an angle of this.getAngle() + other.getAngle(), and a length of this.getLength() * other.getLength().
	inline CCPoint rotate(const CCPoint& other) const {
		return CCPoint(x * other.x - y * other.y, x * other.y + y * other.x);
	};

	// Unrotates two points.
	// return CCPoint vector with an angle of this.getAngle() - other.getAngle(), and a length of this.getLength() * other.getLength().
	inline CCPoint unrotate(const CCPoint& other) const {
		return CCPoint(x * other.x + y * other.y, y * other.x - x * other.y);
	};

	// Returns point multiplied to a length of 1. If the point is 0, it returns (1, 0)
	inline CCPoint normalize() const {
		float length = getLength();
		if (length == 0.) return CCPoint(1.f, 0);
		return *this / getLength();
	};

	// Linear Interpolation between two points a and b. returns alpha == 0 ? a    alpha == 1 ? b   otherwise a value between a..b
	inline CCPoint lerp(const CCPoint& other, float alpha) const {
		return *this * (1.f - alpha) + other * alpha;
	};

	// Rotates a point counter clockwise by the angle around a pivot
	// param pivot is the pivot, naturally
	// param angle is the angle of rotation ccw in radians
	// returns the rotated point
	inline CCPoint rotateByAngle(const CCPoint& pivot, float angle) const {
		return pivot + (*this - pivot).rotate(CCPoint::forAngle(angle));
	}

	static inline CCPoint forAngle(const float a) {
		return CCPoint(cosf(a), sinf(a));
	}
};

struct CCSize {
	float width;
	float height;

	CCSize() : width(0), height(0) {}
	CCSize(float width, float height) : width(width), height(height) {}
	CCSize(const CCSize& other) : width(other.width), height(other.height) {}
	CCSize(const CCPoint& point) : width(point.x), height(point.y) {}
	inline CCSize& operator=(const CCSize& other) {
		setSize(other.width, other.height);
		return *this;
	}
	inline CCSize& operator=(const CCPoint& point) {
		setSize(point.x, point.y);
		return *this;
	}
	inline CCSize operator+(const CCSize& right) const {
		return CCSize(this->width + right.width, this->height + right.height);
	}
	inline CCSize operator-(const CCSize& right) const {
		return CCSize(this->width - right.width, this->height - right.height);
	}
	inline CCSize operator*(float a) const {
		return CCSize(this->width * a, this->height * a);
	}
	inline CCSize operator/(float a) const {
		return CCSize(this->width / a, this->height / a);
	}
	inline void setSize(float width, float height) {
		this->width = width;
		this->height = height;
	}
	inline bool equals(const CCSize& target) const {
		return (fabs(this->width - target.width) < FLT_EPSILON)
			&& (fabs(this->height - target.height) < FLT_EPSILON);
	}
};

inline CCPoint::CCPoint(const CCSize& size) : x(size.width), y(size.height) {}

inline CCPoint& CCPoint::operator=(const CCSize& size) {
	setPoint(size.width, size.height);
	return *this;
}


struct CCRect {
	CCPoint origin;
	CCSize  size;

	CCRect() = default;
	CCRect(float x, float y, float width, float height) {
		setRect(x, y, width, height);
	}
	CCRect(const CCRect& other) {
		setRect(other.origin.x, other.origin.y, other.size.width, other.size.height);
	}
	inline CCRect& operator=(const CCRect& other) {
		setRect(other.origin.x, other.origin.y, other.size.width, other.size.height);
		return *this;
	}
	inline void setRect(float x, float y, float width, float height) {
		origin.x = x;
		origin.y = y;

		size.width = width;
		size.height = height;
	}
	inline float getMinX() const { return origin.x; } /// return the leftmost x-value of current rect
	inline float getMidX() const { return (float)(origin.x + size.width / 2.0); } /// return the midpoint x-value of current rect
	inline float getMaxX() const { return (float)(origin.x + size.width); } /// return the rightmost x-value of current rect
	inline float getMinY() const { return origin.y; } /// return the bottommost y-value of current rect
	inline float getMidY() const { return (float)(origin.y + size.height / 2.0); } /// return the midpoint y-value of current rect
	inline float getMaxY() const { return origin.y + size.height; } /// return the topmost y-value of current rect
	inline bool equals(const CCRect& rect) const {
		return (origin.equals(rect.origin) &&
			size.equals(rect.size));
	}
	inline bool containsPoint(const CCPoint& point) const {
		return point.x >= getMinX() && point.x <= getMaxX()
			&& point.y >= getMinY() && point.y <= getMaxY();
	}
	bool intersectsRect(const CCRect& rect) const {
		return !(getMaxX() < rect.getMinX() ||
			rect.getMaxX() < getMinX() ||
			getMaxY() < rect.getMinY() ||
			rect.getMaxY() < getMinY());
	}
};

#define CCPointMake(x, y) CCPoint((float)(x), (float)(y))
#define CCSizeMake(width, height) CCSize((float)(width), (float)(height))
#define CCRectMake(x, y, width, height) CCRect((float)(x), (float)(y), (float)(width), (float)(height))


const CCPoint CCPointZero;

/* The "zero" size -- equivalent to CCSizeMake(0, 0). */
const CCSize CCSizeZero;

/* The "zero" rectangle -- equivalent to CCRectMake(0, 0, 0, 0). */
const CCRect CCRectZero;




enum class ResolutionPolicy{
	// The entire application is visible in the specified area without trying to preserve the original aspect ratio.
	// Distortion can occur, and the application may appear stretched or compressed.
	ExactFit,
	// The entire application fills the specified area, without distortion but possibly with some cropping,
	// while maintaining the original aspect ratio of the application.
	NoBorder,
	// The entire application is visible in the specified area without distortion while maintaining the original
	// aspect ratio of the application. Borders can appear on two sides of the application.
	ShowAll,
	// The application takes the height of the design resolution size and modifies the width of the internal
	// canvas so that it fits the aspect ratio of the device
	// no distortion will occur however you must make sure your application works on different
	// aspect ratios
	FixedHeight,
	// The application takes the width of the design resolution size and modifies the height of the internal
	// canvas so that it fits the aspect ratio of the device
	// no distortion will occur however you must make sure your application works on different
	// aspect ratios
	FixedWidth,
};


#ifndef NDEBUG
#define CHECK_GL_ERROR_DEBUG() \
    do { \
        GLenum __error = glGetError(); \
        if(__error) { \
            throw std::runtime_error(std::string("OpenGL error 0x%04X in %s %s %d\n") + std::to_string(__error) + __FILE__ + __FUNCTION__ +  std::to_string(__LINE__)); \
        } \
    } while (false)
#else
#define CHECK_GL_ERROR_DEBUG()
#endif
