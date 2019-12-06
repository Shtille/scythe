#ifndef __SCYTHE_MESH_H__
#define __SCYTHE_MESH_H__

#include "common/ref.h"
#include "resource.h"
#include "math/bounding_box.h"
#include "math/bounding_sphere.h"
#include "graphics/vertex_format.h"

#include <vector>

namespace scythe {

	class Renderer;
	class MeshPart;
	struct Material;
	class MaterialBinderInterface;

	/**
	 * Defines a mesh supporting various vertex formats and 1 or more
	 * MeshPart(s) to define how the vertices are connected.
	 */
	class Mesh : public Ref, public Resource {
		friend class Model;
		friend class MeshVerticesEnumerator;
	public:

		enum class FileFormat {
			kUnknown,
			kObj, // Wavefront OBJ
			kScm // Shtille Complex Mesh
		};

		//! Material binder can be null
		Mesh(Renderer * renderer, MaterialBinderInterface * material_binder = nullptr);
		~Mesh();

		bool SaveToFile(const char* filename);
		bool LoadFromFile(const char* filename);

		void CreateQuadFullscreen();
		void CreateSphere(float radius = 1.0f, U32 slices = 20, U32 loops = 10);
		void CreateTetrahedron();
		void CreateCube();
		void CreateBox(float size_x, float size_y, float size_z);
		void CreatePhysicalBox(float size_x, float size_y, float size_z, float size_u, float size_v);

		void AddFormat(const VertexAttribute& attrib);
		bool MakeRenderable();
		
		void Render();

		void ScaleVertices(const Vector3& scale);

		//! Specify separate material binder manually
		void SetMaterialBinder(MaterialBinderInterface * material_binder);

		const BoundingBox& bounding_box() const;

		unsigned int GetNumberOfParts() const;
		
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
		std::vector<MeshPart *> meshes_;
		std::vector<Material> materials_;
	};

} // namespace scythe

#endif