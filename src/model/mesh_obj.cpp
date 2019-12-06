#include "mesh.h"

#include "common/log.h"

#include "mesh_part.h"
#include "material.h"

#include "filesystem/filename.h"
#include "filesystem/directory.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <map>
#include <memory>
#include <cstring>
#include <assert.h>

#ifdef TINYOBJLOADER_USE_DOUBLE
	static_assert(false, "disable double precision on tinyobj");
#endif

namespace scythe {

	bool Mesh::SaveToFileObj(const char *filename)
	{
		assert(!"Haven't been implemented yet");
		return true;
	}
	bool Mesh::LoadFromFileObj(const char *filename)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		Filename fn(filename);
		std::string base_dir = fn.ExtractPath() + GetPathDelimeter();

		std::string err;
		if (! tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename, base_dir.c_str(), true))
		{
			if (!err.empty())
				SC_ERROR("%s", err.c_str());
			SC_ERROR("can't open %s", filename);
			return false;
		}

		if (!err.empty())
			SC_ERROR("%s", err.c_str());

		// Fill materials
		materials_.resize(materials.size());
		for (size_t i = 0; i < materials.size(); ++i)
		{
			Material& material = materials_[i];
			const tinyobj::material_t& obj_material = materials[i];

			memcpy(&material.ambient.x, &obj_material.ambient[0], 3 * sizeof(float));
			memcpy(&material.diffuse.x, &obj_material.diffuse[0], 3 * sizeof(float));
			memcpy(&material.specular.x, &obj_material.specular[0], 3 * sizeof(float));
			memcpy(&material.transmittance.x, &obj_material.transmittance[0], 3 * sizeof(float));
			memcpy(&material.emission.x, &obj_material.emission[0], 3 * sizeof(float));
			material.shininess = obj_material.shininess;
			material.dissolve = obj_material.dissolve;
		}

		Vector3 min = Vector3(1e8), max = Vector3(-1e8);

		// Make unique mesh per material per shape
		typedef std::map<int, std::unique_ptr<MeshPart> > MaterialMap;
		std::vector< MaterialMap > shape_meshes;

		// Loop over shapes
		for (const auto& shape : shapes)
		{
			if (shape.mesh.indices.empty())
				continue;

			// Push an empty map at the beginning
			shape_meshes.push_back(MaterialMap());

			// Loop over faces
			size_t index_offset = 0;
			for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f)
			{
				// Per-face material
				MeshPart * mesh = nullptr;
				int material_id = shape.mesh.material_ids[f];
				if (material_id == -1)
				{
					SC_ERROR("some face(s) don't have material(s) in %s", filename);
					return false;
				}
				else
				{
					MaterialMap& materials_map = shape_meshes.back();
					auto it = materials_map.find(material_id);
					if (it == materials_map.end())
					{
						mesh = new MeshPart(renderer_);
						mesh->primitive_mode_ = PrimitiveType::kTriangles;
						mesh->material_ = &materials_[material_id];
						materials_map[material_id].reset(mesh);
					}
					else
						mesh = it->second.get();
				}

				// Loop over vertices in the face
				size_t fv = static_cast<size_t>(shape.mesh.num_face_vertices[f]);
				for (size_t v = 0; v < fv; ++v)
				{
					// Access to vertex
					tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
					Vertex vertex;
					if (idx.vertex_index != -1)
						memcpy(&vertex.position, &attrib.vertices [3*idx.vertex_index  ], 3 * sizeof(float));
					if (idx.normal_index != -1)
						memcpy(&vertex.normal,   &attrib.normals  [3*idx.normal_index  ], 3 * sizeof(float));
					if (idx.texcoord_index != -1)
						memcpy(&vertex.texcoord, &attrib.texcoords[2*idx.texcoord_index], 2 * sizeof(float));

					min.MakeFloor(vertex.position);
					max.MakeCeil(vertex.position);

					mesh->vertices_.push_back(vertex);
					// Indices isn't necessary
				}
				index_offset += fv;
			}
		}

		bounding_box_.Set(min, max);

		// Finally fill the meshes
		for (auto& map : shape_meshes)
		{
			for (auto& pair : map)
			{
				MeshPart * mesh = pair.second.release();
				meshes_.push_back(mesh);
			}
		}

		return true;
	}

} // namespace scythe