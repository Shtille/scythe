#include "mesh_vertices_enumerator.h"

#include "mesh.h"
#include "mesh_part.h"

namespace scythe {

	MeshVerticesEnumerator::MeshVerticesEnumerator(Mesh * mesh)
	: mesh_(mesh)
	, index_(0)
	{

	}
	bool MeshVerticesEnumerator::GetNextObject(MeshVerticesInfo * info)
	{
		if (index_ < static_cast<unsigned int>(mesh_->meshes_.size()))
		{
			MeshPart * mesh = mesh_->meshes_[index_];
			if (!mesh->vertices_.empty())
			{
				info->vertices = &mesh->vertices_[0];
				info->num_vertices = static_cast<unsigned int>(mesh->vertices_.size());
			}
			else
			{
				info->vertices = nullptr;
				info->num_vertices = 0;
			}
			++index_;
			return true;
		}
		else
			return false;
	}

} // namespace scythe