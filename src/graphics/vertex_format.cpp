#include "vertex_format.h"

#include <cstring>
#include <assert.h>

namespace scythe {

	VertexFormat::VertexFormat()
	: vertex_size_(0)
	, max_generic_(0)
	{
		memset((void*)generic_, 0, sizeof(generic_));
	}
	VertexFormat::~VertexFormat()
	{
	}
	bool VertexFormat::operator == (const VertexFormat& vf)
	{
		return memcmp((void*)this, (void*)&vf, sizeof(VertexFormat)) == 0;
	}
	U32 VertexFormat::vertex_size() const
	{
		return vertex_size_;
	}
	const VertexFormat::Attrib& VertexFormat::generic(U32 index) const
	{
		return generic_[index];
	}
	void VertexFormat::Fill(VertexAttribute *attribs, U32 num_attribs)
	{
		// Loop through and add all arrays to the format
		for (U32 i = 0; i < num_attribs; ++i)
		{
			switch (attribs[i].type)
			{
			case VertexAttribute::kGeneric:
			case VertexAttribute::kTangent:
			case VertexAttribute::kBinormal:
			case VertexAttribute::kVertex:
			case VertexAttribute::kTexcoord:
			case VertexAttribute::kNormal:
			case VertexAttribute::kColor:
				generic_[i].size = attribs[i].size;
				generic_[i].offset = vertex_size_;
				++max_generic_;
				break;
			default:
				assert(!"Unkown vertex attribute");
			}

			vertex_size_ += attribs[i].size * sizeof(float);
		}
	}

} // namespace scythe