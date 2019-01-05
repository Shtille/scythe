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
		Vector3 ray;
		ScreenToRay(screen_point, viewport, proj, view, &ray);
		const Vector3& origin = *camera_manager_->position();
		is_pan_mode_ = RaySphereIntersection(origin, ray, planet_position_, planet_radius_, &pan_point_);
		// Clear all animations
		camera_manager_->PathClear();
		camera_manager_->Clear();
	}
	void PlanetNavigation::PanMove(float screen_x, float screen_y,
							 const Vector4& viewport, const Matrix4& proj, const Matrix4& view)
	{
		if (!is_pan_mode_)
			return;
		
		Vector2 screen_point(screen_x, screen_y);
		Vector3 ray;
		ScreenToRay(screen_point, viewport, proj, view, &ray);
		const Vector3& origin = *camera_manager_->position();
		Vector3 new_point;
		if (RaySphereIntersection(origin, ray, planet_position_, planet_radius_, &new_point))
		{
			Vector3 first_vector, second_vector;
			float dot_product;
			// Forward should vector should point down, up and side will be tangent to sphere
			// We will rotate in two planes
			Vector3 normal1 = camera_manager_->GetUp();
			Vector3 normal2 = camera_manager_->GetSide();
			Vector3 distance1 = (new_point - pan_point_) & normal1;
			Vector3 projected_point = new_point - distance1 * normal1;
			
			Vector3 plane_point1 = (pan_point_ & normal1) * normal1 + planet_position_;
			first_vector = pan_point_ - plane_point1;
			first_vector.Normalize();
			second_vector = projected_point - plane_point1;
			second_vector.Normalize();
			dot_product = first_vector & second_vector;
			if (dot_product > 1.0f)
				dot_product = 1.0f;
			float angle1 = acosf(dot_product);
			if (((projected_point - pan_point_) & normal2) < 0.0f)
				angle1 = -angle1;
			
			Vector3 plane_point2 = (new_point & normal2) * normal2 + planet_position_;
			first_vector = projected_point - plane_point2;
			first_vector.Normalize();
			second_vector = new_point - plane_point2;
			second_vector.Normalize();
			dot_product = first_vector & second_vector;
			if (dot_product > 1.0f)
				dot_product = 1.0f;
			float angle2 = acosf(dot_product);
			if (((projected_point - new_point) & normal1) < 0.0f)
				angle2 = -angle2;

			Quaternion transform1(Vector3::UnitY(), angle1);
			Quaternion transform2(Vector3::UnitZ(), angle2);
			Quaternion inverse_transform = transform1 * transform2;
			inverse_transform.Inverse();

			Quaternion new_orient = *camera_manager_->orientation() * inverse_transform;
			new_orient.Normalize();
			const Vector3& cam_pos = *camera_manager_->position();
			float distance = cam_pos.Distance(planet_position_);
			Vector3 direction;
			new_orient.GetDirection(&direction); // ?????
			Vector3 new_pos = planet_position_ - distance * direction;
			camera_manager_->MakeFreeTargeted(new_pos, new_orient, planet_position_);
		}
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