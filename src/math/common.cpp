#include <scythe/math/common.h>

#include <cmath>

#include <scythe/math/constants.h>

namespace scythe {

	Vector3 ClosestPointOnLine(const Vector3& a, const Vector3& b, const Vector3& p)
	{
		Vector3 c = p - a;
		Vector3 v = b - a;
		float d = v.Length();
		v *= 1.0f/d; // normalize
		float t = v & c;

		if (t < 0.0f)
			return a;
		if (t > d)
			return b;

		v *= t;
		return (a + v);
	}
	Vector3 Orthogonalize(const Vector3& v1, const Vector3& v2)
	{
		Vector3 v2ProjV1 = ClosestPointOnLine(v1, -v1, v2);
		Vector3 res = v2 - v2ProjV1;
		res.Normalize();
		return res;
	}
	float DistanceToCamera(const Vector3& world, const Matrix4& view)
	{
		Vector4 pos_eye = view * Vector4(world.x, world.y, world.z, 1.0f);
		return Vector3(pos_eye.x, pos_eye.y, pos_eye.z).Length();
	}
	float DistanceToCamera(const Vector4& world, const Matrix4& view)
	{
		Vector4 pos_eye = view * world;
		return Vector3(pos_eye.x, pos_eye.y, pos_eye.z).Length();
	}
	void WorldToScreen(const Vector4& world, const Matrix4& proj, const Matrix4& view, const Vector4& viewport, Vector2 * screen)
	{
		// Step 1: 4d Clip space
		Vector4 pos_clip = proj * view * world;

		// Step 2: 3d Normalized Device Coordinate space
		Vector3 pos_ndc = Vector3(pos_clip.x, pos_clip.y, pos_clip.z) / pos_clip.w;

		// Step 3: Window Coordinate space
		screen->x = (pos_ndc.x + 1.0f) * 0.5f * viewport.z + viewport.x;
		screen->y = (pos_ndc.y + 1.0f) * 0.5f * viewport.w + viewport.y;
	}
	void ScreenToRay(const Vector2& screen, const Vector4& viewport, const Matrix4& proj, const Matrix4& view, Vector3 * ray)
	{
		// Step 1: 3d Normalised Device Coordinates ( range [-1:1, -1:1, -1:1] )
		Vector2 screen_ndc;
		screen_ndc.x = 2.0f * (screen.x - viewport.x) / viewport.z - 1.0f;
		screen_ndc.y = 2.0f * (screen.y - viewport.y) / viewport.w - 1.0f;
		// We don't need z component actually
	
		// Step 2: 4d Homogeneous Clip Coordinates ( range [-1:1, -1:1, -1:1, -1:1] )
		Vector4 ray_clip(
			screen_ndc.x,
			screen_ndc.y,
			-1.0, // We want our ray's z to point forwards - this is usually the negative z direction in OpenGL style.
			1.0
			);
		// Step 3: 4d Eye (Camera) Coordinates ( range [-x:x, -y:y, -z:z, -w:w] )
		Matrix4 inversed_proj;
		proj.Invert(&inversed_proj);
		Vector4 ray_eye = inversed_proj * ray_clip;
		// Now, we only needed to un-project the x,y part, so let's manually set the z,w part to mean "forwards, and not a point".
		ray_eye.z = -1.0f;
		ray_eye.w = 0.0f;
		// Step 4: 4d World Coordinates ( range [-x:x, -y:y, -z:z, -w:w] )
		//ray = (view.GetInverse() * ray_eye).xyz();
		Matrix4 inversed_view;
		view.Invert(&inversed_view);
		inversed_view.TransformVector(Vector3(ray_eye.x, ray_eye.y, ray_eye.z), ray);
		ray->Normalize();
	}
	bool RaySphereIntersection(const Vector3& origin, const Vector3& direction, const Vector3& center, float radius, Vector3 * intersection)
	{
		Vector3 line = origin - center;
		float b = (direction & line);
		float c = (line & line) - radius * radius;
		float D = b * b - c;
		if (D < 0.0f) // ray cannot intersect sphere
			return false;
		float t;
		if (c > 0.0f) // origin is outside of sphere, choose closer hitpoint
			t = -b - sqrtf(D);
		else // origin is inside the sphere, choose forward hitpoint
			t = -b + sqrtf(D);
		if (t > 0.0f)
		{
			if (intersection)
				*intersection = origin + t * direction;
			return true;
		}
		else
			return false;
	}
	bool RayPlaneIntersection(const Vector3& origin, const Vector3& direction, const Vector4& plane, Vector3 * intersection)
	{
		float denom = direction.x * plane.x + direction.y * plane.y + direction.z * plane.z;
		if (denom > kFloatSmall || denom < -kFloatSmall)
		{
			if (intersection)
			{
				float t = -(origin.x * plane.x + origin.y * plane.y + origin.z * plane.z + plane.w) / denom;
				*intersection = origin + t * direction;
			}
			return true;
		}
		else
			return false;
	}

} // namespace scythe