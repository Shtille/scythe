#include "drawable.h"

namespace scythe {

	Drawable::Drawable()
	: node_(nullptr)
	{
	}
	Drawable::~Drawable()
	{
	}
	Node* Drawable::GetNode() const
	{
		return node_;
	}
	void Drawable::SetNode(Node* node)
	{
		node_ = node;
	}

} // namespace scythe