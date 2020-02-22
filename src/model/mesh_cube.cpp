#include "mesh.h"

#include "mesh_part.h"

namespace scythe {

	void Mesh::CreateCube(const Vector3 * position)
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

		// Move cube center to desired position
		if (position)
			mesh_part->TranslateVertices(*position);

		if (force_triangles_)
		{
			// Use triangles as primitive for rendering
			mesh_part->primitive_mode_ = PrimitiveType::kTriangles;

			indices.resize(36);
			indices = {
				0,1,2, 2,1,3,
				4,5,6, 6,5,7,
				8,9,10, 10,9,11,
				12,13,14, 14,13,15,
				16,17,18, 18,17,19,
				20,21,22, 22,21,23
			};
		}
		else
		{
			// Use triangle strip as primitive for rendering
			indices.resize(34);
			indices = {
				0,1,2,3, 3,4,
				4,5,6,7, 7,8,
				8,9,10,11, 11,12,
				12,13,14,15, 15,16,
				16,17,18,19, 19,20,
				20,21,22,23
			};
		}

		meshes_.push_back(mesh_part);

		// Set bounds
		Vector3 max = Vector3::One();
		Vector3 min = -max;
		if (!has_bounds_)
		{
			bounding_box_.Set(min, max);
			bounding_sphere_.Set(bounding_box_);
			has_bounds_ = true;
		}
		else
		{
			BoundingBox bounding_box;
			BoundingSphere bounding_sphere;

			bounding_box.Set(min, max);
			bounding_sphere.Set(bounding_box);

			bounding_box_.Merge(bounding_box);
			bounding_sphere_.Merge(bounding_sphere);
		}
	}

} // namespace scythe {