#ifndef __SCYTHE_CONTEXT_H__
#define __SCYTHE_CONTEXT_H__

#include "common/types.h"
#include "common/enum_table.h"

namespace scythe {

	enum class PrimitiveType {
		kLines,
		kLineStrip,
		kTriangles,
		kTriangleStrip,
		kQuads,
		kCount
	};
	
	enum class DataType {
		kUnsignedShort,
		kUnsignedInt,
		kFloat,
		kCount
	};
	
	enum class DataAccessType {
		kRead,
		kWrite,
		kReadWrite,
		kCount
	};
	
	enum class BufferUsage {
		kStaticDraw,
		kStaticRead,
		kStaticCopy,
		kStreamDraw,
		kStreamRead,
		kStreamCopy,
		kDynamicDraw,
		kDynamicRead,
		kDynamicCopy,
		kCount
	};
	
	enum class CullFaceType {
		kBack,
		kFront,
		kCount
	};
	
	class Context {
	public:
		Context();
		virtual ~Context();
		
		void ErrorHandler(const char *message);
		
		virtual bool CheckForErrors() = 0;
		virtual bool CheckFrameBufferStatus() = 0;
		
		virtual void ClearColor(F32 r, F32 g, F32 b, F32 a) = 0;
		virtual void ClearColorBuffer() = 0;
		virtual void ClearDepthBuffer() = 0;
		virtual void ClearColorAndDepthBuffers() = 0;
		virtual void ClearStencil(S32 value) = 0;
		virtual void ClearStencilBuffer() = 0;
		
		virtual void Viewport(int w, int h) = 0;
		
		virtual void EnableBlend() = 0;
		virtual void DisableBlend() = 0;
		
		virtual void EnableDepthTest() = 0;
		virtual void DisableDepthTest() = 0;
		virtual void EnableDepthWrite() = 0;
		virtual void DisableDepthWrite() = 0;
		
		virtual void EnableStencilTest() = 0;
		virtual void DisableStencilTest() = 0;
		virtual void StencilMask(U32 mask) = 0;
		//virtual void StencilFunc() = 0;
		//virtual void StencilOp() = 0;
		
		virtual void EnableWireframeMode() = 0;
		virtual void DisableWireframeMode() = 0;
		
		virtual void CullFace(CullFaceType mode) = 0;
		
		virtual void DrawArrays(PrimitiveType mode, S32 first, U32 count) = 0;
		virtual void DrawElements(PrimitiveType mode, U32 num_indices, DataType index_type) = 0;
		
		// Vertex array object
		virtual void GenVertexArrayObject(U32 &obj) = 0;
		virtual void DeleteVertexArrayObject(U32 &obj) = 0;
		virtual void BindVertexArrayObject(U32 obj) = 0;
		
		// Vertex buffer object
		virtual void GenVertexBuffer(U32& obj) = 0;
		virtual void DeleteVertexBuffer(U32& obj) = 0;
		virtual void BindVertexBuffer(U32 obj) = 0;
		virtual void VertexBufferData(U32 size, const void *data, BufferUsage usage) = 0;
		virtual void VertexBufferSubData(U32 size, const void *data) = 0;
		virtual void* MapVertexBufferData(DataAccessType access) = 0;
		virtual void UnmapVertexBufferData() = 0;
		
		// Index buffer object
		virtual void GenIndexBuffer(U32& obj) = 0;
		virtual void DeleteIndexBuffer(U32& obj) = 0;
		virtual void BindIndexBuffer(U32 obj) = 0;
		virtual void IndexBufferData(U32 size, const void *data, BufferUsage usage) = 0;
		virtual void IndexBufferSubData(U32 size, const void *data) = 0;
		virtual void* MapIndexBufferData(DataAccessType access) = 0;
		virtual void UnmapIndexBufferData() = 0;
		
		// Vertex attribs
		virtual void VertexAttribPointer(U32 index, S32 size, DataType type, U32 stride, const void* ptr) = 0;
		virtual void EnableVertexAttribArray(U32 index) = 0;
		
		// Shader
		virtual void DeleteProgram(U32 program) = 0;
		virtual void BindProgram(U32 program) = 0;
		virtual void BindAttribLocation(U32 program, const char *name) = 0;
		virtual void Uniform1i(U32 program, const char *name, int x) = 0;
		virtual void Uniform2i(U32 program, const char *name, int x, int y) = 0;
		virtual void Uniform3i(U32 program, const char *name, int x, int y, int z) = 0;
		virtual void Uniform4i(U32 program, const char *name, int x, int y, int z, int w) = 0;
		virtual void Uniform1f(U32 program, const char *name, float x) = 0;
		virtual void Uniform2f(U32 program, const char *name, float x, float y) = 0;
		virtual void Uniform3f(U32 program, const char *name, float x, float y, float z) = 0;
		virtual void Uniform4f(U32 program, const char *name, float x, float y, float z, float w) = 0;
		virtual void Uniform1fv(U32 program, const char *name, const float *v, int n = 1) = 0;
		virtual void Uniform2fv(U32 program, const char *name, const float *v, int n = 1) = 0;
		virtual void Uniform3fv(U32 program, const char *name, const float *v, int n = 1) = 0;
		virtual void Uniform4fv(U32 program, const char *name, const float *v, int n = 1) = 0;
		virtual void UniformMatrix2fv(U32 program, const char *name, const float *v, bool trans = false, int n = 1) = 0;
		virtual void UniformMatrix3fv(U32 program, const char *name, const float *v, bool trans = false, int n = 1) = 0;
		virtual void UniformMatrix4fv(U32 program, const char *name, const float *v, bool trans = false, int n = 1) = 0;
		
	protected:
		virtual void FillTables() = 0;
		
		EnumTable<PrimitiveType, U32> primitive_type_map_;  //!< primitive type map
		EnumTable<DataType, U32> data_type_map_;            //!< data type map
		EnumTable<DataAccessType, U32> data_access_map_;    //!< data access map
		EnumTable<BufferUsage, U32> buffer_usage_map_;      //!< buffer usage map
		EnumTable<CullFaceType, U32> cull_face_map_;        //!< cull face map
	};

} // namespace scythe

#endif