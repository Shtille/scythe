#include "partitioner.h"

#include "partition_shaders.h"

static constexpr uint32_t kScanBlock = 1024u; // == local_size_x в scan/add (branching B)
static constexpr uint32_t kFlatGroup = 256u;  // == local_size_x в flags/scatter

static inline uint32_t DivUp(uint32_t a, uint32_t b)
{
	return (a + b - 1u) / b;
}

OpenGLBuffer::OpenGLBuffer()
: id_(0u)
{
	glCreateBuffers(1, &id_);
}
OpenGLBuffer::~OpenGLBuffer()
{
	glDeleteBuffers(1, &id_);
	id_ = 0u;
}

bool Partitioner::Load()
{
	if (!LoadShaders())
		return false;

	return true;
}
void Partitioner::Unload()
{
	// we may not care
}
void Partitioner::Partition(uint32_t count, uint32_t source_indices_buffer, uint32_t filtered_indices_buffer,
	uint32_t source_colors_buffer, uint32_t filtered_colors_buffer)
{
	if (count == 0u)
		return;

	EnsureLevelBuffers(count);

	GLint locFlagsCount   = glGetUniformLocation(flags_program_.id(),   "uCount");
	GLint locScanCount    = glGetUniformLocation(scan_program_.id(),    "uCount");
	GLint locAddCount     = glGetUniformLocation(add_program_.id(),     "uCount");
	GLint locScatterCount = glGetUniformLocation(scatter_program_.id(), "uCount");

	const unsigned int D = static_cast<unsigned int>(level_sizes_.size()) - 1u; // число scan-уровней

	// --- 1. Флаг непрозрачности -> buf[0] ---
	glUseProgram(flags_program_.id());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, source_colors_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, level_bufs_[0]);
	glUniform1ui(locFlagsCount, count);
	glDispatchCompute(::DivUp(count, kFlatGroup), 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	// --- 2. Down-sweep: block-exclusive-scan на каждом уровне (in place) ---
	glUseProgram(scan_program_.id());
	for (unsigned int l = 0; l < D; ++l)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, level_bufs_[l]);     // Data
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, level_bufs_[l + 1]); // Sums
		glUniform1ui(locScanCount, level_sizes_[l]);
		glDispatchCompute(::DivUp(level_sizes_[l], kScanBlock), 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
	// level_bufs_[D][0] == numOpaque (общий итог)

	// --- 3. Up-sweep: прибавляем глобальные смещения блоков ---
	glUseProgram(add_program_.id());
	for (int l = static_cast<int>(D) - 2; l >= 0; --l)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, level_bufs_[l]);     // Data
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, level_bufs_[l + 1]); // Parent
		glUniform1ui(locAddCount, level_sizes_[l]);
		glDispatchCompute(::DivUp(level_sizes_[l], kScanBlock), 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
	// level_bufs_[0][i] == число непрозрачных среди [0, i)

	// --- 4. Scatter в правильном порядке ---
	glUseProgram(scatter_program_.id());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, level_bufs_[0]); // prefix
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, level_bufs_[D]); // grand total
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, source_indices_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, filtered_indices_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, source_colors_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, filtered_colors_buffer);
	glUniform1ui(locScatterCount, count);
	glDispatchCompute(::DivUp(count, kFlatGroup), 1, 1);

	// Результат идёт дальше как EBO + SSBO -> бит на приёмнике:
	glMemoryBarrier(GL_ELEMENT_ARRAY_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
}
uint32_t Partitioner::ReadNumOpaque() const
{
	if (level_sizes_.empty())
		return 0u;
	uint32_t n = 0u;
	glGetNamedBufferSubData(level_bufs_[level_sizes_.size() - 1], 0, 4, &n);
	return n;
}
void Partitioner::EnsureLevelBuffers(uint32_t num_indices)
{
	level_sizes_.clear();
	unsigned int n = num_indices;
	level_sizes_.push_back(n);
	while (n > 1u)
	{
		n = ::DivUp(n, kScanBlock);
		level_sizes_.push_back(n);
	}

	const size_t needed = level_sizes_.size(); // == D+1
	level_bufs_.resize(needed, 0u);
	level_caps_.resize(needed, 0);

	for (size_t i = 0; i < needed; ++i)
	{
		const size_t bytes = static_cast<size_t>(level_sizes_[i]) * 4ull;

		auto it = buffers_.find(i);
		if (it == buffers_.end())
			it = buffers_.emplace(i, std::make_unique<OpenGLBuffer>()).first;
		uint32_t buffer_id = it->second->id();
		level_bufs_[i] = buffer_id;

		if (level_caps_[i] < bytes)
		{
			glNamedBufferData(level_bufs_[i], bytes, nullptr, GL_DYNAMIC_DRAW);
			level_caps_[i] = bytes;
		}
	}
}
bool Partitioner::LoadShaders()
{
	// Flags
	{
		scythe::OpenGLProgram::ComputeShadersInfo info;
		info.compute = partition_flags_shaders::kComputeSource;
		if (!flags_program_.Create(info, false))
			return false;
	}
	// Scan
	{
		scythe::OpenGLProgram::ComputeShadersInfo info;
		info.compute = partition_scan_shaders::kComputeSource;
		if (!scan_program_.Create(info, false))
			return false;
	}
	// Add
	{
		scythe::OpenGLProgram::ComputeShadersInfo info;
		info.compute = partition_add_shaders::kComputeSource;
		if (!add_program_.Create(info, false))
			return false;
	}
	// Scatter
	{
		scythe::OpenGLProgram::ComputeShadersInfo info;
		info.compute = partition_scatter_shaders::kComputeSource;
		if (!scatter_program_.Create(info, false))
			return false;
	}
	return true;
}