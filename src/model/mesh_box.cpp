#include "mesh.h"

#include "mesh_part.h"

namespace scythe {

	void Mesh::CreateBox(float size_x, float size_y, float size_z)
	{
		MeshPart * mesh_part = new MeshPart(renderer_);
		auto& vertices = mesh_part->vertices_;
		auto& indices = mesh_part->indices_;

		vertices.resize(24);
		
		// +X side
		vertices[0].position.Set(1.0f,  1.0f,  1.0f);
		vertices[0].normal.Set(1.0f,  0.0f,  0.0f);
		vertices[0].texcoord.Set(0.0f,  1.0f);
		vertices[1].position.Set(1.0f, -1.0f,  1.0f);
		vertices[1].normal.Set(1.0f,  0.0f,  0.0f);
		vertices[1].texcoord.Set(0.0f,  0.0f);
		vertices[2].position.Set(1.0f,  1.0f, -1.0f);
		vertices[2].normal.Set(1.0f,  0.0f,  0.0f);
		vertices[2].texcoord.Set(1.0f,  1.0f);
		vertices[3].position.Set(1.0f, -1.0f, -1.0f);
		vertices[3].normal.Set(1.0f,  0.0f,  0.0f);
		vertices[3].texcoord.Set(1.0f,  0.0f);
		
		// -Z side
		vertices[4].position.Set( 1.0f,  1.0f, -1.0f);
		vertices[4].normal.Set(0.0f,  0.0f, -1.0f);
		vertices[4].texcoord.Set(0.0f,  1.0f);
		vertices[5].position.Set( 1.0f, -1.0f, -1.0f);
		vertices[5].normal.Set(0.0f,  0.0f, -1.0f);
		vertices[5].texcoord.Set(0.0f,  0.0f);
		vertices[6].position.Set(-1.0f,  1.0f, -1.0f);
		vertices[6].normal.Set(0.0f,  0.0f, -1.0f);
		vertices[6].texcoord.Set(1.0f,  1.0f);
		vertices[7].position.Set(-1.0f, -1.0f, -1.0f);
		vertices[7].normal.Set(0.0f,  0.0f, -1.0f);
		vertices[7].texcoord.Set(1.0f,  0.0f);
		
		// -X side
		vertices[8 ].position.Set(-1.0f,  1.0f, -1.0f);
		vertices[8 ].normal.Set(-1.0f,  0.0f,  0.0f);
		vertices[8 ].texcoord.Set(0.0f,  1.0f);
		vertices[9 ].position.Set(-1.0f, -1.0f, -1.0f);
		vertices[9 ].normal.Set(-1.0f,  0.0f,  0.0f);
		vertices[9 ].texcoord.Set(0.0f,  0.0f);
		vertices[10].position.Set(-1.0f,  1.0f,  1.0f);
		vertices[10].normal.Set(-1.0f,  0.0f,  0.0f);
		vertices[10].texcoord.Set(1.0f,  1.0f);
		vertices[11].position.Set(-1.0f, -1.0f,  1.0f);
		vertices[11].normal.Set(-1.0f,  0.0f,  0.0f);
		vertices[11].texcoord.Set(1.0f,  0.0f);
		
		// +Z side
		vertices[12].position.Set(-1.0f,  1.0f,  1.0f);
		vertices[12].normal.Set(0.0f,  0.0f,  1.0f);
		vertices[12].texcoord.Set(0.0f,  1.0f);
		vertices[13].position.Set(-1.0f, -1.0f,  1.0f);
		vertices[13].normal.Set(0.0f,  0.0f,  1.0f);
		vertices[13].texcoord.Set(0.0f,  0.0f);
		vertices[14].position.Set( 1.0f,  1.0f,  1.0f);
		vertices[14].normal.Set(0.0f,  0.0f,  1.0f);
		vertices[14].texcoord.Set(1.0f,  1.0f);
		vertices[15].position.Set( 1.0f, -1.0f,  1.0f);
		vertices[15].normal.Set(0.0f,  0.0f,  1.0f);
		vertices[15].texcoord.Set(1.0f,  0.0f);
		
		// +Y side
		vertices[16].position.Set( 1.0f,  1.0f,  1.0f);
		vertices[16].normal.Set(0.0f,  1.0f,  0.0f);
		vertices[16].texcoord.Set(1.0f,  1.0f);
		vertices[17].position.Set( 1.0f,  1.0f, -1.0f);
		vertices[17].normal.Set(0.0f,  1.0f,  0.0f);
		vertices[17].texcoord.Set(0.0f,  1.0f);
		vertices[18].position.Set(-1.0f,  1.0f,  1.0f);
		vertices[18].normal.Set(0.0f,  1.0f,  0.0f);
		vertices[18].texcoord.Set(1.0f,  0.0f);
		vertices[19].position.Set(-1.0f,  1.0f, -1.0f);
		vertices[19].normal.Set(0.0f,  1.0f,  0.0f);
		vertices[19].texcoord.Set(0.0f,  0.0f);
		
		// -Y side
		vertices[20].position.Set( 1.0f, -1.0f, -1.0f);
		vertices[20].normal.Set(0.0f, -1.0f,  0.0f);
		vertices[20].texcoord.Set(1.0f,  1.0f);
		vertices[21].position.Set( 1.0f, -1.0f,  1.0f);
		vertices[21].normal.Set(0.0f, -1.0f,  0.0f);
		vertices[21].texcoord.Set(0.0f,  1.0f);
		vertices[22].position.Set(-1.0f, -1.0f, -1.0f);
		vertices[22].normal.Set(0.0f, -1.0f,  0.0f);
		vertices[22].texcoord.Set(1.0f,  0.0f);
		vertices[23].position.Set(-1.0f, -1.0f,  1.0f);
		vertices[23].normal.Set(0.0f, -1.0f,  0.0f);
		vertices[23].texcoord.Set(0.0f,  0.0f);

		// Scale cube to box
		mesh_part->ScaleVertices(Vector3(size_x, size_y, size_z));
		
		indices.resize(34);
		indices = {
			0,1,2,3, 3,4,
			4,5,6,7, 7,8,
			8,9,10,11, 11,12,
			12,13,14,15, 15,16,
			16,17,18,19, 19,20,
			20,21,22,23
		};

		meshes_.push_back(mesh_part);
	}

} // namespace scythe {