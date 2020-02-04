#include "vertex_format.h"

#include <cstring>
#include <assert.h>

namespace scythe {

	VertexAttribute::VertexAttribute(Type type, U32 size)
	: type(type)
	, size(size)
	{
	}
	bool VertexAttribute::operator ==(const VertexAttribute& other)
	{
		return type == other.type && size == other.size;
	}
	bool VertexAttribute::operator !=(const VertexAttribute& other)
	{
		return !operator==(other);
	}

	VertexFormat::VertexFormat()
	: attributes_(nullptr)
	, num_attributes_(0)
	, vertex_size_(0)
	, max_generic_(0)
	{
		memset((void*)generic_, 0, sizeof(generic_));
	}
	VertexFormat::~VertexFormat()
	{
		if (attributes_)
		{
			delete[] attributes_;
			attributes_ = nullptr;
		}
	}
	bool VertexFormat::operator == (const VertexFormat& vertex_format)
	{
		return IsSame(vertex_format.attributes_, vertex_format.num_attributes_);
	}
	U32 VertexFormat::vertex_size() const
	{
		return vertex_size_;
	}
	const VertexFormat::Attrib& VertexFormat::generic(U32 index) const
	{
		return generic_[index];
	}
	const VertexAttribute * VertexFormat::attributes() const
	{
		return attributes_;
	}
	U32 VertexFormat::num_attributes() const
	{
		return num_attributes_;
	}
	bool VertexFormat::IsSame(VertexAttribute *attribs, U32 num_attribs)
	{
		if (num_attributes_ != num_attribs)
			return false;
		for (U32 i = 0; i < num_attributes_; ++i)
		{
			if (attributes_[i] != attribs[i])
				return false;
		}
		return true;
	}
	void VertexFormat::Fill(VertexAttribute *attribs, U32 num_attribs)
	{
		// Fill attributes array
		assert(attributes_ == nullptr);
		num_attributes_ = num_attribs;
		attributes_ = new VertexAttribute[num_attributes_];
		// Loop through and add all arrays to the format
		for (U32 i = 0; i < num_attribs; ++i)
		{
			attributes_[i] = attribs[i];

			generic_[i].size = attribs[i].size;
			generic_[i].offset = vertex_size_;
			++max_generic_;

			vertex_size_ += attribs[i].size * sizeof(float);
		}
	}

} // namespace scythe