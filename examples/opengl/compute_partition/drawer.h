#ifndef __DRAWER_H__
#define __DRAWER_H__

#include <cstdint>
#include <vector>

#include <scythe/opengl/opengl_include.h>
#include <scythe/opengl/opengl_program.h>
#include <scythe/math/vector2.h>
#include <scythe/math/vector4.h>

#include "partitioner.h"

class Drawer final
{
public:
	Drawer() = delete;
	explicit Drawer(uint32_t num_objects);
	~Drawer();

	bool CreateData();
	bool Load();
	void Unload();

	void Render();

private:
	struct Vertex
	{
		scythe::Vector2 position;
		float size;
	};

	void FreeArrays();
	void RenderObjects(bool opaque);
	void FilterObjects();
	void AlignData(uint32_t num_opaque_objects, uint32_t num_transparent_objects);
	bool LoadShaders();

private:
	const uint32_t num_objects_;
	// Data
	const uint32_t vertex_size_;
	const uint32_t index_size_;
	uint32_t num_vertices_;
	uint32_t num_original_indices_;
	uint32_t num_opaque_indices_;
	uint32_t num_transparent_indices_;
	intptr_t opaque_colors_size_;
	intptr_t transparent_colors_size_;
	intptr_t transparent_indices_offset_;
	intptr_t transparent_colors_offset_;
	uint8_t* vertices_array_;
	uint8_t* indices_array_;
	std::vector<scythe::Vector4> colors_;
	// OpenGL resources
	scythe::OpenGLProgram render_program_;
	uint32_t vertex_array_object_;
	uint32_t vertex_buffer_object_;
	uint32_t original_index_buffer_object_;
	uint32_t filtered_index_buffer_object_;
	uint32_t original_colors_buffer_object_;
	uint32_t filtered_colors_buffer_object_;
	uint32_t atomic_counter_object_;
	// Partitioner
	Partitioner partitioner_;
};

#endif