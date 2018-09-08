#include "complex_mesh.h"

#include "mesh.h"
#include "material.h"
#include "graphics/renderer.h"
#include "filesystem/filename.h"

#include <assert.h>

namespace scythe {

	static ComplexMesh::FileFormat ExtractFileFormat(const char* filename)
	{
		Filename fn(filename);
		std::string ext = fn.ExtractExt();
		if (ext == "obj")
			return ComplexMesh::FileFormat::kObj;
		else if (ext == "scm")
			return ComplexMesh::FileFormat::kScm;
		else
			return ComplexMesh::FileFormat::kUnknown;
	}

	ComplexMesh::ComplexMesh(Renderer * renderer, MaterialBinderInterface * material_binder)
	: renderer_(renderer)
	, material_binder_(material_binder)
	, vertex_format_(nullptr)
	{

	}
	ComplexMesh::~ComplexMesh()
	{
		for (auto mesh : meshes_)
		{
			delete mesh;
		}
		if (vertex_format_)
			renderer_->DeleteVertexFormat(vertex_format_);
	}
	bool ComplexMesh::SaveToFile(const char* filename)
	{
		FileFormat fmt = ExtractFileFormat(filename);
		switch (fmt)
		{
		case FileFormat::kObj:
			return SaveToFileObj(filename);
		case FileFormat::kScm:
			return SaveToFileScm(filename);
		default:
			assert(!"unknown model format");
			return false;
		}
	}
	bool ComplexMesh::LoadFromFile(const char* filename)
	{
		FileFormat fmt = ExtractFileFormat(filename);
		switch (fmt)
		{
		case FileFormat::kObj:
			return LoadFromFileObj(filename);
		case FileFormat::kScm:
			return LoadFromFileScm(filename);
		default:
			assert(!"unknown model format");
			return false;
		}
	}
	void ComplexMesh::Create()
	{
		
	}
	void ComplexMesh::AddFormat(const VertexAttribute& attrib)
	{
		attribs_.push_back(attrib);
	}
	bool ComplexMesh::MakeRenderable()
	{
		if (attribs_.empty())
		{
			assert(!"Vertex format hasn't been set.");
			return false;
		}
		renderer_->AddVertexFormat(vertex_format_, &attribs_[0], (U32)attribs_.size());

		for (auto mesh : meshes_)
		{
			if (!mesh->MakeRenderable(vertex_format_, attribs_))
				return false;
		}

		return true;
	}
	void ComplexMesh::Render()
	{
		for (auto mesh : meshes_)
		{
			if (material_binder_)
				material_binder_->Bind(mesh->material_);
			mesh->Render();
		}
	}
	void ComplexMesh::ScaleVertices(const Vector3& scale)
	{
		for (auto mesh : meshes_)
		{
			mesh->ScaleVertices(scale);
		}
		bounding_box_.extent *= scale;
	}
	void ComplexMesh::SetMaterialBinder(MaterialBinderInterface * material_binder)
	{
		material_binder_ = material_binder;
	}
	const BoundingBox& ComplexMesh::bounding_box() const
	{
		return bounding_box_;
	}

} // namespace scythe