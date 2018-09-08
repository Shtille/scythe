#ifndef __SCYTHE_VERTEX_H__
#define __SCYTHE_VERTEX_H__

#include "math/vector.h"

namespace scythe {

	struct Vertex {
		vec3 position;
		vec3 normal;
		vec2 texcoord;
		vec3 tangent;
		vec3 binormal;
	};

} // namespace scythe

#endif