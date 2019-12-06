#ifndef __SCYTHE_MODEL_H__
#define __SCYTHE_MODEL_H__

#include "common/ref.h"
#include "drawable.h"

namespace scythe {

	class Mesh;
	
	/**
	 * Defines a Model or mesh renderer which is an instance of a Mesh. 
	 *
	 * A model has a mesh that can be drawn with the specified materials for
	 * each of the mesh parts within it.
	 */
	class Model : public Ref, public Drawable
	{
		friend class Mesh;
		friend class Node;

	public:

		/**
		 * Creates a new Model.
		 */
		static Model * Create(Mesh * mesh);

		/**
		 * Returns the Mesh for this Model.
		 *
		 * @return The Mesh for this Model.
		 */
		Mesh * GetMesh() const;

		/**
		 * @see Drawable::Draw
		 *
		 * Binds the vertex buffer and index buffers for the Mesh and
		 * all of its MeshPart's and draws the mesh geometry.
		 * Any other state necessary to render the Mesh, such as
		 * rendering states, shader state, and so on, should be set
		 * up before calling this method.
		 */
		unsigned int Draw(bool wireframe = false) final;

	private:

		/**
		 * Constructor.
		 */
		Model();

		/**
		 * Constructor.
		 */
		Model(Mesh * mesh);

		/**
		 * Destructor. Hidden use Release() instead.
		 */
		~Model();

		/**
		 * Hidden copy assignment operator.
		 */
		Model& operator=(const Model&);

		Mesh * mesh_;
	};

} // namespace scythe

#endif