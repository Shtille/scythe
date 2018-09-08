#include "physical_box_model.h"

namespace scythe {

	PhysicalBoxModel::PhysicalBoxModel(Renderer * renderer, float size_x, float size_y, float size_z, float size_u, float size_v)
	: GeneratedModel(renderer)
	, sizes_(size_x, size_y, size_z)
	, texture_sizes_(size_u, size_v)
	{
	}
	PhysicalBoxModel::~PhysicalBoxModel()
	{
		
	}
	void PhysicalBoxModel::Create()
	{
		vertices_.resize(24);
		
		{
			// +X side
			float kU = 2.0f * sizes_.z / texture_sizes_.x;
			float kV = 2.0f * sizes_.y / texture_sizes_.y;

			vertices_[0].position.Set(1.0f,  1.0f,  1.0f);
			vertices_[0].normal.Set(1.0f,  0.0f,  0.0f);
			vertices_[0].texcoord.Set(0.0f,  kV);
			vertices_[0].tangent.Set(0.0f,  0.0f, -1.0f);
			vertices_[0].binormal.Set(0.0f,  1.0f, 0.0f);

			vertices_[1].position.Set(1.0f, -1.0f,  1.0f);
			vertices_[1].normal.Set(1.0f,  0.0f,  0.0f);
			vertices_[1].texcoord.Set(0.0f,  0.0f);
			vertices_[1].tangent.Set(0.0f,  0.0f, -1.0f);
			vertices_[1].binormal.Set(0.0f,  1.0f, 0.0f);

			vertices_[2].position.Set(1.0f,  1.0f, -1.0f);
			vertices_[2].normal.Set(1.0f,  0.0f,  0.0f);
			vertices_[2].texcoord.Set(kU,  kV);
			vertices_[2].tangent.Set(0.0f,  0.0f, -1.0f);
			vertices_[2].binormal.Set(0.0f,  1.0f, 0.0f);

			vertices_[3].position.Set(1.0f, -1.0f, -1.0f);
			vertices_[3].normal.Set(1.0f,  0.0f,  0.0f);
			vertices_[3].texcoord.Set(kU,  0.0f);
			vertices_[3].tangent.Set(0.0f,  0.0f, -1.0f);
			vertices_[3].binormal.Set(0.0f,  1.0f, 0.0f);
		}
		
		{
			// -Z side
			float kU = 2.0f * sizes_.x / texture_sizes_.x;
			float kV = 2.0f * sizes_.y / texture_sizes_.y;

			vertices_[4].position.Set( 1.0f,  1.0f, -1.0f);
			vertices_[4].normal.Set(0.0f,  0.0f, -1.0f);
			vertices_[4].texcoord.Set(0.0f,  kV);
			vertices_[4].tangent.Set(-1.0f, 0.0f, 0.0f);
			vertices_[4].binormal.Set(0.0f, 1.0f, 0.0f);

			vertices_[5].position.Set( 1.0f, -1.0f, -1.0f);
			vertices_[5].normal.Set(0.0f,  0.0f, -1.0f);
			vertices_[5].texcoord.Set(0.0f,  0.0f);
			vertices_[5].tangent.Set(-1.0f, 0.0f, 0.0f);
			vertices_[5].binormal.Set(0.0f, 1.0f, 0.0f);

			vertices_[6].position.Set(-1.0f,  1.0f, -1.0f);
			vertices_[6].normal.Set(0.0f,  0.0f, -1.0f);
			vertices_[6].texcoord.Set(kU,  kV);
			vertices_[6].tangent.Set(-1.0f, 0.0f, 0.0f);
			vertices_[6].binormal.Set(0.0f, 1.0f, 0.0f);

			vertices_[7].position.Set(-1.0f, -1.0f, -1.0f);
			vertices_[7].normal.Set(0.0f,  0.0f, -1.0f);
			vertices_[7].texcoord.Set(kU,  0.0f);
			vertices_[7].tangent.Set(-1.0f, 0.0f, 0.0f);
			vertices_[7].binormal.Set(0.0f, 1.0f, 0.0f);
		}
		
		{
			// -X side
			float kU = 2.0f * sizes_.z / texture_sizes_.x;
			float kV = 2.0f * sizes_.y / texture_sizes_.y;

			vertices_[8 ].position.Set(-1.0f,  1.0f, -1.0f);
			vertices_[8 ].normal.Set(-1.0f,  0.0f,  0.0f);
			vertices_[8 ].texcoord.Set(0.0f,  kV);
			vertices_[8 ].tangent.Set(0.0f, 0.0f, 1.0f);
			vertices_[8 ].binormal.Set(0.0f, 1.0f, 0.0f);

			vertices_[9 ].position.Set(-1.0f, -1.0f, -1.0f);
			vertices_[9 ].normal.Set(-1.0f,  0.0f,  0.0f);
			vertices_[9 ].texcoord.Set(0.0f,  0.0f);
			vertices_[9 ].tangent.Set(0.0f, 0.0f, 1.0f);
			vertices_[9 ].binormal.Set(0.0f, 1.0f, 0.0f);

			vertices_[10].position.Set(-1.0f,  1.0f,  1.0f);
			vertices_[10].normal.Set(-1.0f,  0.0f,  0.0f);
			vertices_[10].texcoord.Set(kU,  kV);
			vertices_[10].tangent.Set(0.0f, 0.0f, 1.0f);
			vertices_[10].binormal.Set(0.0f, 1.0f, 0.0f);

			vertices_[11].position.Set(-1.0f, -1.0f,  1.0f);
			vertices_[11].normal.Set(-1.0f,  0.0f,  0.0f);
			vertices_[11].texcoord.Set(kU,  0.0f);
			vertices_[11].tangent.Set(0.0f, 0.0f, 1.0f);
			vertices_[11].binormal.Set(0.0f, 1.0f, 0.0f);
		}
		
		{
			// +Z side
			float kU = 2.0f * sizes_.x / texture_sizes_.x;
			float kV = 2.0f * sizes_.y / texture_sizes_.y;

			vertices_[12].position.Set(-1.0f,  1.0f,  1.0f);
			vertices_[12].normal.Set(0.0f,  0.0f,  1.0f);
			vertices_[12].texcoord.Set(0.0f,  kV);
			vertices_[12].tangent.Set(1.0f, 0.0f, 0.0f);
			vertices_[12].binormal.Set(0.0f, 1.0f, 0.0f);

			vertices_[13].position.Set(-1.0f, -1.0f,  1.0f);
			vertices_[13].normal.Set(0.0f,  0.0f,  1.0f);
			vertices_[13].texcoord.Set(0.0f,  0.0f);
			vertices_[13].tangent.Set(1.0f, 0.0f, 0.0f);
			vertices_[13].binormal.Set(0.0f, 1.0f, 0.0f);

			vertices_[14].position.Set( 1.0f,  1.0f,  1.0f);
			vertices_[14].normal.Set(0.0f,  0.0f,  1.0f);
			vertices_[14].texcoord.Set(kU,  kV);
			vertices_[14].tangent.Set(1.0f, 0.0f, 0.0f);
			vertices_[14].binormal.Set(0.0f, 1.0f, 0.0f);

			vertices_[15].position.Set( 1.0f, -1.0f,  1.0f);
			vertices_[15].normal.Set(0.0f,  0.0f,  1.0f);
			vertices_[15].texcoord.Set(kU,  0.0f);
			vertices_[15].tangent.Set(1.0f, 0.0f, 0.0f);
			vertices_[15].binormal.Set(0.0f, 1.0f, 0.0f);
		}
		
		{
			// +Y side
			float kU = 2.0f * sizes_.z / texture_sizes_.x;
			float kV = 2.0f * sizes_.x / texture_sizes_.y;

			vertices_[16].position.Set( 1.0f,  1.0f,  1.0f);
			vertices_[16].normal.Set(0.0f,  1.0f,  0.0f);
			vertices_[16].texcoord.Set(kU,  kV);
			vertices_[16].tangent.Set(-1.0f, 0.0f, 0.0f);
			vertices_[16].binormal.Set(0.0f, 0.0f, 1.0f);

			vertices_[17].position.Set( 1.0f,  1.0f, -1.0f);
			vertices_[17].normal.Set(0.0f,  1.0f,  0.0f);
			vertices_[17].texcoord.Set(0.0f,  kV);
			vertices_[17].tangent.Set(-1.0f, 0.0f, 0.0f);
			vertices_[17].binormal.Set(0.0f, 0.0f, 1.0f);

			vertices_[18].position.Set(-1.0f,  1.0f,  1.0f);
			vertices_[18].normal.Set(0.0f,  1.0f,  0.0f);
			vertices_[18].texcoord.Set(kU,  0.0f);
			vertices_[18].tangent.Set(-1.0f, 0.0f, 0.0f);
			vertices_[18].binormal.Set(0.0f, 0.0f, 1.0f);

			vertices_[19].position.Set(-1.0f,  1.0f, -1.0f);
			vertices_[19].normal.Set(0.0f,  1.0f,  0.0f);
			vertices_[19].texcoord.Set(0.0f,  0.0f);
			vertices_[19].tangent.Set(-1.0f, 0.0f, 0.0f);
			vertices_[19].binormal.Set(0.0f, 0.0f, 1.0f);
		}
		
		{
			// -Y side
			float kU = 2.0f * sizes_.z / texture_sizes_.x;
			float kV = 2.0f * sizes_.x / texture_sizes_.y;

			vertices_[20].position.Set( 1.0f, -1.0f, -1.0f);
			vertices_[20].normal.Set(0.0f, -1.0f,  0.0f);
			vertices_[20].texcoord.Set(kU,  kV);
			vertices_[20].tangent.Set(-1.0f, 0.0f, 0.0f);
			vertices_[20].binormal.Set(0.0f, 0.0f, -1.0f);

			vertices_[21].position.Set( 1.0f, -1.0f,  1.0f);
			vertices_[21].normal.Set(0.0f, -1.0f,  0.0f);
			vertices_[21].texcoord.Set(0.0f,  kV);
			vertices_[21].tangent.Set(-1.0f, 0.0f, 0.0f);
			vertices_[21].binormal.Set(0.0f, 0.0f, -1.0f);

			vertices_[22].position.Set(-1.0f, -1.0f, -1.0f);
			vertices_[22].normal.Set(0.0f, -1.0f,  0.0f);
			vertices_[22].texcoord.Set(kU,  0.0f);
			vertices_[22].tangent.Set(-1.0f, 0.0f, 0.0f);
			vertices_[22].binormal.Set(0.0f, 0.0f, -1.0f);

			vertices_[23].position.Set(-1.0f, -1.0f,  1.0f);
			vertices_[23].normal.Set(0.0f, -1.0f,  0.0f);
			vertices_[23].texcoord.Set(0.0f,  0.0f);
			vertices_[23].tangent.Set(-1.0f, 0.0f, 0.0f);
			vertices_[23].binormal.Set(0.0f, 0.0f, -1.0f);
		}

		// Scale box
		ScaleVertices(sizes_);
		
		indices_.resize(34);
		indices_ = {
			0,1,2,3, 3,4,
			4,5,6,7, 7,8,
			8,9,10,11, 11,12,
			12,13,14,15, 15,16,
			16,17,18,19, 19,20,
			20,21,22,23
		};
	}
	const Vector3& PhysicalBoxModel::sizes() const
	{
		return sizes_;
	}
	const Vector2& PhysicalBoxModel::texture_sizes() const
	{
		return texture_sizes_;
	}

} // namespace scythe