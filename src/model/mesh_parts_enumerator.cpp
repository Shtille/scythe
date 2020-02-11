#include "mesh_parts_enumerator.h"

#include "mesh.h"
#include "mesh_part.h"

namespace scythe {

	MeshPartsEnumerator::MeshPartsEnumerator(Mesh * mesh)
	: mesh_(mesh)
	, index_(0)
	{

	}
	void MeshPartsEnumerator::Reset()
	{
		index_ = 0;
	}
	bool MeshPartsEnumerator::GetNextObject(PartInfo * info)
	{
		if (index_ < static_cast<unsigned int>(mesh_->meshes_.size()))
		{
			MeshPart * mesh_part = mesh_->meshes_[index_];

			info->num_vertices = static_cast<unsigned int>(mesh_part->vertices_.size());
			if (!mesh_part->vertices_.empty())
				info->vertices = &mesh_part->vertices_[0];
			else
				info->vertices = nullptr;

			info->num_indices = static_cast<unsigned int>(mesh_part->indices_.size());
			if (!mesh_part->indices_.empty())
				info->indices = &mesh_part->indices_[0];
			else
				info->indices = nullptr;

			info->current_index = index_;

			++index_;
			return true;
		}
		else
			return false;
	}

} // namespace scythe