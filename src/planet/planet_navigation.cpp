#include "planet_navigation.h"

#include "camera.h"
#include "math/vector2.h"
#include "math/common_math.h"

#include <cmath>

namespace scythe {

	PlanetNavigation::PlanetNavigation(CameraManager * camera_manager, const Vector3& position, float planet_radius,
			float animation_time, float farest_distance, float nearest_distance)
	: camera_manager_(camera_manager)
	, planet_position_(position)
	, planet_radius_(planet_radius)
	, animation_time_(animation_time)
	, current_scale_index_(0)
	, is_pan_mode_(false)
	{
		num_scales_ = static_cast<int>(log(farest_distance/nearest_distance)/log(2)) + 1;
		distances_ = new float[num_scales_];
		float distance = farest_distance;
		for (int i = 0; i < num_scales_; ++i)
		{
			distances_[i] = distance;
			distance *= 0.5f;
		}
		// Make static camera at farest point
		camera_manager_->MakeFree(MakePoint(farest_distance), planet_position_);
	}
	PlanetNavigation::~PlanetNavigation()
	{
		delete[] distances_;
	}
	void PlanetNavigation::InstantZoomIn()
	{
		if (current_scale_index_ + 1 < num_scales_)
		{
			++current_scale_index_;
			ProcessZoomInstant();
		}
	}
	void PlanetNavigation::InstantZoomOut()
	{
		if (current_scale_index_ > 0)
		{
			--current_scale_index_;
			ProcessZoomInstant();
		}
	}
	void PlanetNavigation::SmoothZoomIn()
	{
		if (current_scale_index_ + 1 < num_scales_)
		{
			++current_scale_index_;
			ProcessZoomSmooth();
		}
	}
	void PlanetNavigation::SmoothZoomOut()
	{
		if (current_scale_index_ > 0)
		{
			--current_scale_index_;
			ProcessZoomSmooth();
		}
	}
	void PlanetNavigation::InstantRotation(float angle_x)
	{
		// Reset any camera animation first
		camera_manager_->PathClear();
		camera_manager_->Clear();
		camera_manager_->RotateAroundTargetInX(angle_x);
	}
	void PlanetNavigation::SmoothRotation(float angle_x)
	{
		const Vector3& pos = *camera_manager_->position();
		const Quaternion& orient = *camera_manager_->orientation();
		Quaternion new_orient = orient * Quaternion(Vector3::UnitX(), angle_x);
		new_orient.Normalize();
		camera_manager_->Clear();
		auto first_camera = camera_manager_->AddAsCurrent();
		auto second_camera = camera_manager_->Add(pos, new_orient, planet_position_);
		camera_manager_->PathClear();
		camera_manager_->PathSetStart(first_camera, 0.0f);
		camera_manager_->PathAdd(second_camera, animation_time_);
	}
	void PlanetNavigation::PanBegin(float screen_x, float screen_y,
							  const Vector4& viewport, const Matrix4& proj, const Matrix4& view)
	{
		Vector2 screen_point(screen_x, screen_y);
		old_screen_point_ = screen_point;
		is_pan_mode_ = true;
		// Clear all animations
		camera_manager_->PathClear();
		camera_manager_->Clear();
	}
	void PlanetNavigation::PanMove(float screen_x, float screen_y,
							 const Vector4& viewport, const Matrix4& proj, const Matrix4& view)
	{
		if (!is_pan_mode_)
			return;

		/*
		Algorithm description.
		Works only with perspective projection.
		We did approximate approach like old screen position was at screen center
		and camera always looks down.

		angle = atan(l / R)
		l / d = tan(view_angle)
		x / h = tan(view_angle)
		h = x_max / tan(fovx/2)

		In clip space x_max = 1, so
		angle = atan((d * x / factor_x) / R)
		*/
		const Vector3& camera_position = *camera_manager_->position();
		float distance = camera_position.Distance(planet_position_);
		
		Vector2 screen_point(screen_x, screen_y);
		Vector2 screen_delta = screen_point - old_screen_point_;
		old_screen_point_ = screen_point;

		Vector2 screen_ndc;
		screen_ndc.x = screen_delta.x / viewport.z;
		screen_ndc.y = screen_delta.y / viewport.w;

		float factor_x = proj.m[0]; // 1/tan(fovx/2)
		float factor_y = proj.m[5]; // 1/tan(fovy/2)

		float angle_x = atan((distance * (screen_ndc.x / factor_x)) / planet_radius_);
		float angle_y = atan((distance * (screen_ndc.y / factor_y)) / planet_radius_);

		Quaternion transform1(Vector3::UnitY(), -angle_x);
		Quaternion transform2(Vector3::UnitZ(), angle_y);
		Quaternion transform = transform1 * transform2;

		Quaternion new_orient = *camera_manager_->orientation() * transform;
		new_orient.Normalize();
		Vector3 direction;
		new_orient.GetDirection(&direction);
		Vector3 new_pos = planet_position_ - distance * direction;
		camera_manager_->MakeFreeTargeted(new_pos, new_orient, planet_position_);
	}
	void PlanetNavigation::PanEnd()
	{
		is_pan_mode_ = false;
	}
	void PlanetNavigation::ObtainZNearZFar(float * znear, float * zfar) const
	{
		const Vector3* cam_pos = camera_manager_->position();
		const Vector3 to_earth = planet_position_ - *cam_pos;
		const Vector3 camera_direction = camera_manager_->GetDirection(); // normalized
		const float cam_distance = to_earth & camera_direction;
		*znear = cam_distance - planet_radius_;
		*zfar = cam_distance + planet_radius_;
	}
	Vector3 PlanetNavigation::MakePoint(float distance)
	{
		return Vector3(planet_radius_ + distance, 0.0f, 0.0f);
	}
	void PlanetNavigation::ProcessZoomInstant()
	{
		float distance = distances_[current_scale_index_];
		const Vector3& pos = *camera_manager_->position();
		const Quaternion& orient = *camera_manager_->orientation();
		Vector3 from_earth = pos - planet_position_;
		from_earth.Normalize();
		Vector3 new_pos = planet_position_ + (planet_radius_ + distance) * from_earth;
		camera_manager_->PathClear();
		camera_manager_->Clear();
		camera_manager_->MakeFreeTargeted(new_pos, orient, planet_position_);
	}
	void PlanetNavigation::ProcessZoomSmooth()
	{
		// Add two cameras: at current position and at specified distance from Earth
		float distance = distances_[current_scale_index_];
		const Vector3& pos = *camera_manager_->position();
		const Quaternion& orient = *camera_manager_->orientation();
		Vector3 from_earth = pos - planet_position_;
		from_earth.Normalize();
		Vector3 new_pos = planet_position_ + (planet_radius_ + distance) * from_earth;
		camera_manager_->Clear();
		auto first_camera = camera_manager_->AddAsCurrent();
		auto second_camera = camera_manager_->Add(new_pos, orient, planet_position_);
		camera_manager_->PathClear();
		camera_manager_->PathSetStart(first_camera, 0.0f);
		camera_manager_->PathAdd(second_camera, animation_time_);
	}

} // namespace scythe