#ifndef __QUAD_DRAWER_H__
#define __QUAD_DRAWER_H__

#include <scythe/opengl/opengl_include.h>
#include <scythe/opengl/opengl_program.h>

#include "types.h"

/**
 * @brief      This class describes a quad drawer with geometry shader.
 */
class QuadDrawer
{
public:
	QuadDrawer(const Viewport* viewport);
	~QuadDrawer();

	bool CreateData(const PointArray& points);
	bool Load();
	void Unload();

	void Render();

protected:

	struct Vertex
	{
		Point position;
	};

	void FreeArrays();
	bool LoadShaders();

	void ActivateShader();
	void DeactivateShader();

private:
	const Viewport* viewport_;
	scythe::OpenGLProgram render_program_;
	uint32_t vertex_array_object_;
	uint32_t vertex_buffer_object_;
	uint32_t num_vertices_;
	uint8_t* vertices_array_;
	const float pixel_width_;
};

#endif // __QUAD_DRAWER_H__