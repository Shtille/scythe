#include "node.h"

#include "physics/physics_rigid_body.h"
#include "physics/physics_ghost_object.h"
#include "physics/physics_vehicle.h"
#include "physics/physics_vehicle_wheel.h"

// Node dirty flags
#define NODE_DIRTY_WORLD 1
#define NODE_DIRTY_BOUNDS 2
#define NODE_DIRTY_HIERARCHY 4
#define NODE_DIRTY_ALL (NODE_DIRTY_WORLD | NODE_DIRTY_BOUNDS | NODE_DIRTY_HIERARCHY)

namespace scythe {

Node::Node(const char* id)
: _scene(nullptr)
, _firstChild(nullptr)
, _nextSibling(nullptr)
, _prevSibling(nullptr)
, _parent(nullptr)
, _childCount(0)
, _enabled(true)
, _drawable(nullptr)
, _collisionObject(nullptr)
, _userObject(nullptr)
, _dirtyBits(NODE_DIRTY_ALL)
{
	if (id)
	{
		_id = id;
	}
}

Node::~Node()
{
	RemoveAllChildren();
	if (_drawable)
		_drawable->SetNode(nullptr);
	Ref* ref = dynamic_cast<Ref*>(_drawable);
	SC_SAFE_RELEASE(ref);
	SC_SAFE_DELETE(_collisionObject);
	SC_SAFE_RELEASE(_userObject);
}

Node * Node::Create(const char* id)
{
	return new Node(id);
}

const char* Node::GetTypeName() const
{
	return "Node";
}

const char* Node::id() const
{
	return _id.c_str();
}

void Node::SetId(const char* id)
{
	if (id)
	{
		_id = id;
	}
}

Node::Type Node::type() const
{
	return Node::NODE;
}

void Node::AddChild(Node* child)
{
	SC_ASSERT(child);

	if (child->_parent == this)
	{
		// This node is already present in our hierarchy
		return;
	}
	child->AddRef();

	// If the item belongs to another hierarchy, remove it first.
	if (child->_parent)
	{
		child->_parent->RemoveChild(child);
	}
	else if (child->_scene)
	{
		child->_scene->RemoveNode(child);
	}
	// Add child to the end of the list.
	// NOTE: This is different than the original behavior which inserted nodes
	// into the beginning of the list. Although slightly slower to add to the
	// end of the list, it makes scene traversal and drawing order more
	// predictable, so I've changed it.
	if (_firstChild)
	{
		Node* n = _firstChild;
		while (n->_nextSibling)
			n = n->_nextSibling;
		n->_nextSibling = child;
		child->_prevSibling = n;
	}
	else
	{
		_firstChild = child;
	}
	child->_parent = this;
	++_childCount;
	SetBoundsDirty();

	if (_dirtyBits & NODE_DIRTY_HIERARCHY)
	{
		HierarchyChanged();
	}
}

void Node::RemoveChild(Node* child)
{
	if (child == nullptr || child->_parent != this)
	{
		// The child is not in our hierarchy.
		return;
	}
	// Call remove on the child.
	child->Remove();
	SC_SAFE_RELEASE(child);
}

void Node::RemoveAllChildren()
{
	_dirtyBits &= ~NODE_DIRTY_HIERARCHY;
	while (_firstChild)
	{
		RemoveChild(_firstChild);
	}
	_dirtyBits |= NODE_DIRTY_HIERARCHY;
	HierarchyChanged();
}

void Node::Remove()
{
	// Re-link our neighbours.
	if (_prevSibling)
	{
		_prevSibling->_nextSibling = _nextSibling;
	}
	if (_nextSibling)
	{
		_nextSibling->_prevSibling = _prevSibling;
	}
	// Update our parent.
	Node * parent = _parent;
	if (parent)
	{
		if (this == parent->_firstChild)
		{
			parent->_firstChild = _nextSibling;
		}
		--parent->_childCount;
	}
	_nextSibling = nullptr;
	_prevSibling = nullptr;
	_parent = nullptr;

	if (parent && parent->_dirtyBits & NODE_DIRTY_HIERARCHY)
	{
		parent->HierarchyChanged();
	}
}

Node * Node::GetFirstChild() const
{
	return _firstChild;
}

Node * Node::GetNextSibling() const
{
	return _nextSibling;
}

Node * Node::GetPreviousSibling() const
{
	return _prevSibling;
}

Node * Node::GetParent() const
{
	return _parent;
}

unsigned int Node::GetChildCount() const
{
	return _childCount;
}

Node * Node::GetRootNode() const
{
	Node * n = const_cast<Node*>(this);
	while (n->GetParent())
	{
		n = n->GetParent();
	}
	return n;
}

Scene * Node::GetScene() const
{
	if (_scene)
		return _scene;

	// Search our parent for the scene
	if (_parent)
	{
		Scene* scene = _parent->GetScene();
		if (scene)
			return scene;
	}
	return nullptr;
}

void Node::SetEnabled(bool enabled)
{
	if (_enabled != enabled)
	{
		if (_collisionObject)
		{
			_collisionObject->SetEnabled(enabled);
		}
		_enabled = enabled;
	}
}

bool Node::IsEnabled() const
{
	return _enabled;
}

bool Node::IsEnabledInHierarchy() const
{
	if (!_enabled)
	   return false;

   Node* node = _parent;
   while (node)
   {
	   if (!node->_enabled)
	   {
		   return false;
	   }
	   node = node->_parent;
   }
   return true;
}

void Node::Update(float elapsedTime)
{
	for (Node* node = _firstChild; node != nullptr; node = node->_nextSibling)
	{
		if (node->IsEnabled())
		{
			node->Update(elapsedTime);
		}
	}
	/*
	fireScriptEvent<void>(GP_GET_SCRIPT_EVENT(Node, update), dynamic_cast<void*>(this), elapsedTime);
	*/
}

bool Node::IsStatic() const
{
	return (_collisionObject && _collisionObject->IsStatic());
}

const Matrix4& Node::GetWorldMatrix() const
{
	if (_dirtyBits & NODE_DIRTY_WORLD)
	{
		// Clear our dirty flag immediately to prevent this block from being entered if our
		// parent calls our getWorldMatrix() method as a result of the following calculations.
		_dirtyBits &= ~NODE_DIRTY_WORLD;

		if (!IsStatic())
		{
			// If we have a parent, multiply our parent world transform by our local
			// transform to obtain our final resolved world transform.
			Node* parent = GetParent();
			if (parent && (!_collisionObject || _collisionObject->IsKinematic()))
			{
				Matrix4::Multiply(parent->GetWorldMatrix(), GetMatrix(), &_world);
			}
			else
			{
				_world = GetMatrix();
			}

			// Our world matrix was just updated, so call getWorldMatrix() on all child nodes
			// to force their resolved world matrices to be updated.
			for (Node* child = GetFirstChild(); child != nullptr; child = child->GetNextSibling())
			{
				child->GetWorldMatrix();
			}
		}
	}
	return _world;
}

void Node::HierarchyChanged()
{
	// When our hierarchy changes our world transform is affected, so we must dirty it.
	_dirtyBits |= NODE_DIRTY_HIERARCHY;
	TransformChanged();
}

void Node::TransformChanged()
{
	// Our local transform was changed, so mark our world matrices dirty.
	_dirtyBits |= NODE_DIRTY_WORLD | NODE_DIRTY_BOUNDS;

	// Notify our children that their transform has also changed (since transforms are inherited).
	for (Node* n = GetFirstChild(); n != nullptr; n = n->GetNextSibling())
	{
		if (Transform::IsTransformChangedSuspended())
		{
			// If the DIRTY_NOTIFY bit is not set
			if (!n->IsDirty(Transform::DIRTY_NOTIFY))
			{
				n->TransformChanged();
				SuspendTransformChange(n);
			}
		}
		else
		{
			n->TransformChanged();
		}
	}
	Transform::TransformChanged();
}

void Node::SetBoundsDirty()
{
	// Mark ourself and our parent nodes as dirty
	_dirtyBits |= NODE_DIRTY_BOUNDS;

	// Mark our parent bounds as dirty as well
	if (_parent)
		_parent->SetBoundsDirty();
}

Drawable * Node::GetDrawable() const
{
	return _drawable;
}

void Node::SetDrawable(Drawable* drawable)
{
	if (_drawable != drawable)
	{
		if (_drawable)
		{
			_drawable->SetNode(nullptr);
			Ref* ref = dynamic_cast<Ref*>(_drawable);
			if (ref)
				ref->Release();
		}

		_drawable = drawable;

		if (_drawable)
		{
			Ref* ref = dynamic_cast<Ref*>(_drawable);
			if (ref)
				ref->AddRef();
			_drawable->SetNode(this);
		}
	}
	SetBoundsDirty();
}

const BoundingSphere& Node::GetBoundingSphere() const
{
	if (_dirtyBits & NODE_DIRTY_BOUNDS)
	{
		_dirtyBits &= ~NODE_DIRTY_BOUNDS;

		const Matrix4& worldMatrix = GetWorldMatrix();

		// Start with our local bounding sphere
		// TODO: Incorporate bounds from entities other than mesh (i.e. particleemitters, audiosource, etc)
		bool empty = true;
		Model* model = dynamic_cast<Model*>(_drawable);
		if (model && model->GetMesh())
		{
			if (empty)
			{
				_bounds.Set(model->GetMesh()->GetBoundingSphere());
				empty = false;
			}
			else
			{
				_bounds.Merge(model->GetMesh()->GetBoundingSphere());
			}
		}
		if (empty)
		{
			// Empty bounding sphere, set the world translation with zero radius
			worldMatrix.GetTranslation(&_bounds.center);
			_bounds.radius = 0;
		}

		// Transform the sphere (if not empty) into world space.
		if (!empty)
		{
			bool applyWorldTransform = true;
			/*
			if (model && model->getSkin())
			{
				// Special case: If the root joint of our mesh skin is parented by any nodes, 
				// multiply the world matrix of the root joint's parent by this node's
				// world matrix. This computes a final world matrix used for transforming this
				// node's bounding volume. This allows us to store a much smaller bounding
				// volume approximation than would otherwise be possible for skinned meshes,
				// since joint parent nodes that are not in the matrix palette do not need to
				// be considered as directly transforming vertices on the GPU (they can instead
				// be applied directly to the bounding volume transformation below).
				SC_ASSERT(model->getSkin()->getRootJoint());
				Node* jointParent = model->getSkin()->getRootJoint()->getParent();
				if (jointParent)
				{
					// TODO: Should we protect against the case where joints are nested directly
					// in the node hierachy of the model (this is normally not the case)?
					Matrix4 boundsMatrix;
					Matrix4::Multiply(GetWorldMatrix(), jointParent->GetWorldMatrix(), &boundsMatrix);
					_bounds.Transform(boundsMatrix);
					applyWorldTransform = false;
				}
			}
			*/
			if (applyWorldTransform)
			{
				_bounds.Transform(GetWorldMatrix());
			}
		}

		// Merge this world-space bounding sphere with our childrens' bounding volumes.
		for (Node* n = GetFirstChild(); n != nullptr; n = n->GetNextSibling())
		{
			const BoundingSphere& childSphere = n->GetBoundingSphere();
			if (!childSphere.IsEmpty())
			{
				if (empty)
				{
					_bounds.Set(childSphere);
					empty = false;
				}
				else
				{
					_bounds.Merge(childSphere);
				}
			}
		}
	}

	return _bounds;
}

PhysicsCollisionObject* Node::GetCollisionObject() const
{
	return _collisionObject;
}

PhysicsCollisionObject* Node::SetCollisionObject(PhysicsCollisionObject::Type type, const PhysicsCollisionShape::Definition& shape, PhysicsRigidBody::Parameters* rigidBodyParameters, int group, int mask)
{
	SC_SAFE_DELETE(_collisionObject);

	switch (type)
	{
	case PhysicsCollisionObject::kRigidBody:
		{
			_collisionObject = new PhysicsRigidBody(this, shape, rigidBodyParameters ? *rigidBodyParameters : PhysicsRigidBody::Parameters(), group, mask);
		}
		break;

	case PhysicsCollisionObject::kGhostObject:
		{
			_collisionObject = new PhysicsGhostObject(this, shape, group, mask);
		}
		break;

	case PhysicsCollisionObject::kCharacter:
		{
			//_collisionObject = new PhysicsCharacter(this, shape, rigidBodyParameters ? rigidBodyParameters->mass : 1.0f);
		}
		break;

	case PhysicsCollisionObject::kVehicle:
		{
			_collisionObject = new PhysicsVehicle(this, shape, rigidBodyParameters ? *rigidBodyParameters : PhysicsRigidBody::Parameters());
		}
		break;

	case PhysicsCollisionObject::kVehicleWheel:
		{
			//
			// PhysicsVehicleWheel is special because this call will traverse up the scene graph for the
			// first ancestor node that is shared with another node of collision type VEHICLE, and then
			// proceed to add itself as a wheel onto that vehicle. This is by design, and allows the
			// visual scene hierarchy to be the sole representation of the relationship between physics
			// objects rather than forcing that upon the otherwise-flat ".physics" (properties) file.
			//
			// IMPORTANT: The VEHICLE must come before the VEHICLE_WHEEL in the ".scene" (properties) file!
			//
			_collisionObject = new PhysicsVehicleWheel(this, shape, rigidBodyParameters ? *rigidBodyParameters : PhysicsRigidBody::Parameters());
		}
		break;

	case PhysicsCollisionObject::kNone:
		break;  // Already deleted, Just don't add a new collision object back.
	}

	return _collisionObject;
}

Ref* Node::GetUserObject() const
{
	return _userObject;
}

void Node::SetUserObject(Ref* obj)
{
	_userObject = obj;
}

} // namespace scythe