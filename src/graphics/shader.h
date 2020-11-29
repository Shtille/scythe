#ifndef __SCYTHE_SHADER_H__
#define __SCYTHE_SHADER_H__

#include "shader_info.h"
#include "resource.h"
#include "context.h"

namespace scythe {

	//! Shader class
	class Shader : public Resource {
		friend class Renderer;
		friend class OpenGlRenderer;
		
	public:
		void Bind();
		void Unbind();
		
		void Uniform1i(const char *name, int x);
		void Uniform2i(const char *name, int x, int y);
		void Uniform3i(const char *name, int x, int y, int z);
		void Uniform4i(const char *name, int x, int y, int z, int w);
		void Uniform1iv(const char *name, const int *v, int n);
		void Uniform1f(const char *name, float x);
		void Uniform2f(const char *name, float x, float y);
		void Uniform3f(const char *name, float x, float y, float z);
		void Uniform4f(const char *name, float x, float y, float z, float w);
		void Uniform1fv(const char *name, const float *v, int n = 1);
		void Uniform2fv(const char *name, const float *v, int n = 1);
		void Uniform3fv(const char *name, const float *v, int n = 1);
		void Uniform4fv(const char *name, const float *v, int n = 1);
		void UniformMatrix2fv(const char *name, const float *v, bool trans = false, int n = 1);
		void UniformMatrix3fv(const char *name, const float *v, bool trans = false, int n = 1);
		void UniformMatrix4fv(const char *name, const float *v, bool trans = false, int n = 1);
		
	protected:
		Shader(Context * context);
		~Shader();
		Shader(const Shader&) = delete;
		void operator = (const Shader&) = delete;
		
		Context * context_;
		
	private:
		static Shader * Create(Context * context, const ShaderInfo& info);
		
		U32 program_;
	};

} // namespace scythe

#endif