#ifndef __SCYTHE_COMPLEX_MESH_H__
#define __SCYTHE_COMPLEX_MESH_H__

#include "resource.h"
#include "graphics/vertex_format.h"
#include "math/bounding_box.h"

#include <vector>

namespace scythe {

	class Renderer;
	class Mesh;
	struct Material;
	class MaterialBinderInterface;
	
	//! Complex mesh class
	class ComplexMesh : public Resource {
		friend class MeshVerticesEnumerator;
	public:

		enum class FileFormat {
			kUnknown,
			kObj, // Wavefront OBJ
			kScm // Shtille Complex Mesh
		};

		//! Material binder can be null
		ComplexMesh(Renderer * renderer, MaterialBinderInterface * material_binder);
		virtual ~ComplexMesh();

		bool SaveToFile(const char* filename);
		bool LoadFromFile(const char* filename);
		
		virtual void Create();

		void AddFormat(const VertexAttribute& attrib);
		bool MakeRenderable();
		
		void Render();

		void ScaleVertices(const Vector3& scale);

		//! Specify separate material binder manually
		void SetMaterialBinder(MaterialBinderInterface * material_binder);

		const BoundingBox& bounding_box() const;
		
	private:
		// Save routines
		bool SaveToFileObj(const char *filename);
		bool SaveToFileScm(const char *filename);

		// Load routines
		bool LoadFromFileObj(const char *filename);
		bool LoadFromFileScm(const char *filename);

		Renderer * renderer_;
		MaterialBinderInterface * material_binder_;
		VertexFormat * vertex_format_;
		BoundingBox bounding_box_;

		std::vector<VertexAttribute> attribs_;
		std::vector<Mesh*> meshes_;
		std::vector<Material> materials_;
	};

} // namespace scythe

#endif