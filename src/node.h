#ifndef __SCYTHE_NODE_H__
#define __SCYTHE_NODE_H__

#include "common/ref.h"
#include "math/transform.h"
#include "physics/physics_rigid_body.h"

#include <string>

namespace scythe {

	class Scene;
	class Drawable;

	/**
	 * Defines a hierarchical structure of objects in 3D transformation spaces.
	 *
	 * This object allow you to attach components to a scene such as:
	 * Drawable's(Model, Camera, Light, PhysicsCollisionObject, AudioSource, etc.
	 */
	class Node : public Transform, public Ref
	{
		friend class Scene;

	public:

		/**
		 * Defines the types of nodes.
		 */
		enum Type
		{
			NODE = 1,
			JOINT
		};

		/**
		 * Creates a new node with the specified ID.
		 *
		 * @param id The ID for the new node.
		 * @script{create}
		 */
		static Node * Create(const char* id = nullptr);

		/**
		 * Extends ScriptTarget::getTypeName() to return the type name of this class.
		 *
		 * @return The type name of this class: "Node"
		 * @see ScriptTarget::getTypeName()
		 */
		const char* GetTypeName() const;

		/**
		 * Gets the identifier for the node.
		 *
		 * @return The node identifier.
		 */
		const char* id() const;

		/**
		 * Sets the identifier for the node.
		 *
		 * @param id The identifier to set for the node.
		 */
		void SetId(const char* id);

		/**
		 * Returns the type of the node.
		 */
		virtual Node::Type type() const;

		/**
		 * Adds a child node.
		 *
		 * @param child The child to add.
		 */
		virtual void AddChild(Node* child);

		/**
		 * Removes a child node.
		 *
		 * @param child The child to remove.
		 */
		virtual void RemoveChild(Node* child);

		/**
		 * Removes all child nodes.
		 */
		virtual void RemoveAllChildren();

		/**
		 * Returns the first child for this node.
		 *
		 * @return The first child.
		 */
		Node * GetFirstChild() const;

		/**
		 * Returns the first sibling of this node.
		 *
		 * @return The first sibling.
		 */
		Node * GetNextSibling() const;

		/**
		 * Returns the previous sibling to this node.
		 *
		 * @return The previous sibling.
		 */
		Node* GetPreviousSibling() const;

		/**
		 * Returns the parent of this node.
		 *
		 * @return The parent.
		 */
		Node * GetParent() const;

		/**
		 * Returns the number of direct children of this item.
		 *
		 * @return The number of children.
		 */
		unsigned int GetChildCount() const;

		/**
		 * Gets the top level node in this node's parent hierarchy.
		 */
		Node * GetRootNode() const;

		/**
		 * Gets the scene this node is currenlty within.
		 *
		 * @return The scene.
		 */
		virtual Scene * GetScene() const;

		/**
		 * Sets if the node is enabled in the scene.
		 *
		 * @param enabled if the node is enabled in the scene.
		 */
		void SetEnabled(bool enabled);

		/**
		 * Gets if the node is enabled in the scene.
		 *
		 * @return if the node is enabled in the scene.
		 */
		bool IsEnabled() const;

		/**
		 * Gets if the node inherently enabled.
		 *
		 * @return if components attached on this node should be running.
		 */
		bool IsEnabledInHierarchy() const;

		/**
		 * Called to update the state of this Node.
		 *
		 * This method is called by Scene::update(float) to update the state of all active
		 * nodes in a scene. A Node is considered active if Node::isActive() returns true.
		 *
		 * If any scripts are attached to the node, their update event will be fired.
		 *
		 * @param elapsedTime Elapsed time in milliseconds.
		 */
		void Update(float elapsedTime);

		/**
		 * Returns whether the transformation of this node is static.
		 *
		 * Nodes that have static rigid bodies attached to them are considered static.
		 *
		 * @return True if the transformation of this Node is static, false otherwise.
		 *
		 * @see Transform::isStatic()
		 */
		bool IsStatic() const;

		/**
		 * Gets the world matrix corresponding to this node.
		 *
		 * @return The world matrix of this node.
		 */
		virtual const Matrix& GetWorldMatrix() const;

		/**
		 * Gets the drawable object attached to this node.
		 *
		 * @return The drawable component attached to this node.
		 */
		Drawable * GetDrawable() const;

		/**
		 * Set the drawable object to be attached to this node
		 *
		 * This is typically a Model, ParticleEmiiter, Form, Terrrain, Sprite, TileSet or Text.
		 *
		 * This will increase the reference count of the new drawble and decrease
		 * the reference count of the old drawable.
		 *
		 * @param drawable The new drawable component. May be NULL.
		 */
		void SetDrawable(Drawable * drawable);

		/**
		 * Returns the pointer to this node's physics collision object.
		 *
		 * The type of the returned collision object can be queried using
		 * the PhysicsCollisionObject::getType() method.
		 *
		 * @return The pointer to this node's physics collision object.
		 */
		PhysicsCollisionObject * GetCollisionObject() const;

		/**
		 * Sets (or disables) the physics collision object for this node.
		 *
		 * The supported collision object types include rigid bodies, ghost objects,
		 * characters, vehicles, and vehicle wheels.
		 *
		 * Rigid bodies are used to represent most physical objects in a game. The important
		 * feature of rigid bodies is that they can be simulated by the physics system as other
		 * rigid bodies or collision objects collide with them. To support this physics simulation,
		 * rigid bodies require additional parameters, such as mass, friction and restitution to
		 * define their physical features. These parameters can be passed into the
		 * 'rigidBodyParameters' parameter.
		 *
		 * Vehicles consist of a rigid body with wheels. The rigid body parameters can be passed-in
		 * via the 'rigidBodyParameters' parameter, and wheels can be added to the vehicle.
		 *
		 * Ghost objects are a simple type of collision object that are not simulated. By default
		 * they pass through other objects in the scene without affecting them. Ghost objects do
		 * receive collision events however, which makes them useful for representing non-simulated
		 * entities in a game that still require collision events, such as volumetric triggers,
		 * power-ups, etc.
		 *
		 * Characters are an extension of ghost objects which provide a number of additional features
		 * for animating and moving characters within a game. Characters are represented as ghost
		 * objects instead of rigid bodies to allow more direct control over character movement,
		 * since attempting to model a physics character with a simulated rigid body usually results
		 * in unresponsive and unpredictable character movement. Unlike normal ghost objects,
		 * characters to react to other characters and rigid bodies in the world. Characters react
		 * to gravity and collide (and respond) with rigid bodies to allow them to walk on the ground,
		 * slide along walls and walk up/down slopes and stairs.
		 *
		 * @param type The type of the collision object to set; to disable the physics
		 *        collision object, pass PhysicsCollisionObject::NONE.
		 * @param shape Definition of a physics collision shape to be used for this collision object.
		 *        Use the static shape methods on the PhysicsCollisionShape class to specify a shape
		 *        definition, such as PhysicsCollisionShape::box().
		 * @param rigidBodyParameters If type is PhysicsCollisionObject::RIGID_BODY or
		 *        PhysicsCollisionObject::VEHICLE, this must point to a valid rigid body
		 *        parameters object containing information about the rigid body;
		 *        otherwise, this parameter may be NULL.
		 * @param group Group identifier of the object for collision filtering.
		 * @param mask Bitmask to filter groups of objects to collide with this one.
		 */
		PhysicsCollisionObject* SetCollisionObject(PhysicsCollisionObject::Type type,
												   const PhysicsCollisionShape::Definition& shape = PhysicsCollisionShape::Box(),
												   PhysicsRigidBody::Parameters* rigidBodyParameters = NULL,
												   int group = PHYSICS_COLLISION_GROUP_DEFAULT,
												   int mask = PHYSICS_COLLISION_MASK_DEFAULT);

		/**
		 * Gets the user object assigned to this node.
		 *
		 * @return The user object assigned object to this node.
		 */
		Ref* GetUserObject() const;

		/**
		* Sets a user object to be assigned object to this node.
		*
		* @param obj The user object assigned object to this node.
		*/
		void SetUserObject(Ref* obj);

		/**
		 * Returns the bounding sphere for the Node, in world space.
		 *
		 * The bounding sphere for a node represents the area, in world
		 * space, that the node contains. This includes the space occupied
		 * by any child nodes as well as the space occupied by any data
		 * inside the node (such as models).
		 *
		 * Bounding spheres for nodes are rough approximations of the data
		 * contained within a node and they are intended for visibility
		 * testing or first-pass intersection testing only. They are not
		 * appropriate for accurate collision detection since they most often
		 * do not tightly contain a node's content.
		 *
		 * A node that does not occupy any space will return a bounding sphere
		 * with a center point equal to the node translation and a radius of zero.
		 *
		 * @return The world-space bounding sphere for the node.
		 */
		const BoundingSphere& GetBoundingSphere() const;

	protected:

		/**
		 * Constructor.
		 */
		Node(const char* id);

		/**
		 * Destructor.
		 */
		virtual ~Node();

		/**
		 * Removes this node from its parent.
		 */
		void Remove();

		/**
		 * Called when this Node's transform changes.
		 */
		void TransformChanged();

		/**
		 * Called when this Node's hierarchy changes.
		 */
		void HierarchyChanged();

		/**
		 * Marks the bounding volume of the node as dirty.
		 */
		void SetBoundsDirty();

	private:

		/**
		 * Hidden copy constructor.
		 */
		Node(const Node& copy);

		/**
		 * Hidden copy assignment operator.
		 */
		Node& operator=(const Node&);

	protected:

		/** The scene this node is attached to. */
		Scene * _scene;
		/** The nodes id. */
		std::string _id;
		/** The nodes first child. */
		Node * _firstChild;
		/** The nodes next sibiling. */
		Node * _nextSibling;
		/** The nodes previous sibiling. */
		Node * _prevSibling;
		/** The nodes parent. */
		Node * _parent;
		/** The number of child nodes. */
		unsigned int _childCount;
		/** If this node is enabled. Maybe different if parent is enabled/disabled. */
		bool _enabled; 
		/** The drawble component attached to this node. */
		Drawable * _drawable;
		/** The collision object component attached to this node. */
		PhysicsCollisionObject * _collisionObject;
		/** The user object component attached to this node. */
		Ref * _userObject;
		/** The world matrix for this node. */
		mutable Matrix4 _world;
		/** The bounding sphere for this node. */
		mutable BoundingSphere _bounds;
		/** The dirty bits used for optimization. */
		mutable int _dirtyBits;
	};

} // namespace scythe

#endif