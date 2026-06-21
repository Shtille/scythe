#ifndef __PARTITIONER_H__
#define __PARTITIONER_H__

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <memory>

#include <scythe/opengl/opengl_program.h>

class OpenGLBuffer
{
public:
	OpenGLBuffer();
	~OpenGLBuffer();

	GLuint id() const { return id_; }
private:
	GLuint id_;
};

class Partitioner
{
public:
	Partitioner() = default;

	bool Load();
	void Unload();

	void Partition(uint32_t num_indices, uint32_t source_indices_buffer, uint32_t filtered_indices_buffer,
		uint32_t source_colors_buffer, uint32_t filtered_colors_buffer);

	uint32_t ReadNumOpaque() const;

private:
	void EnsureLevelBuffers(uint32_t num_indices);
	bool LoadShaders();

private:
	scythe::OpenGLProgram flags_program_;
	scythe::OpenGLProgram scan_program_;
	scythe::OpenGLProgram add_program_;
	scythe::OpenGLProgram scatter_program_;

	std::vector<uint32_t> level_bufs_;
	std::vector<uint32_t> level_sizes_;
	std::vector<size_t>   level_caps_;
	std::unordered_map<size_t, std::unique_ptr<OpenGLBuffer>> buffers_;
};

#endif