#include "model.h"
#include "mesh.h"

#include "common/sc_assert.h"
#include "common/sc_delete.h"

namespace scythe {

	Model::Model()
	: Drawable()
	, mesh_(nullptr)
	{

	}
	Model::Model(Mesh * mesh)
	: Drawable()
	, mesh_(mesh)
	{
		SC_ASSERT(mesh);
	}
	Model::~Model()
	{
		SC_SAFE_RELEASE(mesh_);
	}
	Model * Model::Create(Mesh * mesh)
	{
		SC_ASSERT(mesh);
		mesh->AddRef();
		return new Model(mesh);
	}
	Mesh * Model::GetMesh() const
	{
		return mesh_;
	}
	unsigned int Model::Draw(bool wireframe)
	{
		// TODO: add wireframe usage
		mesh_->Render();
		// Return number of draw calls
		return mesh_->GetNumberOfParts();
	}

} // namespace scythe {