#ifndef __SCYTHE_PLANET_NAVIGATION_H__
#define __SCYTHE_PLANET_NAVIGATION_H__

#include "math/matrix4.h"
#include "math/vector2.h"

namespace scythe {

	// Forward declarations
	class CameraManager;

	//! Class for planet navigation
	class PlanetNavigation {
	public:
		PlanetNavigation(CameraManager * camera_manager, const Vector3& position, float planet_radius,
			float animation_time, float farest_distance, float nearest_distance);
		~PlanetNavigation();

		void InstantZoomIn();
		void InstantZoomOut();
		void SmoothZoomIn();
		void SmoothZoomOut();
		
		void InstantRotation(float angle_x);
		void SmoothRotation(float angle_x);
		
		void ObtainZNearZFar(float * znear, float * zfar) const;
		
		void PanBegin(float screen_x, float screen_y,
					  const Vector4& viewport, const Matrix4& proj, const Matrix4& view);
		void PanMove(float screen_x, float screen_y,
					 const Vector4& viewport, const Matrix4& proj, const Matrix4& view);
		void PanEnd();
		
	protected:
		Vector3 MakePoint(float distance);
		void ProcessZoomInstant();
		void ProcessZoomSmooth();

	private:
		CameraManager * camera_manager_;
		Vector3 planet_position_;			//!< planet position
		float planet_radius_;				//!< planet radius
		float animation_time_;				//!< animation time, seconds
		int num_scales_;					//!< number of scales
		int current_scale_index_;			//!< current scale index
		float * distances_;					//!< array of distances
		Vector2 old_screen_point_;			//!< old screen position of mouse
		bool is_pan_mode_;					//!< is pan mode enabled
	};

} // namespace scythe

#endif