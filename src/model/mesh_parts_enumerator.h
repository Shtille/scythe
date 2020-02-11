#ifndef __SCYTHE_MESH_PARTS_ENUMERATOR_H__
#define __SCYTHE_MESH_PARTS_ENUMERATOR_H__

#include "vertex.h"

namespace scythe {

	class Mesh;

	/**
	 * Defines mesh vertices enumerator
	 *
	 * We assume that position is always the first in stride.
	 */
	class MeshPartsEnumerator {
	public:

		/**
		 * Defines mesh part information structure
		 */
		struct PartInfo {
			const Vertex * vertices;
			const unsigned int * indices;
			unsigned int num_vertices;
			unsigned int num_indices;
			unsigned int current_index;
		};

		/**
		 * Constructor
		 */
		MeshPartsEnumerator(Mesh * mesh);

		/**
		 * Resets objects enumeration counter
		 */
		void Reset();

		/**
		 * Obtains information for the next object.
		 *
		 * @param[in,out] info Pointer to information structure instance.
		 * @return True if next object has been obtained and false otherwise.
		 */
		bool GetNextObject(PartInfo * info);

	private:
		Mesh * mesh_;
		unsigned int index_;
	};

} // namespace scythe

#endif