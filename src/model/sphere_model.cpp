#include "sphere_model.h"

#include "math/common_math.h"

#include <cmath>

namespace scythe {

	SphereModel::SphereModel(Renderer * renderer, U32 slices, U32 loops, float radius)
	: GeneratedModel(renderer)
	, slices_(slices)
	, loops_(loops)
	, radius_(radius)
	{
	}
	SphereModel::~SphereModel()
	{
		
	}
	void SphereModel::Create()
	{
		if (HasTexture())
		{
			vertices_.resize((slices_ + 1)*(loops_));
			
			volatile U32 ind = 0;
			
			for (U32 j = 0; j < loops_; ++j)
			{
				float part_j = (float)j / (float)(loops_-1);
				float aj = (kPi / (float)(loops_-1)) * (float)j;
				float sin_aj = sinf(aj);
				float cos_aj = cosf(aj);
				for (U32 i = 0; i <= slices_; ++i)
				{
					float part_i = (float)i / (float)slices_;
					float ai = (kTwoPi / (float)slices_) * (float)i;
					float sin_ai = sinf(ai);
					float cos_ai = cosf(ai);
					
					vertices_[ind].normal.Set(sin_aj*cos_ai, -cos_aj, -sin_aj*sin_ai);
					vertices_[ind].position = vertices_[ind].normal * radius_;
					vertices_[ind].texcoord.Set(part_i, part_j);
					vertices_[ind].tangent.Set(-sin_ai, 0.0f, -cos_ai);
					vertices_[ind].binormal = vertices_[ind].normal ^ vertices_[ind].tangent;
					++ind;
				}
			}
			
			assert(loops_ > 3);
			indices_.resize((2 + 2*slices_)*(loops_ - 1) + 2*(loops_ - 2));
			ind = 0;
			U32 width = slices_ + 1;
			for (U32 j = 0; j < loops_-1; ++j)
			{
				bool lat_end = (j+2 == loops_);
				indices_[ind++] = (j+1)*width;
				indices_[ind++] = (j  )*width;
				for (U32 i = 0; i < slices_; ++i)
				{
					U32 next_i = (i+1);
					
					indices_[ind++] = (next_i) + (j+1)*width;
					indices_[ind++] = (next_i) + (j  )*width;
				}
				// Degenerates
				if (!lat_end)
				{
					indices_[ind] = indices_[ind - 1], ++ind;
					indices_[ind++] = (j+2)*width;
				}
			}
		}
		else // doesn't have a texture
		{
			vertices_.resize(2 + slices_*(loops_ - 2));
			
			volatile U32 ind = 0;
			
			vertices_[ind].position.Set(0.0f, -radius_, 0.0f);
			vertices_[ind].normal.Set(0.0f, -1.0f, 0.0f);
			vertices_[ind].texcoord.Set(0.0f, 0.0f);
			++ind;
			vertices_[ind].position.Set(0.0f, radius_, 0.0f);
			vertices_[ind].normal.Set(0.0f, 1.0f, 0.0f);
			vertices_[ind].texcoord.Set(0.0f, 1.0f);
			++ind;
			
			for (U32 j = 1; j < loops_-1; ++j)
			{
				float part_j = (float)j / (float)(loops_-1);
				float aj = (kPi / (float)(loops_-1)) * (float)j;
				float sin_aj = sinf(aj);
				float cos_aj = cosf(aj);
				for (U32 i = 0; i < slices_; ++i)
				{
					float part_i = (float)i / (float)slices_;
					float ai = (kTwoPi / (float)slices_) * (float)i;
					
					vertices_[ind].normal.Set(sin_aj*cosf(ai), -cos_aj, -sin_aj*sinf(ai));
					vertices_[ind].position = vertices_[ind].normal * radius_;
					vertices_[ind].texcoord.Set(part_i, part_j);
					++ind;
				}
			}
			
			assert(loops_ > 3);
			indices_.resize((2 + 2*slices_)*(loops_ - 1) + 2*(loops_ - 2));
			ind = 0;
			for (U32 j = 0; j < loops_-1; ++j)
			{
				bool lat_end = (j+2 == loops_);
				bool lat_beg = (j == 0);
				indices_[ind++] = lat_end ? (1) : (2 + (j  )*slices_);
				indices_[ind++] = lat_beg ? (0) : (2 + (j-1)*slices_);
				for (U32 i = 0; i < slices_; ++i)
				{
					U32 next_i = (i + 1 == slices_) ? (0) : (i+1);
					
					indices_[ind++] = lat_end ? (1) : (2 + (next_i) + (j  )*slices_);
					indices_[ind++] = lat_beg ? (0) : (2 + (next_i) + (j-1)*slices_);
				}
				// Degenerates
				if (!lat_end)
				{
					if (j+3 == loops_)
					{
						indices_[ind] = indices_[ind - 1], ++ind;
						indices_[ind++] = 1;
					}
					else
					{
						indices_[ind] = indices_[ind - 1], ++ind;
						indices_[ind++] = 2 + (j+1)*slices_;
					}
				}
			}
		}
	}

} // namespace scythe