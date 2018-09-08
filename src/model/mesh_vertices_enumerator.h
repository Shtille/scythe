#ifndef __SCYTHE_MESH_VERTICES_ENUMERATOR_H__
#define __SCYTHE_MESH_VERTICES_ENUMERATOR_H__

#include "vertex.h"

namespace scythe {

	class ComplexMesh;

	struct MeshVerticesInfo {
		const Vertex * vertices;
		unsigned int num_vertices;
	};

	class MeshVerticesEnumerator {
	public:
		MeshVerticesEnumerator(ComplexMesh * complex_mesh);

		bool GetNextObject(MeshVerticesInfo * info);

	private:
		ComplexMesh * complex_mesh_;
		unsigned int index_;
	};

} // namespace scythe

#endif