#ifndef __SCYTHE_MESH_H__
#define __SCYTHE_MESH_H__

#include "common/types.h"
#include "common/ref.h"
#include "resource.h"
#include "math/bounding_box.h"
#include "math/bounding_sphere.h"

#include <vector>

namespace scythe {

	class Renderer;
	class VertexFormat;
	class MeshPart;
	struct Material;
	class MaterialBinderInterface;

	/**
	 * Defines a mesh supporting various vertex formats and 1 or more
	 * MeshPart(s) to define how the vertices are connected.
	 */
	class Mesh : public Ref, public Resource {
		friend class Model;
		friend class MeshPartsEnumerator;
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
		void CreateCube(const Vector3 * position = nullptr);
		void CreateBox(const Vector3& extents, const Vector3 * position = nullptr);
		void CreatePhysicalBox(float size_x, float size_y, float size_z, float size_u, float size_v,
			const Vector3 * position = nullptr);

		/**
		 * Translates data to video memory.
		 *
		 * @param[in] vertex_format The vertex format.
		 * @param[in] keep_data True if need to keep data in RAM and false otherwise.
		 */
		bool MakeRenderable(const VertexFormat * vertex_format, bool keep_data = false);

		/**
		 * Cleans all the data that might has been used by physics
		 */
		void CleanUp();
		
		void Render();

		void ScaleVertices(const Vector3& scale);

		//! Specify separate material binder manually
		void SetMaterialBinder(MaterialBinderInterface * material_binder);

		const BoundingBox& GetBoundingBox() const;
		const BoundingSphere& GetBoundingSphere() const;

		unsigned int GetNumberOfParts() const;
		unsigned int GetNumberOfVertices() const;

		/**
		 * Whether mesh primitive type is triangles
		 */
		bool IsTriangleMesh() const;

		void ForceTriangles();
		
	private:
		// Save routines
		bool SaveToFileObj(const char *filename);
		bool SaveToFileScm(const char *filename);

		// Load routines
		bool LoadFromFileObj(const char *filename);
		bool LoadFromFileScm(const char *filename);

		Renderer * renderer_;
		MaterialBinderInterface * material_binder_;
		BoundingBox bounding_box_;
		BoundingSphere bounding_sphere_;
		bool has_bounds_; // if bounds have been set
		bool force_triangles_; // may be needed triangles for physics

		std::vector<MeshPart *> meshes_;
		std::vector<Material> materials_;
	};

} // namespace scythe

#endif