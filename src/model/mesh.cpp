#include "mesh.h"

#include "mesh_part.h"
#include "material.h"
#include "graphics/renderer.h"
#include "filesystem/filename.h"

#include "common/sc_assert.h"

namespace scythe {

	static Mesh::FileFormat ExtractFileFormat(const char* filename)
	{
		Filename fn(filename);
		std::string ext = fn.ExtractExt();
		if (ext == "obj")
			return Mesh::FileFormat::kObj;
		else if (ext == "scm")
			return Mesh::FileFormat::kScm;
		else
			return Mesh::FileFormat::kUnknown;
	}

	Mesh::Mesh(Renderer * renderer, MaterialBinderInterface * material_binder)
	: renderer_(renderer)
	, material_binder_(material_binder)
	, has_bounds_(false)
	, force_triangles_(false)
	{

	}
	Mesh::~Mesh()
	{
		for (auto mesh : meshes_)
		{
			delete mesh;
		}
	}
	bool Mesh::SaveToFile(const char* filename)
	{
		FileFormat fmt = ExtractFileFormat(filename);
		switch (fmt)
		{
		case FileFormat::kObj:
			return SaveToFileObj(filename);
		case FileFormat::kScm:
			return SaveToFileScm(filename);
		default:
			SC_ASSERT(!"unknown model format");
			return false;
		}
	}
	bool Mesh::LoadFromFile(const char* filename)
	{
		FileFormat fmt = ExtractFileFormat(filename);
		switch (fmt)
		{
		case FileFormat::kObj:
			return LoadFromFileObj(filename);
		case FileFormat::kScm:
			return LoadFromFileScm(filename);
		default:
			SC_ASSERT(!"unknown model format");
			return false;
		}
	}
	bool Mesh::MakeRenderable(const VertexFormat * vertex_format, bool keep_data)
	{
		BoundingBox * bounding_box_ptr = nullptr;
		if (!has_bounds_)
		{
			// Set initial values for bounding box
			bounding_box_.Set(Vector3(1e8), Vector3(-1e8));
			bounding_box_ptr = &bounding_box_;
		}

		for (auto mesh : meshes_)
		{
			if (!mesh->MakeRenderable(vertex_format, bounding_box_ptr, keep_data))
				return false;
		}

		if (!has_bounds_)
		{
			// Also calculate bounding sphere from bounding box
			bounding_sphere_.Set(bounding_box_);
		}

		return true;
	}
	void Mesh::CleanUp()
	{
		for (auto mesh_part : meshes_)
		{
			mesh_part->CleanUp();
		}
	}
	void Mesh::Render()
	{
		for (auto mesh : meshes_)
		{
			if (material_binder_)
				material_binder_->Bind(mesh->material_);
			mesh->Render();
		}
	}
	void Mesh::ScaleVertices(const Vector3& scale)
	{
		for (auto mesh : meshes_)
		{
			mesh->ScaleVertices(scale);
		}
		bounding_box_.Scale(scale);
	}
	void Mesh::SetMaterialBinder(MaterialBinderInterface * material_binder)
	{
		material_binder_ = material_binder;
	}
	const BoundingBox& Mesh::GetBoundingBox() const
	{
		return bounding_box_;
	}
	const BoundingSphere& Mesh::GetBoundingSphere() const
	{
		return bounding_sphere_;
	}
	unsigned int Mesh::GetNumberOfParts() const
	{
		return static_cast<unsigned int>(meshes_.size());
	}
	unsigned int Mesh::GetNumberOfVertices() const
	{
		unsigned int num_vertices = 0;
		for (auto mesh_part : meshes_)
		{
			num_vertices += mesh_part->num_vertices_;
		}
		return num_vertices;
	}
	bool Mesh::IsTriangleMesh() const
	{
		for (auto mesh_part : meshes_)
		{
			if (mesh_part->primitive_mode_ != PrimitiveType::kTriangles)
				return false;
		}
		return true;
	}
	void Mesh::ForceTriangles()
	{
		force_triangles_ = true;
	}

} // namespace scythe