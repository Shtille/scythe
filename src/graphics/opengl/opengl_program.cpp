#include <scythe/opengl/opengl_program.h>

#include <scythe/log.h>

#include <string>
#include <vector>
#include <array>
#include <optional>
#include <cstdio>

/**
 * @brief      Reads a file.
 * @todo       Replace with scythe function.
 *
 * @param[in]       filename  The filename
 * @param[in,out]   data      The data
 *
 * @return     True on success and false otherwise.
 */
static bool ReadFile(const char* filename, std::string& data)
{
	FILE * f = fopen(filename, "rt");
	if (!f)
	{
		scythe::Error("Can't open file '%s'\n", filename);
		return false;
	}

	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	if (size <= 0)
		return false;

	data.resize(size+1);
	fseek(f, 0, SEEK_SET);
	fread(&data[0], sizeof(char), size, f);
	data[size] = '\0';
	fclose(f);

	return true;
}

namespace scythe {

	/**
	 * @brief      This class describes an OpenGL shader.
	 */
	class OpenGLShader
	{
	public:
		OpenGLShader(GLenum shader_type);
		~OpenGLShader();
		GLuint id() const;
	private:
		GLuint id_ = 0u;
	};

	OpenGLShader::OpenGLShader(GLenum shader_type)
	{
		id_ = glCreateShader(shader_type);
	}
	OpenGLShader::~OpenGLShader()
	{
		glDeleteShader(id_);
	}
	GLuint OpenGLShader::id() const
	{
		return id_;
	}

	bool OpenGLProgram::Create(const ShadersInfo& info, bool use_files)
	{
		std::array<const char*, kNumShaders> shader_files = {
			info.vertex,
			info.fragment,
			info.geometry,
		};
		std::array<GLenum, kNumShaders> shader_types = {
			GL_VERTEX_SHADER,
			GL_FRAGMENT_SHADER,
			GL_GEOMETRY_SHADER,
		};
		std::array<bool, kNumShaders> shader_required = {
			true, true, false,
		};
		std::array<std::optional<OpenGLShader>, kNumShaders> shaders;

		for (std::size_t i = 0u; i < kNumShaders; ++i)
		{
			if (!shader_required[i] && shader_files[i] == nullptr)
				continue;
			if (shader_required[i] && shader_files[i] == nullptr)
				return false;

			// Read our shader into the appropriate buffer
			std::string shader_code;
			if (use_files && !ReadFile(shader_files[i], shader_code))
				return false;

			auto& shader = shaders[i];
			// Create an empty shader handle
			shader.emplace(shader_types[i]);
			GLuint shader_id = shader.value().id();

			// Send the shader source code to GL
			// Note that std::string's .c_str is NULL character terminated.
			const GLchar *source;
			if (use_files)
				source = (const GLchar *)shader_code.c_str();
			else
				source = (const GLchar *)shader_files[i];
			glShaderSource(shader_id, 1, &source, 0);

			// Compile the vertex shader
			glCompileShader(shader_id);

			GLint is_compiled = 0;
			glGetShaderiv(shader_id, GL_COMPILE_STATUS, &is_compiled);
			if (is_compiled == GL_FALSE)
			{
				GLint max_length = 0;
				glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &max_length);

				// The max_length includes the NULL character
				std::vector<GLchar> info_log(static_cast<std::size_t>(max_length));
				glGetShaderInfoLog(shader_id, max_length, &max_length, &info_log[0]);
				
				// We don't need the shader anymore.

				// Use the info_log as you see fit.
				Error("Error during shader '%s' compilation:", shader_files[i]);
				Error("%s", &info_log[0]);
				
				// In this simple program, we'll just leave
				return false;
			}
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		id_ = glCreateProgram();

		// Attach our shaders to our program
		for (const auto& shader : shaders)
			if (shader.has_value())
				glAttachShader(id_, shader.value().id());

		// Link our program
		glLinkProgram(id_);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint is_linked = 0;
		glGetProgramiv(id_, GL_LINK_STATUS, (int *)&is_linked);
		if (is_linked == GL_FALSE)
		{
			GLint max_length = 0;
			glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &max_length);

			// The max_length includes the NULL character
			std::vector<GLchar> info_log(static_cast<std::size_t>(max_length));
			glGetProgramInfoLog(id_, max_length, &max_length, &info_log[0]);
			
			// We don't need the program anymore.
			glDeleteProgram(id_);

			// Use the info_log as you see fit.
			Error("Error during shader linkage:\n");
			Error("%s\n", &info_log[0]);
			
			// In this simple program, we'll just leave
			return false;
		}

		// Always detach shaders after a successful link.
		for (const auto& shader : shaders)
			if (shader.has_value())
				glDetachShader(id_, shader.value().id());

		return true;
	}
	void OpenGLProgram::Destroy()
	{
		if (id_ != 0u)
		{
			glDeleteProgram(id_);
			id_ = 0u;
		}
	}
	GLuint OpenGLProgram::id() const
	{
		return id_;
	}

} // namespace scythe