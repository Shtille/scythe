#include "shader_info.h"

namespace scythe {

	ShaderInfo::ShaderInfo(const char* base_filename)
	: base_filename_(base_filename)
	, vertex_program_(nullptr)
	, fragment_program_(nullptr)
	, attributes_(nullptr)
	, num_attributes_(0)
	, defines_(nullptr)
	, num_defines_(0)
	{
	}
	ShaderInfo::ShaderInfo(
		const char* base_filename, const char* vertex_program, const char* fragment_program,
		const char** attributes, U32 num_attributes, const char** defines, U32 num_defines)
	: base_filename_(base_filename)
	, vertex_program_(vertex_program)
	, fragment_program_(fragment_program)
	, attributes_(attributes)
	, num_attributes_(num_attributes)
	, defines_(defines)
	, num_defines_(num_defines)
	{
	}

} // namespace scythe