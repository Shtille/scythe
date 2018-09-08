#ifndef __SCYTHE_GENERATED_MODEL_H__
#define __SCYTHE_GENERATED_MODEL_H__

#include "vertex.h"

#include "graphics/vertex_format.h"
#include "graphics/renderer.h"

#include <vector>

namespace scythe {

	//! Standart model class
	class GeneratedModel {
	public:
		GeneratedModel(Renderer * renderer);
		virtual ~GeneratedModel();
		
		virtual void Create() = 0;
		void AddFormat(const VertexAttribute& attrib);
		bool MakeRenderable();
		
		bool HasTexture() const;
		
		void Render();

		void ScaleVertices(const Vector3& scale);
		void ScaleTexcoord(const Vector2& scale);

		void ComputeTangentBasis();
		
	protected:            
		std::vector<Vertex> vertices_;
		std::vector<U32> indices_;
		
		PrimitiveType primitive_mode_;

	private:
		void FreeArrays();
		void TransformVertices();
		
		Renderer * renderer_;
		VertexFormat * vertex_format_;
		VertexBuffer * vertex_buffer_;
		IndexBuffer * index_buffer_;
		U32 vertex_array_object_;
		
		U32 num_vertices_;
		U8 * vertices_array_;
		U32 num_indices_;
		U32 index_size_;
		U8 * indices_array_;
		DataType index_data_type_;
		
		std::vector<VertexAttribute> attribs_;
	};

} // namespace scythe

#endif