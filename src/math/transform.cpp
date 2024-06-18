#include "transform.h"

#include "common/sc_assert.h"
#include "common/sc_delete.h"

namespace scythe {

	int Transform::suspend_transform_changed_(0);
	std::vector<Transform*> Transform::transforms_changed_;

	Transform::Transform()
	: matrix_dirty_bits_(0)
	, listeners_(nullptr)
	{
		scale_.Set(Vector3::One());
	}

	Transform::Transform(const Vector3& scale, const Quaternion& rotation, const Vector3& translation)
	: matrix_dirty_bits_(0)
	, listeners_(nullptr)
	{
		Set(scale, rotation, translation);
	}

	Transform::Transform(const Vector3& scale, const Matrix4& rotation, const Vector3& translation)
	: matrix_dirty_bits_(0)
	, listeners_(nullptr)
	{
		Set(scale, rotation, translation);
	}

	Transform::Transform(const Transform& copy)
	: matrix_dirty_bits_(0)
	, listeners_(nullptr)
	{
		Set(copy);
	}

	Transform::~Transform()
	{
		SC_SAFE_DELETE(listeners_);
	}

	void Transform::SuspendTransformChanged()
	{
		++suspend_transform_changed_;
	}

	void Transform::ResumeTransformChanged()
	{
		if (suspend_transform_changed_ == 0) // We haven't suspended transformChanged() calls, so do nothing.
			return;
		
		if (suspend_transform_changed_ == 1)
		{
			// Call TransformChanged() on all transforms in the list
			std::size_t transformCount = transforms_changed_.size();
			for (std::size_t i = 0; i < transformCount; i++)
			{
				Transform* t = transforms_changed_.at(i);
				SC_ASSERT(t);
				t->TransformChanged();
			}

			// Go through list and reset DIRTY_NOTIFY bit. The list could potentially be larger here if the 
			// transforms we were delaying calls to transformChanged() have any child nodes.
			transformCount = transforms_changed_.size();
			for (std::size_t i = 0; i < transformCount; i++)
			{
				Transform* t = transforms_changed_.at(i);
				SC_ASSERT(t);
				t->matrix_dirty_bits_ &= ~DIRTY_NOTIFY;
			}

			// empty list for next frame.
			transforms_changed_.clear();
		}
		--suspend_transform_changed_;
	}

	bool Transform::IsTransformChangedSuspended()
	{
		return (suspend_transform_changed_ > 0);
	}

	const char* Transform::GetTypeName() const
	{
		return "Transform";
	}

	const Matrix4& Transform::GetMatrix() const
	{
		if (matrix_dirty_bits_ & (DIRTY_TRANSLATION | DIRTY_ROTATION | DIRTY_SCALE))
		{
			if (!IsStatic())
			{
				bool hasScale = !scale_.IsOne();
				bool hasRotation = !rotation_.IsIdentity();

				// Compose the matrix in TRS order since we use column-major matrices with column vectors and
				// multiply M*v (as opposed to XNA and DirectX that use row-major matrices with row vectors and multiply v*M).
				Matrix4::CreateTranslation(translation_, &matrix_);
				if (hasRotation)
				{
					matrix_.Rotate(rotation_);
				}
				if (hasScale)
				{
					matrix_.Scale(scale_);
				}
			}

			matrix_dirty_bits_ &= ~(DIRTY_TRANSLATION | DIRTY_ROTATION | DIRTY_SCALE);
		}

		return matrix_;
	}

	const Vector3& Transform::GetScale() const
	{
		return scale_;
	}

	void Transform::GetScale(Vector3* scale) const
	{
		SC_ASSERT(scale);
		scale->Set(scale_);
	}

	float Transform::GetScaleX() const
	{
		return scale_.x;
	}

	float Transform::GetScaleY() const
	{
		return scale_.y;
	}

	float Transform::GetScaleZ() const
	{
		return scale_.z;
	}

	const Quaternion& Transform::GetRotation() const
	{
		return rotation_;
	}

	void Transform::GetRotation(Quaternion* rotation) const
	{
		SC_ASSERT(rotation);
		rotation->Set(rotation_);
	}

	void Transform::GetRotation(Matrix4* rotation) const
	{
		SC_ASSERT(rotation);
		Matrix4::CreateRotation(rotation_, rotation);
	}

	float Transform::GetRotation(Vector3* axis) const
	{
		SC_ASSERT(axis);
		return rotation_.ToAxisAngle(axis);
	}

	const Vector3& Transform::GetTranslation() const
	{
		return translation_;
	}

	void Transform::GetTranslation(Vector3* translation) const
	{
		SC_ASSERT(translation);
		translation->Set(translation_);
	}

	float Transform::GetTranslationX() const
	{
		return translation_.x;
	}

	float Transform::GetTranslationY() const
	{
		return translation_.y;
	}

	float Transform::GetTranslationZ() const
	{
		return translation_.z;
	}

	Vector3 Transform::GetForwardVector() const
	{
		Vector3 v;
		GetForwardVector(&v);
		return v;
	}

	void Transform::GetForwardVector(Vector3* dst) const
	{
		GetMatrix().GetForwardVector(dst);
	}

	Vector3 Transform::GetBackVector() const
	{
		Vector3 v;
		GetBackVector(&v);
		return v;
	}

	void Transform::GetBackVector(Vector3* dst) const
	{
		GetMatrix().GetBackVector(dst);
	}

	Vector3 Transform::GetUpVector() const
	{
		Vector3 v;
		GetUpVector(&v);
		return v;
	}

	void Transform::GetUpVector(Vector3* dst) const
	{
		GetMatrix().GetUpVector(dst);
	}

	Vector3 Transform::GetDownVector() const
	{
		Vector3 v;
		GetDownVector(&v);
		return v;
	}

	void Transform::GetDownVector(Vector3* dst) const
	{
		GetMatrix().GetDownVector(dst);
	}

	Vector3 Transform::GetLeftVector() const
	{
		Vector3 v;
		GetLeftVector(&v);
		return v;
	}

	void Transform::GetLeftVector(Vector3* dst) const
	{
		GetMatrix().GetLeftVector(dst);
	}

	Vector3 Transform::GetRightVector() const
	{
		Vector3 v;
		GetRightVector(&v);
		return v;
	}

	void Transform::GetRightVector(Vector3* dst) const
	{
		GetMatrix().GetRightVector(dst);
	}

	void Transform::Rotate(float qx, float qy, float qz, float qw)
	{
		if (IsStatic())
			return;

		Quaternion q(qx, qy, qz, qw);
		rotation_.Multiply(q);
		Dirty(DIRTY_ROTATION);
	}

	void Transform::Rotate(const Quaternion& rotation)
	{
		if (IsStatic())
			return;

		rotation_.Multiply(rotation);
		Dirty(DIRTY_ROTATION);
	}

	void Transform::Rotate(const Vector3& axis, float angle)
	{
		if (IsStatic())
			return;

		Quaternion rotationQuat;
		Quaternion::CreateFromAxisAngle(axis, angle, &rotationQuat);
		rotation_.Multiply(rotationQuat);
		rotation_.Normalize();
		Dirty(DIRTY_ROTATION);
	}

	void Transform::Rotate(const Matrix4& rotation)
	{
		if (IsStatic())
			return;

		Quaternion rotationQuat;
		Quaternion::CreateFromRotationMatrix(rotation, &rotationQuat);
		rotation_.Multiply(rotationQuat);
		Dirty(DIRTY_ROTATION);
	}

	void Transform::RotateX(float angle)
	{
		if (IsStatic())
			return;

		Quaternion rotationQuat;
		Quaternion::CreateFromAxisAngle(Vector3::UnitX(), angle, &rotationQuat);
		rotation_.Multiply(rotationQuat);
		Dirty(DIRTY_ROTATION);
	}

	void Transform::RotateY(float angle)
	{
		if (IsStatic())
			return;

		Quaternion rotationQuat;
		Quaternion::CreateFromAxisAngle(Vector3::UnitY(), angle, &rotationQuat);
		rotation_.Multiply(rotationQuat);
		Dirty(DIRTY_ROTATION);
	}

	void Transform::RotateZ(float angle)
	{
		if (IsStatic())
			return;

		Quaternion rotationQuat;
		Quaternion::CreateFromAxisAngle(Vector3::UnitZ(), angle, &rotationQuat);
		rotation_.Multiply(rotationQuat);
		Dirty(DIRTY_ROTATION);
	}

	void Transform::Scale(float scale)
	{
		if (IsStatic())
			return;

		scale_.Scale(scale);
		Dirty(DIRTY_SCALE);
	}

	void Transform::Scale(float sx, float sy, float sz)
	{
		if (IsStatic())
			return;

		scale_.x *= sx;
		scale_.y *= sy;
		scale_.z *= sz;
		Dirty(DIRTY_SCALE);
	}

	void Transform::Scale(const Vector3& scale)
	{
		if (IsStatic())
			return;

		scale_.x *= scale.x;
		scale_.y *= scale.y;
		scale_.z *= scale.z;
		Dirty(DIRTY_SCALE);
	}

	void Transform::ScaleX(float sx)
	{
		if (IsStatic())
			return;

		scale_.x *= sx;
		Dirty(DIRTY_SCALE);
	}

	void Transform::ScaleY(float sy)
	{
		if (IsStatic())
			return;

		scale_.y *= sy;
		Dirty(DIRTY_SCALE);
	}

	void Transform::ScaleZ(float sz)
	{
		if (IsStatic())
			return;

		scale_.z *= sz;
		Dirty(DIRTY_SCALE);
	}

	void Transform::Set(const Vector3& scale, const Quaternion& rotation, const Vector3& translation)
	{
		if (IsStatic())
			return;

		scale_.Set(scale);
		rotation_.Set(rotation);
		translation_.Set(translation);
		Dirty(DIRTY_TRANSLATION | DIRTY_ROTATION | DIRTY_SCALE);
	}

	void Transform::Set(const Vector3& scale, const Matrix4& rotation, const Vector3& translation)
	{
		if (IsStatic())
			return;

		scale_.Set(scale);
		Quaternion rotationQuat;
		Quaternion::CreateFromRotationMatrix(rotation, &rotationQuat);
		rotation_.Set(rotationQuat);
		translation_.Set(translation);
		Dirty(DIRTY_TRANSLATION | DIRTY_ROTATION | DIRTY_SCALE);
	}

	void Transform::Set(const Vector3& scale, const Vector3& axis, float angle, const Vector3& translation)
	{
		if (IsStatic())
			return;

		scale_.Set(scale);
		rotation_.Set(axis, angle);
		translation_.Set(translation);
		Dirty(DIRTY_TRANSLATION | DIRTY_ROTATION | DIRTY_SCALE);
	}

	void Transform::Set(const Transform& transform)
	{
		if (IsStatic())
			return;

		scale_.Set(transform.scale_);
		rotation_.Set(transform.rotation_);
		translation_.Set(transform.translation_);
		Dirty(DIRTY_TRANSLATION | DIRTY_ROTATION | DIRTY_SCALE);
	}

	void Transform::SetIdentity()
	{
		if (IsStatic())
			return;

		scale_.Set(1.0f, 1.0f, 1.0f);
		rotation_.SetIdentity();
		translation_.Set(0.0f, 0.0f, 0.0f);
		Dirty(DIRTY_TRANSLATION | DIRTY_ROTATION | DIRTY_SCALE);
	}

	void Transform::SetScale(float scale)
	{
		if (IsStatic())
			return;

		scale_.Set(scale, scale, scale);
		Dirty(DIRTY_SCALE);
	}

	void Transform::SetScale(float sx, float sy, float sz)
	{
		if (IsStatic())
			return;

		scale_.Set(sx, sy, sz);
		Dirty(DIRTY_SCALE);
	}

	void Transform::SetScale(const Vector3& scale)
	{
		scale_.Set(scale);
		Dirty(DIRTY_SCALE);
	}

	void Transform::SetScaleX(float sx)
	{
		if (IsStatic())
			return;

		scale_.x = sx;
		Dirty(DIRTY_SCALE);
	}

	void Transform::SetScaleY(float sy)
	{
		if (IsStatic())
			return;

		scale_.y = sy;
		Dirty(DIRTY_SCALE);
	}

	void Transform::SetScaleZ(float sz)
	{
		if (IsStatic())
			return;

		scale_.z = sz;
		Dirty(DIRTY_SCALE);
	}

	void Transform::SetRotation(const Quaternion& rotation)
	{
		if (IsStatic())
			return;

		rotation_.Set(rotation);
		Dirty(DIRTY_ROTATION);
	}

	void Transform::SetRotation(float qx, float qy, float qz, float qw)
	{
		if (IsStatic())
			return;

		rotation_.Set(qx, qy, qz, qw);
		Dirty(DIRTY_ROTATION);
	}

	void Transform::SetRotation(const Matrix4& rotation)
	{
		if (IsStatic())
			return;

		Quaternion rotationQuat;
		Quaternion::CreateFromRotationMatrix(rotation, &rotationQuat);
		rotation_.Set(rotationQuat);
		Dirty(DIRTY_ROTATION);
	}

	void Transform::SetRotation(const Vector3& axis, float angle)
	{
		if (IsStatic())
			return;

		rotation_.Set(axis, angle);
		Dirty(DIRTY_ROTATION);
	}

	void Transform::SetTranslation(const Vector3& translation)
	{
		if (IsStatic())
			return;

		translation_.Set(translation);
		Dirty(DIRTY_TRANSLATION);
	}

	void Transform::SetTranslation(float tx, float ty, float tz)
	{
		if (IsStatic())
			return;

		translation_.Set(tx, ty, tz);
		Dirty(DIRTY_TRANSLATION);
	}

	void Transform::SetTranslationX(float tx)
	{
		if (IsStatic())
			return;

		translation_.x = tx;
		Dirty(DIRTY_TRANSLATION);
	}

	void Transform::SetTranslationY(float ty)
	{
		if (IsStatic())
			return;

		translation_.y = ty;
		Dirty(DIRTY_TRANSLATION);
	}

	void Transform::SetTranslationZ(float tz)
	{
		if (IsStatic())
			return;

		translation_.z = tz;
		Dirty(DIRTY_TRANSLATION);
	}

	void Transform::Translate(float tx, float ty, float tz)
	{
		if (IsStatic())
			return;

		translation_.x += tx;
		translation_.y += ty;
		translation_.z += tz;
		Dirty(DIRTY_TRANSLATION);
	}

	void Transform::Translate(const Vector3& translation)
	{
		if (IsStatic())
			return;

		translation_.x += translation.x;
		translation_.y += translation.y;
		translation_.z += translation.z;
		Dirty(DIRTY_TRANSLATION);
	}

	void Transform::TranslateX(float tx)
	{
		if (IsStatic())
			return;

		translation_.x += tx;
		Dirty(DIRTY_TRANSLATION);
	}

	void Transform::TranslateY(float ty)
	{
		if (IsStatic())
			return;

		translation_.y += ty;
		Dirty(DIRTY_TRANSLATION);
	}

	void Transform::TranslateZ(float tz)
	{
		if (IsStatic())
			return;

		translation_.z += tz;
		Dirty(DIRTY_TRANSLATION);
	}

	void Transform::TranslateLeft(float amount)
	{
		if (IsStatic())
			return;

		// Force the current transform matrix to be updated.
		GetMatrix();

		Vector3 left;
		matrix_.GetLeftVector(&left);
		left.Normalize();
		left.Scale(amount);

		Translate(left);
	}

	void Transform::TranslateUp(float amount)
	{
		if (IsStatic())
			return;

		// Force the current transform matrix to be updated.
		GetMatrix();

		Vector3 up;
		matrix_.GetUpVector(&up);
		up.Normalize();
		up.Scale(amount);

		Translate(up);
	}

	void Transform::TranslateForward(float amount)
	{
		if (IsStatic())
			return;

		// Force the current transform matrix to be updated.
		GetMatrix();

		Vector3 forward;
		matrix_.GetForwardVector(&forward);
		forward.Normalize();
		forward.Scale(amount);

		Translate(forward);
	}

	void Transform::TranslateSmooth(const Vector3& target, float elapsedTime, float responseTime)
	{
		if (IsStatic())
			return;

		if (elapsedTime > 0.0f)
		{
			translation_ += (target - translation_) * (elapsedTime / (elapsedTime + responseTime));
			Dirty(DIRTY_TRANSLATION);
		}
	}

	void Transform::TransformPoint(Vector3* point)
	{
		GetMatrix();
		matrix_.TransformPoint(point);
	}

	void Transform::TransformPoint(const Vector3& point, Vector3* dst)
	{
		GetMatrix();
		matrix_.TransformPoint(point, dst);
	}

	void Transform::TransformVector(Vector3* normal)
	{
		GetMatrix();
		matrix_.TransformVector(normal);
	}

	void Transform::TransformVector(const Vector3& normal, Vector3* dst)
	{
		GetMatrix();
		matrix_.TransformVector(normal, dst);
	}

	void Transform::TransformVector(float x, float y, float z, float w, Vector3* dst)
	{
		GetMatrix();
		matrix_.TransformVector(x, y, z, w, dst);
	}

	bool Transform::IsStatic() const
	{
		return false;
	}

	/*
	unsigned int Transform::getAnimationPropertyComponentCount(int propertyId) const
	{
		switch (propertyId)
		{
			case ANIMATE_SCALE_UNIT:
			case ANIMATE_SCALE_X:
			case ANIMATE_SCALE_Y:
			case ANIMATE_SCALE_Z:
			case ANIMATE_TRANSLATE_X:
			case ANIMATE_TRANSLATE_Y:
			case ANIMATE_TRANSLATE_Z:
				return 1;
			case ANIMATE_SCALE:
			case ANIMATE_TRANSLATE:
				return 3;
			case ANIMATE_ROTATE:
				return 4;
			case ANIMATE_SCALE_TRANSLATE:
				return 6;
			case ANIMATE_ROTATE_TRANSLATE:
			case ANIMATE_SCALE_ROTATE:
				return 7;
			case ANIMATE_SCALE_ROTATE_TRANSLATE:
				return 10;
			default:
				return -1;
		}
	}

	void Transform::getAnimationPropertyValue(int propertyId, AnimationValue* value)
	{
		SC_ASSERT(value);

		switch (propertyId)
		{
			case ANIMATE_SCALE_UNIT:
				value->setFloat(0, scale_.x);
				break;
			case ANIMATE_SCALE:
				value->setFloats(0, &scale_.x, 3);
				break;
			case ANIMATE_SCALE_X:
				value->setFloat(0, scale_.x);
				break;
			case ANIMATE_SCALE_Y:
				value->setFloat(0, scale_.y);
				break;
			case ANIMATE_SCALE_Z:
				value->setFloat(0, scale_.z);
				break;
			case ANIMATE_ROTATE:
				value->setFloats(0, &rotation_.x, 4);
				break;
			case ANIMATE_TRANSLATE:
				value->setFloats(0, &translation_.x, 3);
				break;
			case ANIMATE_TRANSLATE_X:
				value->setFloat(0, translation_.x);
				break;
			case ANIMATE_TRANSLATE_Y:
				value->setFloat(0, translation_.y);
				break;
			case ANIMATE_TRANSLATE_Z:
				value->setFloat(0, translation_.z);
				break;
			case ANIMATE_ROTATE_TRANSLATE:
				value->setFloats(0, &rotation_.x, 4);
				value->setFloats(4, &translation_.x, 3);
				break;
			case ANIMATE_SCALE_ROTATE:
				value->setFloats(0, &scale_.x, 3);
				value->setFloats(3, &rotation_.x, 4);
				break;
			case ANIMATE_SCALE_TRANSLATE:
				value->setFloats(0, &scale_.x, 3);
				value->setFloats(3, &translation_.x, 3);
				break;
			case ANIMATE_SCALE_ROTATE_TRANSLATE:
				value->setFloats(0, &scale_.x, 3);
				value->setFloats(3, &rotation_.x, 4);
				value->setFloats(7, &translation_.x, 3);
				break;
			default:
				break;
		}
	}

	void Transform::setAnimationPropertyValue(int propertyId, AnimationValue* value, float blendWeight)
	{
		SC_ASSERT(value);
		SC_ASSERT(blendWeight >= 0.0f && blendWeight <= 1.0f);

		switch (propertyId)
		{
			case ANIMATE_SCALE_UNIT:
			{
				float scale = Curve::lerp(blendWeight, scale_.x, value->getFloat(0));
				setScale(scale);
				break;
			}   
			case ANIMATE_SCALE:
			{
				setScale(Curve::lerp(blendWeight, scale_.x, value->getFloat(0)), Curve::lerp(blendWeight, scale_.y, value->getFloat(1)), Curve::lerp(blendWeight, scale_.z, value->getFloat(2)));
				break;
			}
			case ANIMATE_SCALE_X:
			{
				setScaleX(Curve::lerp(blendWeight, scale_.x, value->getFloat(0)));
				break;
			}
			case ANIMATE_SCALE_Y:
			{
				setScaleY(Curve::lerp(blendWeight, scale_.y, value->getFloat(0)));
				break;
			}
			case ANIMATE_SCALE_Z:
			{
				setScaleZ(Curve::lerp(blendWeight, scale_.z, value->getFloat(0)));
				break;
			}
			case ANIMATE_ROTATE:
			{
				applyAnimationValueRotation(value, 0, blendWeight);
				break;
			}
			case ANIMATE_TRANSLATE:
			{
				setTranslation(Curve::lerp(blendWeight, translation_.x, value->getFloat(0)), Curve::lerp(blendWeight, translation_.y, value->getFloat(1)), Curve::lerp(blendWeight, translation_.z, value->getFloat(2)));
				break;
			}
			case ANIMATE_TRANSLATE_X:
			{
				setTranslationX(Curve::lerp(blendWeight, translation_.x, value->getFloat(0)));
				break;
			}
			case ANIMATE_TRANSLATE_Y:
			{
				setTranslationY(Curve::lerp(blendWeight, translation_.y, value->getFloat(0)));
				break;
			}
			case ANIMATE_TRANSLATE_Z:
			{
				setTranslationZ(Curve::lerp(blendWeight, translation_.z, value->getFloat(0)));
				break;
			}
			case ANIMATE_ROTATE_TRANSLATE:
			{
				applyAnimationValueRotation(value, 0, blendWeight);
				setTranslation(Curve::lerp(blendWeight, translation_.x, value->getFloat(4)), Curve::lerp(blendWeight, translation_.y, value->getFloat(5)), Curve::lerp(blendWeight, translation_.z, value->getFloat(6)));
				break;
			}
			case ANIMATE_SCALE_ROTATE:
			{
				setScale(Curve::lerp(blendWeight, scale_.x, value->getFloat(0)), Curve::lerp(blendWeight, scale_.y, value->getFloat(1)), Curve::lerp(blendWeight, scale_.z, value->getFloat(2)));
				applyAnimationValueRotation(value, 3, blendWeight);
				break;
			}
			case ANIMATE_SCALE_TRANSLATE:
			{
				setScale(Curve::lerp(blendWeight, scale_.x, value->getFloat(0)), Curve::lerp(blendWeight, scale_.y, value->getFloat(1)), Curve::lerp(blendWeight, scale_.z, value->getFloat(2)));
				setTranslation(Curve::lerp(blendWeight, translation_.x, value->getFloat(3)), Curve::lerp(blendWeight, translation_.y, value->getFloat(4)), Curve::lerp(blendWeight, translation_.z, value->getFloat(5)));
				break;
			}
			case ANIMATE_SCALE_ROTATE_TRANSLATE:
			{
				setScale(Curve::lerp(blendWeight, scale_.x, value->getFloat(0)), Curve::lerp(blendWeight, scale_.y, value->getFloat(1)), Curve::lerp(blendWeight, scale_.z, value->getFloat(2)));
				applyAnimationValueRotation(value, 3, blendWeight);
				setTranslation(Curve::lerp(blendWeight, translation_.x, value->getFloat(7)), Curve::lerp(blendWeight, translation_.y, value->getFloat(8)), Curve::lerp(blendWeight, translation_.z, value->getFloat(9)));
				break;
			}
			default:
				break;
		}
	}
	*/

	void Transform::Dirty(char matrixDirtyBits)
	{
		matrix_dirty_bits_ |= matrixDirtyBits;
		if (IsTransformChangedSuspended())
		{
			if (!IsDirty(DIRTY_NOTIFY))
			{
				SuspendTransformChange(this);
			}
		}
		else
		{
			TransformChanged();
		}
	}

	bool Transform::IsDirty(char matrixDirtyBits) const
	{
		return (matrix_dirty_bits_ & matrixDirtyBits) == matrixDirtyBits;
	}

	void Transform::SuspendTransformChange(Transform* transform)
	{
		SC_ASSERT(transform);
		transform->matrix_dirty_bits_ |= DIRTY_NOTIFY;
		transforms_changed_.push_back(transform);
	}

	void Transform::AddListener(Transform::Listener* listener, long cookie)
	{
		SC_ASSERT(listener);

		if (listeners_ == nullptr)
			listeners_ = new std::list<TransformListener>();

		TransformListener l;
		l.listener = listener;
		l.cookie = cookie;
		listeners_->push_back(l);
	}

	void Transform::RemoveListener(Transform::Listener* listener)
	{
		SC_ASSERT(listener);

		if (listeners_)
		{
			for (std::list<TransformListener>::iterator itr = listeners_->begin(); itr != listeners_->end(); ++itr)
			{
				if ((*itr).listener == listener)
				{
					listeners_->erase(itr);
					break;
				}
			}
		}
	}

	void Transform::TransformChanged()
	{
		if (listeners_)
		{
			for (std::list<TransformListener>::iterator itr = listeners_->begin(); itr != listeners_->end(); ++itr)
			{
				TransformListener& l = *itr;
				SC_ASSERT(l.listener);
				l.listener->TransformChanged(this, l.cookie);
			}
		}
		//fireScriptEvent<void>(GP_GET_SCRIPT_EVENT(Transform, transformChanged), dynamic_cast<void*>(this));
	}

	/*
	void Transform::applyAnimationValueRotation(AnimationValue* value, unsigned int index, float blendWeight)
	{
		if (isStatic())
			return;

		SC_ASSERT(value);
		Quaternion::slerp(rotation_.x, rotation_.y, rotation_.z, rotation_.w, value->getFloat(index), value->getFloat(index + 1), value->getFloat(index + 2), value->getFloat(index + 3), blendWeight, 
			&rotation_.x, &rotation_.y, &rotation_.z, &rotation_.w);
		dirty(DIRTY_ROTATION);
	}
	*/

} // namespace scythe