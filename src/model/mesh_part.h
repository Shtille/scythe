#ifndef __SCYTHE_MESH_PART_H__
#define __SCYTHE_MESH_PART_H__

#include "vertex.h"
#include "math/bounding_box.h"
#include "graphics/renderer.h"

#include <vector>

namespace scythe {

	struct Material;

	/**
	 * Defines a part of a mesh describing the way the
	 * mesh's vertices are connected together.
	 */
	class MeshPart {
		friend class Mesh;
		friend class MeshVerticesEnumerator;
	public:
		MeshPart(Renderer * renderer);
		virtual ~MeshPart();

		bool MakeRenderable(VertexFormat * vertex_format, const std::vector<VertexAttribute>& attribs, BoundingBox& bounding_box);
		
		void Render();

		void ScaleVertices(const Vector3& scale);
		void ScaleTexcoord(const Vector2& scale);
		
	protected:
		std::vector<Vertex> vertices_;
		std::vector<U32> indices_;
		
		PrimitiveType primitive_mode_;
		
	private:
		void FreeArrays();
		void TransformVertices(VertexFormat * vertex_format, const std::vector<VertexAttribute>& attribs, BoundingBox& bounding_box);
		
		Renderer * renderer_;
		Material * material_;
		VertexBuffer * vertex_buffer_;
		IndexBuffer * index_buffer_;
		U32 vertex_array_object_;
		
		U32 num_vertices_;
		U8 * vertices_array_;
		U32 num_indices_;
		U32 index_size_;
		U8 * indices_array_;
		DataType index_data_type_;
	};

} // namespace scythe

#endif