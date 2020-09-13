#include "shader.h"

#include "opengl/opengl_include.h"
#include "stream/file_stream.h"
#include "common/sc_assert.h"
#include "common/string_format.h"

#include <string>

namespace scythe {

	Shader::Shader(Context * context)
	: context_(context)
	, program_(0)
	{
	}
	Shader::~Shader()
	{
		context_->DeleteProgram(program_);
	}
	void Shader::Bind()
	{
		context_->BindProgram(program_);
	}
	void Shader::Unbind()
	{
		context_->BindProgram(0);
	}
	void Shader::Uniform1i(const char *name, int x)
	{
		context_->Uniform1i(program_, name, x);
	}
	void Shader::Uniform2i(const char *name, int x, int y)
	{
		context_->Uniform2i(program_, name, x, y);
	}
	void Shader::Uniform3i(const char *name, int x, int y, int z)
	{
		context_->Uniform3i(program_, name, x, y, z);
	}
	void Shader::Uniform4i(const char *name, int x, int y, int z, int w)
	{
		context_->Uniform4i(program_, name, x, y, z, w);
	}
	void Shader::Uniform1f(const char *name, float x)
	{
		context_->Uniform1f(program_, name, x);
	}
	void Shader::Uniform2f(const char *name, float x, float y)
	{
		context_->Uniform2f(program_, name, x, y);
	}
	void Shader::Uniform3f(const char *name, float x, float y, float z)
	{
		context_->Uniform3f(program_, name, x, y, z);
	}
	void Shader::Uniform4f(const char *name, float x, float y, float z, float w)
	{
		context_->Uniform4f(program_, name, x, y, z, w);
	}
	void Shader::Uniform1fv(const char *name, const float *v, int n)
	{
		context_->Uniform1fv(program_, name, v, n);
	}
	void Shader::Uniform2fv(const char *name, const float *v, int n)
	{
		context_->Uniform2fv(program_, name, v, n);
	}
	void Shader::Uniform3fv(const char *name, const float *v, int n)
	{
		context_->Uniform3fv(program_, name, v, n);
	}
	void Shader::Uniform4fv(const char *name, const float *v, int n)
	{
		context_->Uniform4fv(program_, name, v, n);
	}
	void Shader::UniformMatrix2fv(const char *name, const float *v, bool trans, int n)
	{
		context_->UniformMatrix2fv(program_, name, v, trans, n);
	}
	void Shader::UniformMatrix3fv(const char *name, const float *v, bool trans, int n)
	{
		context_->UniformMatrix3fv(program_, name, v, trans, n);
	}
	void Shader::UniformMatrix4fv(const char *name, const float *v, bool trans, int n)
	{
		context_->UniformMatrix4fv(program_, name, v, trans, n);
	}
	static void InsertAfterVersion(std::string& str, const std::string& str_to_insert)
	{
		size_t pos = str.find("#version", 0);
		if (pos != std::string::npos)
		{
			size_t len = str.length();
			pos = str.find("\n", pos);
			while (pos < len && (str[pos] == '\n' || str[pos] == '\r'))
				++pos;
		}
		else
		{
			// No #version declared in shader, insert in the beginning
			pos = 0;
		}
		str.insert(pos, str_to_insert);
	}
	Shader * Shader::Create(Context * context, const ShaderInfo& info)
	{
		Shader * shader = new Shader(context);
		
		GLint success;
		FileStream stream;
		std::string shader_source;
		std::string shader_filename;
		
		U32 vertex_shader, fragment_shader;

		// Defines source
		std::string defines_source;
		for (U32 i = 0; i < info.num_defines_; ++i)
		{
			const char* define_str = info.defines_[i];
			size_t len = strlen(define_str);
			if (define_str == nullptr || len == 0)
				continue;
			// Check if define was included
			if (strstr(define_str, "#define"))
			{
				// Nothing to worry about
				defines_source.append(define_str);
			}
			else
			{
				defines_source.append(scythe::string_format("#define %s", define_str));
			}
			// Check if define string has end line symbol
			if (define_str[len-1] != '\n' && define_str[len-1] != '\r')
				defines_source.append("\n");
		}
		
		// Vertex program
		if (info.base_filename_)
		{
			shader_filename = info.base_filename_;
			shader_filename += ".vs";
		}
		else
		{
			SC_ASSERT(info.vertex_program_);
			shader_filename = info.vertex_program_;
		}
		if (stream.Open(shader_filename.c_str(), StreamAccess::kReadBinary))
		{
			shader_source.resize(stream.Length());
			if (!stream.Read(&shader_source[0], shader_source.size()))
			{
				context->ErrorHandler("Failed to read from vertex shader file");
				delete shader;
				return nullptr;
			}
			stream.Close();

			if (info.defines_)
				InsertAfterVersion(shader_source, defines_source);
			
			vertex_shader = glCreateShader(GL_VERTEX_SHADER);
			const char * source = shader_source.c_str();
			glShaderSource(vertex_shader, 1, &source, NULL);
			glCompileShader(vertex_shader);
			glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				char infoLog[2048];
				glGetShaderInfoLog(vertex_shader, 2048, NULL, infoLog);
				char temp[100];
				sprintf(temp, "%s %s", "Error in vertex shader compilation in", shader_filename.c_str());
				context->ErrorHandler(temp);
				context->ErrorHandler(infoLog);
				glDeleteShader(vertex_shader);
				delete shader;
				return nullptr;
			}
			context->CheckForErrors();
		}
		else
		{
			context->ErrorHandler("Failed to open vertex shader file");
			delete shader;
			return nullptr;
		}
		
		// Fragment program
		if (info.base_filename_)
		{
			shader_filename = info.base_filename_;
			shader_filename += ".fs";
		}
		else
		{
			SC_ASSERT(info.fragment_program_);
			shader_filename = info.fragment_program_;
		}
		if (stream.Open(shader_filename.c_str(), StreamAccess::kReadBinary))
		{
			shader_source.resize(stream.Length());
			if (!stream.Read(&shader_source[0], shader_source.size()))
			{
				context->ErrorHandler("Failed to read from fragment shader file");
				delete shader;
				return nullptr;
			}
			stream.Close();

			if (info.defines_)
				InsertAfterVersion(shader_source, defines_source);
			
			fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
			const char * source = shader_source.c_str();
			glShaderSource(fragment_shader, 1, &source, NULL);
			glCompileShader(fragment_shader);
			glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				char infoLog[2048];
				glGetShaderInfoLog(fragment_shader, 2048, NULL, infoLog);
				char temp[100];
				sprintf(temp, "%s %s", "Error in fragment shader compilation in", shader_filename.c_str());
				context->ErrorHandler(temp);
				context->ErrorHandler(infoLog);
				glDeleteShader(fragment_shader);
				glDeleteShader(vertex_shader);
				delete shader;
				return nullptr;
			}
			context->CheckForErrors();
		}
		else
		{
			context->ErrorHandler("Failed to open fragment shader file");
			delete shader;
			return nullptr;
		}
		
		// Create program object, attach shader, then link
		shader->program_ = glCreateProgram();
		glAttachShader(shader->program_, vertex_shader);
		glAttachShader(shader->program_, fragment_shader);
		for (U32 i = 0; i < info.num_attributes_; ++i)
		{
			if (info.attributes_[i])
			{
				glBindAttribLocation(shader->program_, i, info.attributes_[i]);
				context->CheckForErrors();
			}
		}
		glLinkProgram(shader->program_);
		glGetProgramiv(shader->program_, GL_LINK_STATUS, &success);
		if (!success)
		{
			char infoLog[2048];
			glGetProgramInfoLog(shader->program_, 2048, NULL, infoLog);
			char temp[100];
			sprintf(temp, "%s %s", "Error in shader linkage in", shader_filename.c_str());
			context->ErrorHandler(temp);
			context->ErrorHandler(infoLog);
			glDeleteProgram(shader->program_);
			glDeleteShader(vertex_shader);
			glDeleteShader(fragment_shader);
			delete shader;
			return nullptr;
		}
		context->CheckForErrors();
		
		// After linkage these objects may be deleted
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		
		return shader;
	}

} // namespace scythe