#include "Structs.h"
#include "AppDelegate.h"
#include <cassert>
#include <string.h>

void Mat4::createOrthographicOffCenter(float left, float right, float bottom, float top,
	float zNearPlane, float zFarPlane, Mat4* dst) {
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

void Mat4::createTranslation(float xTranslation, float yTranslation, float zTranslation, Mat4* dst) {
    assert(dst);
    memcpy(dst, &Mat4_IDENTITY, sizeof(Mat4));
    dst->m[12] = xTranslation;
    dst->m[13] = yTranslation;
    dst->m[14] = zTranslation;
}

void Mat4::createRotation(const Quaternion& q, Mat4* dst) {
    assert(dst);
    float x2 = q.x + q.x;
    float y2 = q.y + q.y;
    float z2 = q.z + q.z;

    float xx2 = q.x * x2;
    float yy2 = q.y * y2;
    float zz2 = q.z * z2;
    float xy2 = q.x * y2;
    float xz2 = q.x * z2;
    float yz2 = q.y * z2;
    float wx2 = q.w * x2;
    float wy2 = q.w * y2;
    float wz2 = q.w * z2;

    dst->m[0] = 1.0f - yy2 - zz2;
    dst->m[1] = xy2 + wz2;
    dst->m[2] = xz2 - wy2;
    dst->m[3] = 0.0f;

    dst->m[4] = xy2 - wz2;
    dst->m[5] = 1.0f - xx2 - zz2;
    dst->m[6] = yz2 + wx2;
    dst->m[7] = 0.0f;

    dst->m[8] = xz2 + wy2;
    dst->m[9] = yz2 - wx2;
    dst->m[10] = 1.0f - xx2 - yy2;
    dst->m[11] = 0.0f;

    dst->m[12] = 0.0f;
    dst->m[13] = 0.0f;
    dst->m[14] = 0.0f;
    dst->m[15] = 1.0f;
}

void Node::NodeToParentTransform() const {
    if (_transformDirty) {
        // Translate values
        float x = _position.x;
        float y = _position.y;
        float z = _positionZ;

        // Build Transform Matrix = translation * rotation * scale
        Mat4 translation;
        //move to anchor point first, then rotate
        Mat4::createTranslation(x, y, z, &translation);

        Mat4::createRotation(_rotationQuat, &_transform);

        _transform = Mat4::Multiply(translation, _transform);

        if (_scaleX != 1.f) {
            _transform[0] *= _scaleX, _transform[1] *= _scaleX, _transform[2] *= _scaleX;
        }
        if (_scaleY != 1.f) {
            _transform[4] *= _scaleY, _transform[5] *= _scaleY, _transform[6] *= _scaleY;
        }

        // adjust anchor point
        if (!_anchorPointInPoints.isZero()) {
            // FIXME:: Argh, Mat4 needs a "translate" method.
            // FIXME:: Although this is faster than multiplying a vec4 * mat4
            _transform[12] += _transform[0] * -_anchorPointInPoints.x + _transform[4] * -_anchorPointInPoints.y;
            _transform[13] += _transform[1] * -_anchorPointInPoints.x + _transform[5] * -_anchorPointInPoints.y;
            _transform[14] += _transform[2] * -_anchorPointInPoints.x + _transform[6] * -_anchorPointInPoints.y;
        }
    }

    if (_additionalTransform) {
        // This is needed to support both Node::setNodeToParentTransform() and Node::setAdditionalTransform()
        // at the same time. The scenario is this:
        // at some point setNodeToParentTransform() is called.
        // and later setAdditionalTransform() is called every time. And since _transform
        // is being overwritten everyframe, _additionalTransform[1] is used to have a copy
        // of the last "_transform without _additionalTransform"
        if (_transformDirty)
            _additionalTransform[1] = _transform;

        if (_transformUpdated)
            _transform = Mat4::Multiply(_additionalTransform[1], _additionalTransform[0]);
    }

    _transformDirty = _additionalTransformDirty = false;
}

void Node::Render(const Mat4* eyeTransforms, const Mat4* eyeProjections, uint32_t multiViewCount) {
    // todo
    Camera* defaultCamera = nullptr;
    //const auto& transform = getNodeToParentTransform();
    NodeToParentTransform();
    gAppDelegate->_projectionMatrixStackList[0].push(
        gAppDelegate->_projectionMatrixStackList[0].top());
}
