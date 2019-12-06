#include "mesh.h"

#include "mesh_part.h"

namespace scythe {

	void Mesh::CreateTetrahedron()
	{
		MeshPart * mesh_part = new MeshPart(renderer_);
		auto& vertices = mesh_part->vertices_;
		auto& indices = mesh_part->indices_;

		mesh_part->primitive_mode_ = PrimitiveType::kTriangles;

		/* unique vertices
		 (1, 0, 0)
		 (0, 0, 1)
		 (0, 0,-1)
		 (0, 1, 0)
		 */
		
		const float kS = 1.115355f;
		const float kY = -0.577350f;
		const float kZ = 0.965925f;
		const float kCosA = 0.942809f; // 2√2/3
		const float kSinA = 0.333333f; // 1/3
		const float kCosB = 0.500000f; // cos(60˚)
		const float kSinB = 0.866025f; // sin(60˚)
		
		vertices.resize(12);
		
		// +X+Z side
		vertices[0].position.Set(-0.5f*kS, kY, kZ);
		vertices[0].normal.Set(kCosA*kCosB, kSinA, kCosA*kSinB);
		vertices[1].position.Set(kS, kY, 0.0f);
		vertices[1].normal.Set(kCosA*kCosB, kSinA, kCosA*kSinB);
		vertices[2].position.Set(0.0f,  1.0f,  0.0f);
		vertices[2].normal.Set(kCosA*kCosB, kSinA, kCosA*kSinB);
		
		// +X-Z side
		vertices[3].position.Set(kS, kY, 0.0f);
		vertices[3].normal.Set(kCosA*kCosB, kSinA, -kCosA*kSinB);
		vertices[4].position.Set(-0.5f*kS, kY, -kZ);
		vertices[4].normal.Set(kCosA*kCosB, kSinA, -kCosA*kSinB);
		vertices[5].position.Set( 0.0f,  1.0f,  0.0f);
		vertices[5].normal.Set(kCosA*kCosB, kSinA, -kCosA*kSinB);
		
		// -X side
		vertices[6 ].position.Set(-0.5f*kS, kY, -kZ);
		vertices[6 ].normal.Set(-kCosA, kSinA, 0.0f);
		vertices[7 ].position.Set(-0.5f*kS, kY, kZ);
		vertices[7 ].normal.Set(-kCosA, kSinA, 0.0f);
		vertices[8 ].position.Set( 0.0f,  1.0f,  0.0f);
		vertices[8 ].normal.Set(-kCosA, kSinA, 0.0f);
		
		// -Y side
		vertices[9 ].position.Set(-0.5f*kS, kY, kZ);
		vertices[9 ].normal.Set(0.0f, -1.0f, 0.0f);
		vertices[10].position.Set(-0.5f*kS, kY, -kZ);
		vertices[10].normal.Set(0.0f, -1.0f, 0.0f);
		vertices[11].position.Set(kS, kY, 0.0f);
		vertices[11].normal.Set(0.0f, -1.0f, 0.0f);
		
		indices.resize(12);
		indices = {
			0,1,2,
			3,4,5,
			6,7,8,
			9,10,11
		};

		meshes_.push_back(mesh_part);
	}

} // namespace scythe {