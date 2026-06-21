#include "drawer.h"

#include <vector>

#include <scythe/log.h> // temp

#include "shaders.h"

Drawer::Drawer(uint32_t num_objects)
: num_objects_(num_objects)
, vertex_size_(sizeof(Vertex))
, index_size_(sizeof(uint32_t))
, num_vertices_(0)
, num_original_indices_(0)
, num_opaque_indices_(0)
, num_transparent_indices_(0)
, opaque_colors_size_(0)
, transparent_colors_size_(0)
, transparent_indices_offset_(0)
, transparent_colors_offset_(0)
, vertices_array_(nullptr)
, indices_array_(nullptr)
, render_program_()
, vertex_array_object_(0)
, vertex_buffer_object_(0)
, original_index_buffer_object_(0)
, filtered_index_buffer_object_(0)
, original_colors_buffer_object_(0)
, filtered_colors_buffer_object_(0)
, atomic_counter_object_(0)
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
	float size = 1.0f / static_cast<float>(num_objects_); // half of quad length
	float start_x = -1.0f + size;
	float offset_x = size + size;
	for (uint32_t i = 0; i < num_vertices_; ++i)
	{
		float x = start_x + offset_x * static_cast<float>(i);

		bool even = (i % 2) == 0;
		Vertex& vertex = vertices[i];
		vertex.position.Set(x, 0.0f);
		vertex.size = even ? size : (size * 1.4f);
	}

	// Fill indices data
	num_original_indices_ = num_objects_;

	indices_array_ = new uint8_t[num_original_indices_ * index_size_];
	if (!indices_array_)
		return false;
	uint32_t* indices = reinterpret_cast<uint32_t*>(indices_array_);
	for (uint32_t i = 0; i < num_original_indices_; ++i)
	{
		indices[i] = i;
	}

	// Fill colors
	colors_.reserve(num_objects_);
	for (uint32_t i = 0u; i < num_objects_; ++i)
	{
		bool even = (i % 2) == 0;
		scythe::Vector4 color;
		if (even)
			color.Set(1.0f, 0.0f, 0.0f, 1.0f);
		else
			color.Set(0.0f, 1.0f, 0.0f, 0.5f);
		colors_.push_back(color);
	}

	return true;
}
bool Drawer::Load()
{
	if (!LoadShaders())
		return false;

	// Original indices buffer
	glGenBuffers(1, &original_index_buffer_object_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, original_index_buffer_object_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_original_indices_ * index_size_, indices_array_, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Atomic counter buffer
	glGenBuffers(1, &atomic_counter_object_);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomic_counter_object_);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	// Original colors buffer
	glGenBuffers(1, &original_colors_buffer_object_);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, original_colors_buffer_object_);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(colors_[0]) * colors_.size(), colors_.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// Filtered colors buffer
	glGenBuffers(1, &filtered_colors_buffer_object_);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, filtered_colors_buffer_object_);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(colors_[0]) * colors_.size(), colors_.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// --- Render objects ---
	glGenVertexArrays(1, &vertex_array_object_);
	glBindVertexArray(vertex_array_object_);

	// Vertex buffer
	glGenBuffers(1, &vertex_buffer_object_);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
	glBufferData(GL_ARRAY_BUFFER, num_vertices_ * vertex_size_, vertices_array_, GL_STATIC_DRAW);

	// Filtered index buffer
	glGenBuffers(1, &filtered_index_buffer_object_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, filtered_index_buffer_object_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_original_indices_ * index_size_, indices_array_, GL_DYNAMIC_DRAW);

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

	if (!partitioner_.Load())
		return false;

	FilterObjects();

	// Enable blend to see transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return true;
}
void Drawer::Unload()
{
	partitioner_.Unload();

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
	if (original_index_buffer_object_ != 0)
	{
		glDeleteBuffers(1, &original_index_buffer_object_);
		original_index_buffer_object_ = 0;
	}
	if (filtered_index_buffer_object_ != 0)
	{
		glDeleteBuffers(1, &filtered_index_buffer_object_);
		filtered_index_buffer_object_ = 0;
	}
	if (atomic_counter_object_ != 0)
	{
		glDeleteBuffers(1, &atomic_counter_object_);
		atomic_counter_object_ = 0;
	}
	if (original_colors_buffer_object_ != 0)
	{
		glDeleteBuffers(1, &original_colors_buffer_object_);
		original_colors_buffer_object_ = 0;
	}
	if (filtered_colors_buffer_object_ != 0)
	{
		glDeleteBuffers(1, &filtered_colors_buffer_object_);
		filtered_colors_buffer_object_ = 0;
	}
}
void Drawer::Render()
{
	RenderObjects(true);
	RenderObjects(false);
}
void Drawer::RenderObjects(bool opaque)
{
	GLsizei num_indices;
	const uint8_t* base = nullptr;
	size_t indices_offset;
	GLintptr colors_offset;
	GLsizeiptr colors_size;
	if (opaque)
	{
		indices_offset = 0;
		num_indices = static_cast<GLsizei>(num_opaque_indices_);
		colors_offset = 0;
		colors_size = opaque_colors_size_;
	}
	else // transparent
	{
		indices_offset = transparent_indices_offset_;
		num_indices = static_cast<GLsizei>(num_transparent_indices_);
		colors_offset = transparent_colors_offset_;
		colors_size = transparent_colors_size_;
	}

	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, filtered_colors_buffer_object_, colors_offset, colors_size);

	glUseProgram(render_program_.id());
	glBindVertexArray(vertex_array_object_);
	glDrawElements(GL_POINTS, num_indices, GL_UNSIGNED_INT, base + indices_offset);
	glBindVertexArray(0);
	glUseProgram(0);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
}
void Drawer::FilterObjects()
{
	partitioner_.Partition(num_objects_, original_index_buffer_object_, filtered_index_buffer_object_,
		original_colors_buffer_object_, filtered_colors_buffer_object_);

	uint32_t num_opaque_objects = partitioner_.ReadNumOpaque();
	uint32_t num_transparent_objects = num_objects_ - num_opaque_objects;

	num_opaque_indices_ = num_opaque_objects;
	num_transparent_indices_ = num_transparent_objects;
	transparent_indices_offset_ = num_opaque_indices_ * sizeof(uint32_t);

	AlignData(num_opaque_objects, num_transparent_objects);
}
void Drawer::AlignData(uint32_t num_opaque_objects, uint32_t num_transparent_objects)
{
	GLint alignment;
	glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &alignment);

	constexpr size_t element_size = sizeof(colors_[0]);
	size_t original_transparent_offset = element_size * num_opaque_objects;
	size_t aligned_transparent_offset = ((original_transparent_offset + alignment - 1) / alignment) * alignment;
	size_t aligned_size = sizeof(colors_[0]) * colors_.size() + (aligned_transparent_offset - original_transparent_offset);

	// Create new colors buffer
	uint32_t aligned_colors_buffer_object = 0u;
	glGenBuffers(1, &aligned_colors_buffer_object);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, aligned_colors_buffer_object);
	glBufferData(GL_SHADER_STORAGE_BUFFER, aligned_size, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_COPY_READ_BUFFER, filtered_colors_buffer_object_);
	glBindBuffer(GL_COPY_WRITE_BUFFER, aligned_colors_buffer_object);

	// Copy opaque colors
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, num_opaque_objects * element_size);
	// Copy transparent colors
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, original_transparent_offset, aligned_transparent_offset, num_transparent_objects * element_size);

	glBindBuffer(GL_COPY_READ_BUFFER, 0);
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

	// Replace old buffer with aligned version
	glDeleteBuffers(1, &filtered_colors_buffer_object_);
	filtered_colors_buffer_object_ = aligned_colors_buffer_object;

	// Finally
	opaque_colors_size_ = static_cast<intptr_t>(num_opaque_objects * element_size);
	transparent_colors_size_ = static_cast<intptr_t>(num_transparent_objects * element_size);
	transparent_colors_offset_ = static_cast<intptr_t>(aligned_transparent_offset);
}
bool Drawer::LoadShaders()
{
	scythe::OpenGLProgram::RenderShadersInfo render_info;
	render_info.vertex   = quad_shaders::kVertexSource;
	render_info.geometry = quad_shaders::kGeometrySource;
	render_info.fragment = quad_shaders::kFragmentSource;
	if (!render_program_.Create(render_info, false))
		return false;

	return true;
}