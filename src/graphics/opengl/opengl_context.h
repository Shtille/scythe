#ifndef __SCYTHE_OPENGL_CONTEXT_H__
#define __SCYTHE_OPENGL_CONTEXT_H__

#include "../context.h"

namespace scythe {

	class OpenGlContext final : public Context {
	public:
		OpenGlContext();
		~OpenGlContext();

		bool CheckForErrors();
			bool CheckFrameBufferStatus();

		void ClearColor(float r, float g, float b, float a);
		void ClearColorBuffer();
		void ClearDepthBuffer();
		void ClearColorAndDepthBuffers();
		void ClearStencil(int value);
		void ClearStencilBuffer();

		void Viewport(int w, int h);

		void EnableBlend();
		void DisableBlend();

		void EnableDepthTest();
		void DisableDepthTest();
		void EnableDepthWrite();
		void DisableDepthWrite();

		void EnableStencilTest();
		void DisableStencilTest();
		void StencilMask(U32 mask);

		void EnableWireframeMode();
		void DisableWireframeMode();

		void CullFace(CullFaceType mode);

		void DrawArrays(PrimitiveType mode, int first, U32 count);
		void DrawElements(PrimitiveType mode, U32 num_indices, DataType index_type);

		// Vertex array object
		void GenVertexArrayObject(U32 &obj);
		void DeleteVertexArrayObject(U32 &obj);
		void BindVertexArrayObject(U32 obj);

		// Vertex buffer object
		void GenVertexBuffer(U32& obj);
		void DeleteVertexBuffer(U32& obj);
		void BindVertexBuffer(U32 obj);
		void VertexBufferData(U32 size, const void *data, BufferUsage usage);
		void VertexBufferSubData(U32 size, const void *data);
		void* MapVertexBufferData(DataAccessType access);
		void UnmapVertexBufferData();

		// Index buffer object
		void GenIndexBuffer(U32& obj);
		void DeleteIndexBuffer(U32& obj);
		void BindIndexBuffer(U32 obj);
		void IndexBufferData(U32 size, const void *data, BufferUsage usage);
		void IndexBufferSubData(U32 size, const void *data);
		void* MapIndexBufferData(DataAccessType access);
		void UnmapIndexBufferData();

		// Vertex attribs
		void VertexAttribPointer(U32 index, int size, DataType type, U32 stride, const void* ptr);
		void EnableVertexAttribArray(U32 index);

		// Shader
		void DeleteProgram(U32 program);
		void BindProgram(U32 program);
		void BindAttribLocation(U32 program, const char *name);
		void Uniform1i(U32 program, const char *name, int x);
		void Uniform2i(U32 program, const char *name, int x, int y);
		void Uniform3i(U32 program, const char *name, int x, int y, int z);
		void Uniform4i(U32 program, const char *name, int x, int y, int z, int w);
		void Uniform1iv(U32 program, const char *name, const int *v, int n);
		void Uniform1f(U32 program, const char *name, float x);
		void Uniform2f(U32 program, const char *name, float x, float y);
		void Uniform3f(U32 program, const char *name, float x, float y, float z);
		void Uniform4f(U32 program, const char *name, float x, float y, float z, float w);
		void Uniform1fv(U32 program, const char *name, const float *v, int n = 1);
		void Uniform2fv(U32 program, const char *name, const float *v, int n = 1);
		void Uniform3fv(U32 program, const char *name, const float *v, int n = 1);
		void Uniform4fv(U32 program, const char *name, const float *v, int n = 1);
		void UniformMatrix2fv(U32 program, const char *name, const float *v, bool trans = false, int n = 1);
		void UniformMatrix3fv(U32 program, const char *name, const float *v, bool trans = false, int n = 1);
		void UniformMatrix4fv(U32 program, const char *name, const float *v, bool trans = false, int n = 1);

	protected:
		void FillTables();
	};

} // namespace scythe

#endif