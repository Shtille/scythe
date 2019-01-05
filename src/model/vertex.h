#ifndef __SCYTHE_VERTEX_H__
#define __SCYTHE_VERTEX_H__

#include "math/vector2.h"
#include "math/vector3.h"

namespace scythe {

	struct Vertex {
		Vector3 position;
		Vector3 normal;
		Vector2 texcoord;
		Vector3 tangent;
		Vector3 binormal;
	};

} // namespace scythe

#endif