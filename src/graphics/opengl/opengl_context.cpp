#include "opengl_context.h"
#include "opengl_include.h"

namespace scythe {

	OpenGlContext::OpenGlContext()
	{
		FillTables();
	}
	OpenGlContext::~OpenGlContext()
	{
		
	}
	void OpenGlContext::FillTables()
	{
		primitive_type_map_[PrimitiveType::kLines] = GL_LINES;
		primitive_type_map_[PrimitiveType::kLineStrip] = GL_LINE_STRIP;
		primitive_type_map_[PrimitiveType::kTriangles] = GL_TRIANGLES;
		primitive_type_map_[PrimitiveType::kTriangleStrip] = GL_TRIANGLE_STRIP;
		primitive_type_map_[PrimitiveType::kQuads] = GL_QUADS;
		assert(primitive_type_map_.size() == (size_t)PrimitiveType::kCount);
		
		data_type_map_[DataType::kUnsignedShort] = GL_UNSIGNED_SHORT;
		data_type_map_[DataType::kUnsignedInt] = GL_UNSIGNED_INT;
		data_type_map_[DataType::kFloat] = GL_FLOAT;
		assert(data_type_map_.size() == (size_t)DataType::kCount);
		
		data_access_map_[DataAccessType::kRead] = GL_READ_ONLY;
		data_access_map_[DataAccessType::kWrite] = GL_WRITE_ONLY;
		data_access_map_[DataAccessType::kReadWrite] = GL_READ_WRITE;
		assert(data_access_map_.size() == (size_t)DataAccessType::kCount);
		
		buffer_usage_map_[BufferUsage::kStaticDraw] = GL_STATIC_DRAW;
		buffer_usage_map_[BufferUsage::kStaticRead] = GL_STATIC_READ;
		buffer_usage_map_[BufferUsage::kStaticCopy] = GL_STATIC_COPY;
		buffer_usage_map_[BufferUsage::kDynamicDraw] = GL_DYNAMIC_DRAW;
		buffer_usage_map_[BufferUsage::kDynamicRead] = GL_DYNAMIC_READ;
		buffer_usage_map_[BufferUsage::kDynamicCopy] = GL_DYNAMIC_COPY;
		buffer_usage_map_[BufferUsage::kStreamDraw] = GL_STREAM_DRAW;
		buffer_usage_map_[BufferUsage::kStreamRead] = GL_STREAM_READ;
		buffer_usage_map_[BufferUsage::kStreamCopy] = GL_STREAM_COPY;
		assert(buffer_usage_map_.size() == (size_t)BufferUsage::kCount);
		
		cull_face_map_[CullFaceType::kBack] = GL_BACK;
		cull_face_map_[CullFaceType::kFront] = GL_FRONT;
		assert(cull_face_map_.size() == (size_t)CullFaceType::kCount);
	}
	bool OpenGlContext::CheckForErrors()
	{
#if defined(_DEBUG) || defined(DEBUG)
		int error = glGetError();
		if (error != GL_NO_ERROR){
			if (error == GL_INVALID_ENUM)
			{
				ErrorHandler("GL_INVALID_ENUM");
			}
			else if (error == GL_INVALID_VALUE){
				ErrorHandler("GL_INVALID_VALUE");
			}
			else if (error == GL_INVALID_OPERATION){
				ErrorHandler("GL_INVALID_OPERATION");
			}
			else if (error == GL_OUT_OF_MEMORY){
				ErrorHandler("GL_OUT_OF_MEMORY");
			}
			else if (error == GL_INVALID_FRAMEBUFFER_OPERATION_EXT){
				ErrorHandler("GL_INVALID_FRAMEBUFFER_OPERATION_EXT");
			}
			else {
				char str[32];
				sprintf(str, "Unknown OpenGL error: %d", error);
				ErrorHandler(str);
			}
			return true;
		}
#endif
		return false;
	}
	bool OpenGlContext::CheckFrameBufferStatus()
	{
#if defined(_DEBUG) || defined(DEBUG)
		GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		switch (status){
			case GL_FRAMEBUFFER_COMPLETE_EXT:
				return true;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
				ErrorHandler("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT");
				return false;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
				ErrorHandler("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT");
				return false;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
				ErrorHandler("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT");
				return false;
			case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
				ErrorHandler("GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT");
				return false;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
				ErrorHandler("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT");
				return false;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
				ErrorHandler("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT");
				return false;
			case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
				ErrorHandler("GL_FRAMEBUFFER_UNSUPPORTED_EXT");
				return false;
			default:
				ErrorHandler("Unknown FBO error");
				return false;
		}
#else
		return true;
#endif
	}
	void OpenGlContext::ClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}
	void OpenGlContext::ClearColorBuffer()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}
	void OpenGlContext::ClearDepthBuffer()
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	void OpenGlContext::ClearColorAndDepthBuffers()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void OpenGlContext::ClearStencil(int value)
	{
		glClearStencil(value);
	}
	void OpenGlContext::ClearStencilBuffer()
	{
		glClear(GL_STENCIL_BUFFER_BIT);
	}
	void OpenGlContext::Viewport(int w, int h)
	{
		glViewport(0, 0, w, h);
	}
	void OpenGlContext::EnableBlend()
	{
		glEnable(GL_BLEND);
	}
	void OpenGlContext::DisableBlend()
	{
		glDisable(GL_BLEND);
	}
	void OpenGlContext::EnableDepthTest()
	{
		glEnable(GL_DEPTH_TEST);
	}
	void OpenGlContext::DisableDepthTest()
	{
		glDisable(GL_DEPTH_TEST);
	}
	void OpenGlContext::EnableDepthWrite()
	{
		glDepthMask(GL_TRUE);
	}
	void OpenGlContext::DisableDepthWrite()
	{
		glDepthMask(GL_FALSE);
	}
	void OpenGlContext::EnableStencilTest()
	{
		glEnable(GL_STENCIL_TEST);
	}
	void OpenGlContext::DisableStencilTest()
	{
		glDisable(GL_STENCIL_TEST);
	}
	void OpenGlContext::StencilMask(U32 mask)
	{
		glStencilMask(mask);
	}
	void OpenGlContext::EnableWireframeMode()
	{
		glPolygonMode(GL_FRONT, GL_LINE);
	}
	void OpenGlContext::DisableWireframeMode()
	{
		glPolygonMode(GL_FRONT, GL_FILL);
	}
	void OpenGlContext::CullFace(CullFaceType mode)
	{
		U32 cull_face = cull_face_map_[mode];
		glCullFace(cull_face);
	}
	void OpenGlContext::DrawArrays(PrimitiveType mode, int first, U32 count)
	{
		U32 primitive_type = primitive_type_map_[mode];
		glDrawArrays(primitive_type, first, count);
	}
	void OpenGlContext::DrawElements(PrimitiveType mode, U32 num_indices, DataType index_type)
	{
		U32 primitive_type = primitive_type_map_[mode];
		U32 data_type = data_type_map_[index_type];
		glDrawElements(primitive_type, num_indices, data_type, 0);
	}
	void OpenGlContext::GenVertexArrayObject(U32 &obj)
	{
		glGenVertexArrays(1, &obj);
	}
	void OpenGlContext::DeleteVertexArrayObject(U32 &obj)
	{
		glDeleteVertexArrays(1, &obj);
	}
	void OpenGlContext::BindVertexArrayObject(U32 obj)
	{
		glBindVertexArray(obj);
	}
	void OpenGlContext::GenVertexBuffer(U32& obj)
	{
		glGenBuffers(1, &obj);
	}
	void OpenGlContext::DeleteVertexBuffer(U32& obj)
	{
		glDeleteBuffers(1, &obj);
	}
	void OpenGlContext::BindVertexBuffer(U32 obj)
	{
		glBindBuffer(GL_ARRAY_BUFFER, obj);
	}
	void OpenGlContext::VertexBufferData(U32 size, const void *data, BufferUsage usage)
	{
		U32 usage_type = buffer_usage_map_[usage];
		glBufferData(GL_ARRAY_BUFFER, size, data, usage_type);
	}
	void OpenGlContext::VertexBufferSubData(U32 size, const void *data)
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}
	void* OpenGlContext::MapVertexBufferData(DataAccessType access)
	{
		U32 access_type = data_access_map_[access];
		return glMapBuffer(GL_ARRAY_BUFFER, access_type);
	}
	void OpenGlContext::UnmapVertexBufferData()
	{
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
	void OpenGlContext::GenIndexBuffer(U32& obj)
	{
		glGenBuffers(1, &obj);
	}
	void OpenGlContext::DeleteIndexBuffer(U32& obj)
	{
		glDeleteBuffers(1, &obj);
	}
	void OpenGlContext::BindIndexBuffer(U32 obj)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj);
	}
	void OpenGlContext::IndexBufferData(U32 size, const void *data, BufferUsage usage)
	{
		U32 usage_type = buffer_usage_map_[usage];
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage_type);
	}
	void OpenGlContext::IndexBufferSubData(U32 size, const void *data)
	{
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, data);
	}
	void* OpenGlContext::MapIndexBufferData(DataAccessType access)
	{
		U32 access_type = data_access_map_[access];
		return glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, access_type);
	}
	void OpenGlContext::UnmapIndexBufferData()
	{
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	}
	void OpenGlContext::VertexAttribPointer(U32 index, int size, DataType type, U32 stride, const void* ptr)
	{
		U32 data_type = data_type_map_[type];
		glVertexAttribPointer(index, size, data_type, GL_FALSE, stride, ptr);
	}
	void OpenGlContext::EnableVertexAttribArray(U32 index)
	{
		glEnableVertexAttribArray(index);
	}
	void OpenGlContext::DeleteProgram(U32 program)
	{
		glDeleteProgram(program);
	}
	void OpenGlContext::BindProgram(U32 program)
	{
		glUseProgram(program);
	}
	void OpenGlContext::BindAttribLocation(U32 program, const char *name)
	{
		GLint ind = glGetAttribLocation(program, name);
		assert(ind != -1);
		glBindAttribLocation(program, ind, name);
	}
	void OpenGlContext::Uniform1i(U32 program, const char *name, int x)
	{
		GLint location = glGetUniformLocation(program, name);
		assert(location != -1);
		glUniform1i(location, x);
	}
	void OpenGlContext::Uniform1iv(U32 program, const char *name, const int *v, int n)
	{
		GLint location = glGetUniformLocation(program, name);
		assert(location != -1);
		glUniform1iv(location, n, v);
	}
	void OpenGlContext::Uniform2i(U32 program, const char *name, int x, int y)
	{
		GLint location = glGetUniformLocation(program, name);
		assert(location != -1);
		glUniform2i(location, x, y);
	}
	void OpenGlContext::Uniform3i(U32 program, const char *name, int x, int y, int z)
	{
		GLint location = glGetUniformLocation(program, name);
		assert(location != -1);
		glUniform3i(location, x, y, z);
	}
	void OpenGlContext::Uniform4i(U32 program, const char *name, int x, int y, int z, int w)
	{
		GLint location = glGetUniformLocation(program, name);
		assert(location != -1);
		glUniform4i(location, x, y, z, w);
	}
	void OpenGlContext::Uniform1f(U32 program, const char *name, float x)
	{
		GLint location = glGetUniformLocation(program, name);
		assert(location != -1);
		glUniform1f(location, x);
	}
	void OpenGlContext::Uniform2f(U32 program, const char *name, float x, float y)
	{
		GLint location = glGetUniformLocation(program, name);
		assert(location != -1);
		glUniform2f(location, x, y);
	}
	void OpenGlContext::Uniform3f(U32 program, const char *name, float x, float y, float z)
	{
		GLint location = glGetUniformLocation(program, name);
		assert(location != -1);
		glUniform3f(location, x, y, z);
	}
	void OpenGlContext::Uniform4f(U32 program, const char *name, float x, float y, float z, float w)
	{
		GLint location = glGetUniformLocation(program, name);
		assert(location != -1);
		glUniform4f(location, x, y, z, w);
	}
	void OpenGlContext::Uniform1fv(U32 program, const char *name, const float *v, int n)
	{
		GLint location = glGetUniformLocation(program, name);
		assert(location != -1);
		glUniform1fv(location, n, v);
	}
	void OpenGlContext::Uniform2fv(U32 program, const char *name, const float *v, int n)
	{
		GLint location = glGetUniformLocation(program, name);
		assert(location != -1);
		glUniform2fv(location, n, v);
	}
	void OpenGlContext::Uniform3fv(U32 program, const char *name, const float *v, int n)
	{
		GLint location = glGetUniformLocation(program, name);
		assert(location != -1);
		glUniform3fv(location, n, v);
	}
	void OpenGlContext::Uniform4fv(U32 program, const char *name, const float *v, int n)
	{
		GLint location = glGetUniformLocation(program, name);
		assert(location != -1);
		glUniform4fv(location, n, v);
	}
	void OpenGlContext::UniformMatrix2fv(U32 program, const char *name, const float *v, bool trans, int n)
	{
		GLint location = glGetUniformLocation(program, name);
		assert(location != -1);
		glUniformMatrix2fv(location, n, trans, v);
	}
	void OpenGlContext::UniformMatrix3fv(U32 program, const char *name, const float *v, bool trans, int n)
	{
		GLint location = glGetUniformLocation(program, name);
		assert(location != -1);
		glUniformMatrix3fv(location, n, trans, v);
	}
	void OpenGlContext::UniformMatrix4fv(U32 program, const char *name, const float *v, bool trans, int n)
	{
		GLint location = glGetUniformLocation(program, name);
		assert(location != -1);
		glUniformMatrix4fv(location, n, trans, v);
	}

} // namespace scythe