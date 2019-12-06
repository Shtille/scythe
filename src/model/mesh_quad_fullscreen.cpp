#include "mesh.h"

#include "mesh_part.h"

namespace scythe {

	void Mesh::CreateQuadFullscreen()
	{
		MeshPart * mesh_part = new MeshPart(renderer_);
		auto& vertices = mesh_part->vertices_;
		auto& indices = mesh_part->indices_;

		vertices.resize(4);
		vertices[0].position.Set(-1.0f,  -1.0f,  0.0f);
		vertices[1].position.Set( 1.0f,  -1.0f,  0.0f);
		vertices[2].position.Set(-1.0f,   1.0f,  0.0f);
		vertices[3].position.Set( 1.0f,   1.0f,  0.0f);
		
		indices.resize(4);
		indices = {
			0,1,2,3
		};

		meshes_.push_back(mesh_part);
	}

} // namespace scythe {