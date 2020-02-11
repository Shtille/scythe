#ifndef __SCYTHE_PHYSICS_COLLISION_SHAPE_H__
#define __SCYTHE_PHYSICS_COLLISION_SHAPE_H__

#include "common/ref.h"
#include "math/vector3.h"

#include <vector>

// Forward Bullet declarations
class btCollisionShape;
class btStridingMeshInterface;

namespace scythe {

	class Mesh;

	/**
	 * Defines the physics collision shape class that all supported shapes derive from.
	 */
	class PhysicsCollisionShape : public Ref {

		friend class PhysicsController;
		friend class PhysicsRigidBody;

	public:
		/**
		 * Defines the supported collision shape types.
		 */
		enum Type {
			kNone,
			kBox,
			kSphere,
			kCapsule,
			kMesh,
			kHeightfield
		};

		/**
		 * Structure representing the definition of a collision shape, which is used
		 * during collision shape construction time.
		 *
		 * Use the static methods on the PhysicsCollisionShape class to return
		 * 
		 */
		struct Definition
		{
			friend class PhysicsCollisionShape;
			friend class PhysicsController;
			friend class PhysicsRigidBody;
			friend class PhysicsCharacter;
			friend class PhysicsGhostObject;

		public:

			/**
			 * Constructor.
			 */
			Definition();

			/** 
			 * Constructs a new Definition that is a copy of the specified Definition.
			 *
			 * @param definition The Definition to copy.
			 */ 
			Definition(const Definition& definition);

			/**
			 * Assigns the specified Definition as the Definition.
			 *
			 * @param definition The Definition to assign to the Definition.
			 */
			Definition& operator=(const Definition& definition);
			
			/**
			 * Destructor.
			 */
			~Definition();

			/**
			 * Determines if this is an empty/undefined collision shape definition.
			 */
			bool IsEmpty() const;

		private:

			// Shape type.
			PhysicsCollisionShape::Type type;

			// Shape data.
			struct BoxData { float center[3], extents[3]; };
			struct SphereData { float center[3]; float radius; };
			struct CapsuleData { float center[3]; float radius, height; };

			union UnitedData
			{
				/** @script{ignore} */
				BoxData box;
				/** @script{ignore} */
				SphereData sphere;
				/** @script{ignore} */
				CapsuleData capsule;
				/** @script{ignore} */
				Mesh* mesh;
			};
			UnitedData data;

			// Whether the shape definition is explicit, or if it is inherited from node bounds.
			bool is_explicit;

			// Whether the center position is absolute or relative to the node position.
			bool center_absolute;
		};

		/**
		 * Returns the type of this collision shape.
		 *
		 * @return The collision shape type.
		 */
		PhysicsCollisionShape::Type type() const;

		/**
		 * Returns the internal bullet physics shape object.
		 *
		 * @return The bullet shape object.
		 * @script{ignore}
		 */
		btCollisionShape * shape() const;

		/**
		 * Defines a box shape, using the bounding volume of the node it is attached to.
		 *
		 * @return Definition of a box shape.
		 */
		static PhysicsCollisionShape::Definition DefineBox();

		/**
		 * Defines a box shape, using the specified shape information and center.
		 *
		 * @param extents Extents of the box shape along the x, y and z axes.
		 * @param center Center point of the box.
		 * @param absolute True to specify that the given center point is an absolute position.
		 *        By default the center point is treated as relative to the location of the node
		 *        that the shape is attached to.
		 *
		 * @return Definition of a box shape.
		 */
		static PhysicsCollisionShape::Definition DefineBox(const Vector3& extents, const Vector3& center = Vector3::Zero(), bool absolute = false);

		/**
		 * Defines a sphere shape, using the bounding volume of the node it is attached to.
		 *
		 * @return Definition of a sphere shape.
		 */
		static PhysicsCollisionShape::Definition DefineSphere();

		/**
		 * Defines a sphere shape, using the specified shape information and center.
		 *
		 * @param radius Radius of the sphere.
		 * @param center Center point of the sphere.
		 * @param absolute True to specify that the given center point is an absolute position.
		 *        By default the center point is treated as relative to the location of the node
		 *        that the shape is attached to.
		 *
		 * @return Definition of a sphere shape.
		 */
		static PhysicsCollisionShape::Definition DefineSphere(float radius, const Vector3& center = Vector3::Zero(), bool absolute = false);

		/**
		 * Defines a capsule shape, using the bounding volume of the node it is attached to.
		 *
		 * @return Definition of a capsule shape.
		 */
		static PhysicsCollisionShape::Definition DefineCapsule();

		/**
		 * Defines a capsule shape, using the specified shape information and center.
		 *
		 * @param radius Radius of the capsule.
		 * @param height Height of the capsule.
		 * @param center Center point of the capsule.
		 * @param absolute True to specify that the given center point is an absolute position.
		 *        By default the center point is treated as relative to the location of the node
		 *        that the shape is attached to.
		 *
		 * @return Definition of a capsule shape.
		 */
		static PhysicsCollisionShape::Definition DefineCapsule(float radius, float height, const Vector3& center = Vector3::Zero(), bool absolute = false);

		/**
		 * Defines a mesh shape using the specified mesh.
		 *
		 * @return Definition of a mesh shape.
		 */
		static PhysicsCollisionShape::Definition DefineMesh(Mesh* mesh);

	private:

		struct MeshData
		{
			float * vertex_data;
			std::vector<unsigned int*> index_data;
		};

		// struct HeightfieldData
		// {
		//     HeightField* heightfield;
		//     bool inverseIsDirty;
		//     Matrix inverse;
		//     float minHeight;
		//     float maxHeight;
		// };

		/**
		 * Constructor.
		 */
		PhysicsCollisionShape(Type type, btCollisionShape* shape, btStridingMeshInterface* mesh_interface = nullptr);

		/**
		 * Destructor.
		 */
		~PhysicsCollisionShape();

		/** 
		 * Hidden copy constructor.
		 */
		PhysicsCollisionShape(const PhysicsCollisionShape& copy);

		//! Shape type
		Type type_;

		//! Bullet shape object
		btCollisionShape * shape_;

		//! Bullet mesh interface for mesh types (NULL otherwise)
		btStridingMeshInterface * mesh_interface_;

		// Shape specific cached data
		union ShapeData
		{
			MeshData * mesh_data;
			//HeightfieldData * heightfield_data;
		};
		ShapeData shape_data_;
	};

} // namespace scythe

#endif