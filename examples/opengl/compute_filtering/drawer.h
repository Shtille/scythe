#ifndef __DRAWER_H__
#define __DRAWER_H__

#include <cstdint>

#include <scythe/opengl/opengl_include.h>
#include <scythe/opengl/opengl_program.h>
#include <scythe/math/vector2.h>

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
	void ResetCounterToZero();
	void FilterIndices();
	void ReadFilteredCount();
	void RenderObjects();
	bool LoadShaders();

private:
	uint32_t num_objects_;
	// Data
	const uint32_t vertex_size_;
	const uint32_t index_size_;
	uint32_t num_vertices_;
	uint32_t num_original_indices_;
	uint32_t num_filtered_indices_;
	uint8_t* vertices_array_;
	uint8_t* indices_array_;
	// OpenGL resources
	scythe::OpenGLProgram render_program_;
	scythe::OpenGLProgram compute_program_;
	uint32_t vertex_array_object_;
	uint32_t vertex_buffer_object_;
	uint32_t original_index_buffer_object_;
	uint32_t filtered_index_buffer_object_;
	uint32_t atomic_counter_object_;
};

#endif