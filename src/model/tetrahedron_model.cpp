#include "tetrahedron_model.h"

namespace scythe {

	TetrahedronModel::TetrahedronModel(Renderer * renderer)
	: GeneratedModel(renderer)
	{
		primitive_mode_ = PrimitiveType::kTriangles;
	}
	TetrahedronModel::~TetrahedronModel()
	{
		
	}
	void TetrahedronModel::Create()
	{
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
		
		vertices_.resize(12);
		
		// +X+Z side
		vertices_[0].position.Set(-0.5f*kS, kY, kZ);
		vertices_[0].normal.Set(kCosA*kCosB, kSinA, kCosA*kSinB);
		vertices_[1].position.Set(kS, kY, 0.0f);
		vertices_[1].normal.Set(kCosA*kCosB, kSinA, kCosA*kSinB);
		vertices_[2].position.Set(0.0f,  1.0f,  0.0f);
		vertices_[2].normal.Set(kCosA*kCosB, kSinA, kCosA*kSinB);
		
		// +X-Z side
		vertices_[3].position.Set(kS, kY, 0.0f);
		vertices_[3].normal.Set(kCosA*kCosB, kSinA, -kCosA*kSinB);
		vertices_[4].position.Set(-0.5f*kS, kY, -kZ);
		vertices_[4].normal.Set(kCosA*kCosB, kSinA, -kCosA*kSinB);
		vertices_[5].position.Set( 0.0f,  1.0f,  0.0f);
		vertices_[5].normal.Set(kCosA*kCosB, kSinA, -kCosA*kSinB);
		
		// -X side
		vertices_[6 ].position.Set(-0.5f*kS, kY, -kZ);
		vertices_[6 ].normal.Set(-kCosA, kSinA, 0.0f);
		vertices_[7 ].position.Set(-0.5f*kS, kY, kZ);
		vertices_[7 ].normal.Set(-kCosA, kSinA, 0.0f);
		vertices_[8 ].position.Set( 0.0f,  1.0f,  0.0f);
		vertices_[8 ].normal.Set(-kCosA, kSinA, 0.0f);
		
		// -Y side
		vertices_[9 ].position.Set(-0.5f*kS, kY, kZ);
		vertices_[9 ].normal.Set(0.0f, -1.0f, 0.0f);
		vertices_[10].position.Set(-0.5f*kS, kY, -kZ);
		vertices_[10].normal.Set(0.0f, -1.0f, 0.0f);
		vertices_[11].position.Set(kS, kY, 0.0f);
		vertices_[11].normal.Set(0.0f, -1.0f, 0.0f);
		
		indices_.resize(12);
		indices_ = {
			0,1,2,
			3,4,5,
			6,7,8,
			9,10,11
		};
	}

} // namespace scythe