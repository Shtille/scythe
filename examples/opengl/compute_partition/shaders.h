#ifndef __SHADERS_H__
#define __SHADERS_H__

namespace quad_shaders {

static const char* kVertexSource = R"(
#version 460 core

layout (location = 0) in vec2 a_position;
layout (location = 1) in float a_size;

out VS_OUT {
	float size;
} vs_out;

void main()
{
	gl_Position = vec4(a_position, 0.0, 1.0);
	vs_out.size = a_size;
}
)";

static const char* kGeometrySource = R"(
#version 460 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in VS_OUT {
	float size;
} gs_in[];

void build_quad(vec4 pos, float size)
{
	gl_Position = pos + vec4(-size, -size, 0.0, 0.0);
	gl_PrimitiveID = gl_PrimitiveIDIn;
	EmitVertex();
	gl_Position = pos + vec4(+size, -size, 0.0, 0.0);
	gl_PrimitiveID = gl_PrimitiveIDIn;
	EmitVertex();
	gl_Position = pos + vec4(-size, +size, 0.0, 0.0);
	gl_PrimitiveID = gl_PrimitiveIDIn;
	EmitVertex();
	gl_Position = pos + vec4(+size, +size, 0.0, 0.0);
	gl_PrimitiveID = gl_PrimitiveIDIn;
	EmitVertex();
	EndPrimitive();
}

void main()
{
	build_quad(gl_in[0].gl_Position, gs_in[0].size);
}
)";

static const char* kFragmentSource = R"(
#version 460 core

layout (std430, binding = 0) readonly buffer Colors {
	vec4 colors[];
};

out vec4 fragment_color;

void main()
{
	vec4 current_color = colors[gl_PrimitiveID];
	fragment_color = current_color;
}
)";

} // namespace quad_shaders

namespace filter_shaders {

static const char* kComputeSource = R"(
#version 460 core

layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

layout (std430, binding = 0) readonly buffer InputIndexBuffer {
	uint input_indices[];
};
layout (std430, binding = 1) writeonly buffer OutputIndexBuffer {
	uint output_indices[];
};

layout (binding = 0) uniform atomic_uint counter;

uniform uint count;

void main()
{
	uint src_index = gl_GlobalInvocationID.x;
	if (src_index >= count)
		return;
	uint dst_index = atomicCounterIncrement(counter);
	output_indices[dst_index] = input_indices[src_index];
}
)";

} // namespace filter_shaders

#endif