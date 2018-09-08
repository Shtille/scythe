#include "mesh_vertices_enumerator.h"

#include "complex_mesh.h"
#include "mesh.h"

namespace scythe {

	MeshVerticesEnumerator::MeshVerticesEnumerator(ComplexMesh * complex_mesh)
	: complex_mesh_(complex_mesh)
	, index_(0)
	{

	}
	bool MeshVerticesEnumerator::GetNextObject(MeshVerticesInfo * info)
	{
		if (index_ < static_cast<unsigned int>(complex_mesh_->meshes_.size()))
		{
			Mesh * mesh = complex_mesh_->meshes_[index_];
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