#ifndef __SCYTHE_MESH_VERTICES_ENUMERATOR_H__
#define __SCYTHE_MESH_VERTICES_ENUMERATOR_H__

#include "vertex.h"

namespace scythe {

	class Mesh;

	struct MeshVerticesInfo {
		const Vertex * vertices;
		unsigned int num_vertices;
	};

	class MeshVerticesEnumerator {
	public:
		MeshVerticesEnumerator(Mesh * mesh);

		bool GetNextObject(MeshVerticesInfo * info);

	private:
		Mesh * mesh_;
		unsigned int index_;
	};

} // namespace scythe

#endif