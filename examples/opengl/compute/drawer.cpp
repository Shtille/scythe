#include "drawer.h"

#include <vector>

#include "shaders.h"

Drawer::Drawer(uint32_t num_objects)
: num_objects_(num_objects)
, vertex_size_(sizeof(Vertex))
, index_size_(sizeof(uint32_t))
, num_vertices_(0)
, num_indices_(0)
, vertices_array_(nullptr)
, indices_array_(nullptr)
, program_()
, vertex_array_object_(0)
, vertex_buffer_object_(0)
, index_buffer_object_(0)
{
	//
}
Drawer::~Drawer()
{
	Unload();
	FreeArrays();
}
void Drawer::FreeArrays()
{
	if (vertices_array_)
	{
		delete[] vertices_array_;
		vertices_array_ = nullptr;
	}
	if (indices_array_)
	{
		delete[] indices_array_;
		indices_array_ = nullptr;
	}
}
bool Drawer::CreateData()
{
	// Geometry shader will generate 4 vertices per 1 attribute one
	num_vertices_ = num_objects_;

	vertices_array_ = new uint8_t[num_vertices_ * vertex_size_];
	if (!vertices_array_)
		return false;
	Vertex* vertices = reinterpret_cast<Vertex*>(vertices_array_);
	float size = 1.0f / static_cast<float>(num_objects_);
	float start_x = -1.0f + size;
	float offset_x = size + size;
	for (uint32_t i = 0; i < num_vertices_; ++i)
	{
		float x = start_x + offset_x * static_cast<float>(i);

		Vertex& vertex = vertices[i];
		vertex.position.Set(x, 0.0f);
		vertex.size = size;
	}

	// Fill indices data
	num_indices_ = num_objects_;

	indices_array_ = new uint8_t[num_indices_ * index_size_];
	if (!indices_array_)
		return false;
	uint32_t* indices = reinterpret_cast<uint32_t*>(indices_array_);
	for (uint32_t i = 0; i < num_indices_; ++i)
	{
		indices[i] = i;
	}

	return true;
}
bool Drawer::Load()
{
	if (!LoadShaders())
		return false;

	glGenVertexArrays(1, &vertex_array_object_);
	glBindVertexArray(vertex_array_object_);

	glGenBuffers(1, &vertex_buffer_object_);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
	glBufferData(GL_ARRAY_BUFFER, num_vertices_ * vertex_size_, vertices_array_, GL_STATIC_DRAW);

	glGenBuffers(1, &index_buffer_object_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices_ * index_size_, indices_array_, GL_STATIC_DRAW);

	// Attributes layout
	const GLsizei stride = sizeof(Vertex);
	const uint8_t* base = nullptr;
	const uint8_t* position_offset = base;
	const uint8_t* size_offset = position_offset + sizeof(scythe::Vector2);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, position_offset); // vec2 a_position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, stride, size_offset); // float a_size
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	// Finally we can free arrays
	FreeArrays();
	return true;
}
void Drawer::Unload()
{
	if (vertex_array_object_ != 0)
	{
		glDeleteVertexArrays(1, &vertex_array_object_);
		vertex_array_object_ = 0;
	}
	if (vertex_buffer_object_ != 0)
	{
		glDeleteBuffers(1, &vertex_buffer_object_);
		vertex_buffer_object_ = 0;
	}
	if (index_buffer_object_ != 0)
	{
		glDeleteBuffers(1, &index_buffer_object_);
		index_buffer_object_ = 0;
	}
}
void Drawer::Render()
{
	glUseProgram(program_.id());
	int location = glGetUniformLocation(program_.id(), "u_color");
	glUniform4f(location, 1.0f, 0.0f, 0.0f, 0.0f);

	glBindVertexArray(vertex_array_object_);
	glDrawElements(GL_POINTS, num_indices_, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
	glUseProgram(0);
}
bool Drawer::LoadShaders()
{
	scythe::OpenGLProgram::RenderShadersInfo info;
	info.vertex   = quad_shaders::kVertexSource;
	info.geometry = quad_shaders::kGeometrySource;
	info.fragment = quad_shaders::kFragmentSource;
	return program_.Create(info, false);
}