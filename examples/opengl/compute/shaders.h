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
	EmitVertex();
	gl_Position = pos + vec4(+size, -size, 0.0, 0.0);
	EmitVertex();
	gl_Position = pos + vec4(-size, +size, 0.0, 0.0);
	EmitVertex();
	gl_Position = pos + vec4(+size, +size, 0.0, 0.0);
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

uniform vec4 u_color;

out vec4 color;

void main()
{
	color = u_color;
}
)";

} // namespace quad_shaders

#endif