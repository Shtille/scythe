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
	, vertex_format_(nullptr)
	, has_bounds_(false)
	{

	}
	Mesh::~Mesh()
	{
		for (auto mesh : meshes_)
		{
			delete mesh;
		}
		if (vertex_format_)
			renderer_->DeleteVertexFormat(vertex_format_);
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
	void Mesh::AddFormat(const VertexAttribute& attrib)
	{
		attribs_.push_back(attrib);
	}
	bool Mesh::MakeRenderable()
	{
		if (attribs_.empty())
		{
			SC_ASSERT(!"Vertex format hasn't been set.");
			return false;
		}
		renderer_->AddVertexFormat(vertex_format_, &attribs_[0], (U32)attribs_.size());

		BoundingBox * bounding_box_ptr = nullptr;
		if (!has_bounds_)
		{
			// Set initial values for bounding box
			bounding_box_.Set(Vector3(1e8), Vector3(-1e8));
			bounding_box_ptr = &bounding_box_;
		}

		for (auto mesh : meshes_)
		{
			if (!mesh->MakeRenderable(vertex_format_, attribs_, bounding_box_ptr))
				return false;
		}

		if (!has_bounds_)
		{
			// Also calculate bounding sphere from bounding box
			bounding_sphere_.Set(bounding_box_);
		}

		return true;
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

} // namespace scythe