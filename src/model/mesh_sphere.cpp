#include "mesh.h"

#include "mesh_part.h"

#include "math/constants.h"

namespace scythe {

	void Mesh::CreateSphere(float radius, U32 slices, U32 loops)
	{
		MeshPart * mesh_part = new MeshPart(renderer_);
		auto& vertices = mesh_part->vertices_;
		auto& indices = mesh_part->indices_;

		vertices.resize((slices + 1)*(loops));
		
		U32 ind = 0;
		
		for (U32 j = 0; j < loops; ++j)
		{
			float part_j = (float)j / (float)(loops-1);
			float aj = (kPi / (float)(loops-1)) * (float)j;
			float sin_aj = sinf(aj);
			float cos_aj = cosf(aj);
			for (U32 i = 0; i <= slices; ++i)
			{
				float part_i = (float)i / (float)slices;
				float ai = (kTwoPi / (float)slices) * (float)i;
				float sin_ai = sinf(ai);
				float cos_ai = cosf(ai);
				
				vertices[ind].normal.Set(sin_aj*cos_ai, -cos_aj, -sin_aj*sin_ai);
				vertices[ind].position = vertices[ind].normal * radius;
				vertices[ind].texcoord.Set(part_i, part_j);
				vertices[ind].tangent.Set(-sin_ai, 0.0f, -cos_ai);
				vertices[ind].binormal = vertices[ind].normal ^ vertices[ind].tangent;
				++ind;
			}
		}
		
		assert(loops > 3);
		indices.resize((2 + 2*slices)*(loops - 1) + 2*(loops - 2));
		ind = 0;
		U32 width = slices + 1;
		for (U32 j = 0; j < loops-1; ++j)
		{
			bool lat_end = (j+2 == loops);
			indices[ind++] = (j+1)*width;
			indices[ind++] = (j  )*width;
			for (U32 i = 0; i < slices; ++i)
			{
				U32 next_i = (i+1);
				
				indices[ind++] = (next_i) + (j+1)*width;
				indices[ind++] = (next_i) + (j  )*width;
			}
			// Degenerates
			if (!lat_end)
			{
				indices[ind] = indices[ind - 1], ++ind;
				indices[ind++] = (j+2)*width;
			}
		}

		meshes_.push_back(mesh_part);

		// Set bounds
		Vector3 max(radius);
		Vector3 min = -max;
		bounding_box_.Set(min, max);
		bounding_sphere_.Set(Vector3::Zero(), radius);
		has_bounds_ = true;
	}

} // namespace scythe {