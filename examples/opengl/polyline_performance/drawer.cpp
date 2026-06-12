#include "drawer.h"

#include <vector>

#include "shaders.h"

QuadDrawer::QuadDrawer(const Viewport* viewport)
: viewport_(viewport)
, render_program_()
, vertex_array_object_(0)
, vertex_buffer_object_(0)
, num_vertices_(0)
, vertices_array_(nullptr)
, pixel_width_(20.0f)
{

}
QuadDrawer::~QuadDrawer()
{
	Unload();
	FreeArrays();
}
void QuadDrawer::FreeArrays()
{
	if (vertices_array_ != nullptr)
	{
		delete[] vertices_array_;
		vertices_array_ = nullptr;
	}
}
bool QuadDrawer::CreateData(const PointArray& points)
{
	uint32_t num_points = static_cast<uint32_t>(points.size());
	if (num_points < 2) return false;

	num_vertices_ = num_points;
	vertices_array_ = new uint8_t[num_vertices_ * sizeof(Vertex)];
	Vertex* vertices = reinterpret_cast<Vertex*>(vertices_array_);

	for (uint32_t i = 0; i < num_vertices_; ++i)
	{
		Vertex& vertex = vertices[i];
		const Point& point = points[i];

		vertex.position = {point[0], point[1]};
	}

	return true;
}
bool QuadDrawer::Load()
{
	if (!LoadShaders())
		return false;

	glGenVertexArrays(1, &vertex_array_object_);
	glBindVertexArray(vertex_array_object_);

	glGenBuffers(1, &vertex_buffer_object_);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
	glBufferData(GL_ARRAY_BUFFER, num_vertices_ * sizeof(Vertex), vertices_array_, GL_STATIC_DRAW);

	// Attributes layout
	const GLsizei stride = sizeof(Vertex);
	const uint8_t* base = nullptr;
	const uint8_t* curr_offset = base;
	const uint8_t* next_offset = curr_offset + stride;
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, curr_offset); // vec2 a_position_curr
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, next_offset); // vec2 a_position_next
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	// Finally we can free arrays
	FreeArrays();
	return true;
}
void QuadDrawer::Unload()
{
	if (vertex_buffer_object_ != 0)
	{
		glDeleteBuffers(1, &vertex_buffer_object_);
		vertex_buffer_object_ = 0;
	}
	if (vertex_array_object_ != 0)
	{
		glDeleteVertexArrays(1, &vertex_array_object_);
		vertex_array_object_ = 0;
	}
}
void QuadDrawer::Render()
{
	ActivateShader();

	glBindVertexArray(vertex_array_object_);
	glDrawArrays(GL_POINTS, 0, num_vertices_-1);
	glBindVertexArray(0);

	DeactivateShader();
}
bool QuadDrawer::LoadShaders()
{
	scythe::OpenGLProgram::RenderShadersInfo render_info;
	render_info.vertex   = quad_shaders::kVertexSource;
	render_info.geometry = quad_shaders::kGeometrySource;
	render_info.fragment = quad_shaders::kFragmentSource;
	if (!render_program_.Create(render_info, false))
		return false;

	return true;
}
void QuadDrawer::ActivateShader()
{
	int location;

	glUseProgram(render_program_.id());
	location = glGetUniformLocation(render_program_.id(), "u_viewport");
	glUniform4f(location, 0.0f, 0.0f, (float)viewport_->width, (float)viewport_->height);
	location = glGetUniformLocation(render_program_.id(), "u_pixel_width");
	glUniform1f(location, pixel_width_);
}
void QuadDrawer::DeactivateShader()
{
	glUseProgram(0);
}