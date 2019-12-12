#include "node.h"

#include "physics/physics_rigid_body.h"
#include "physics/physics_ghost_object.h"
#include "physics/physics_vehicle.h"
#include "physics/physics_vehicle_wheel.h"

#include "model/model.h"
#include "model/mesh.h"

#include "common/sc_assert.h"
#include "common/sc_delete.h"

namespace scythe {

Node::Node(const char* id)
: scene_(nullptr)
, first_child_(nullptr)
, next_sibling_(nullptr)
, prev_sibling_(nullptr)
, parent_(nullptr)
, child_count_(0)
, enabled_(true)
, drawable_(nullptr)
, collision_object_(nullptr)
, user_object_(nullptr)
, dirty_bits_(kNodeDirtyAll)
{
	if (id)
	{
		id_ = id;
	}
}

Node::~Node()
{
	RemoveAllChildren();
	if (drawable_)
		drawable_->SetNode(nullptr);
	Ref* ref = dynamic_cast<Ref*>(drawable_);
	SC_SAFE_RELEASE(ref);
	SC_SAFE_DELETE(collision_object_);
	SC_SAFE_RELEASE(user_object_);
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
	return id_.c_str();
}

void Node::SetId(const char* id)
{
	if (id)
	{
		id_ = id;
	}
}

Node::Type Node::type() const
{
	return Node::kNode;
}

void Node::AddChild(Node* child)
{
	SC_ASSERT(child);

	if (child->parent_ == this)
	{
		// This node is already present in our hierarchy
		return;
	}
	child->AddRef();

	// If the item belongs to another hierarchy, remove it first.
	if (child->parent_)
	{
		child->parent_->RemoveChild(child);
	}
	else if (child->scene_)
	{
		child->scene_->RemoveNode(child);
	}
	// Add child to the end of the list.
	// NOTE: This is different than the original behavior which inserted nodes
	// into the beginning of the list. Although slightly slower to add to the
	// end of the list, it makes scene traversal and drawing order more
	// predictable, so I've changed it.
	if (first_child_)
	{
		Node* n = first_child_;
		while (n->next_sibling_)
			n = n->next_sibling_;
		n->next_sibling_ = child;
		child->prev_sibling_ = n;
	}
	else
	{
		first_child_ = child;
	}
	child->parent_ = this;
	++child_count_;
	SetBoundsDirty();

	if (dirty_bits_ & kNodeDirtyHierarchy)
	{
		HierarchyChanged();
	}
}

void Node::RemoveChild(Node* child)
{
	if (child == nullptr || child->parent_ != this)
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
	dirty_bits_ &= ~kNodeDirtyHierarchy;
	while (first_child_)
	{
		RemoveChild(first_child_);
	}
	dirty_bits_ |= kNodeDirtyHierarchy;
	HierarchyChanged();
}

void Node::Remove()
{
	// Re-link our neighbours.
	if (prev_sibling_)
	{
		prev_sibling_->next_sibling_ = next_sibling_;
	}
	if (next_sibling_)
	{
		next_sibling_->prev_sibling_ = prev_sibling_;
	}
	// Update our parent.
	Node * parent = parent_;
	if (parent)
	{
		if (this == parent->first_child_)
		{
			parent->first_child_ = next_sibling_;
		}
		--parent->child_count_;
	}
	next_sibling_ = nullptr;
	prev_sibling_ = nullptr;
	parent_ = nullptr;

	if (parent && parent->dirty_bits_ & kNodeDirtyHierarchy)
	{
		parent->HierarchyChanged();
	}
}

Node * Node::GetFirstChild() const
{
	return first_child_;
}

Node * Node::GetNextSibling() const
{
	return next_sibling_;
}

Node * Node::GetPreviousSibling() const
{
	return prev_sibling_;
}

Node * Node::GetParent() const
{
	return parent_;
}

unsigned int Node::GetChildCount() const
{
	return child_count_;
}

Node * Node::GetRootNode() const
{
	Node * node = const_cast<Node*>(this);
	while (node->GetParent())
	{
		node = node->GetParent();
	}
	return node;
}

Scene * Node::GetScene() const
{
	if (scene_)
		return scene_;

	// Search our parent for the scene
	if (parent_)
	{
		Scene* scene = parent_->GetScene();
		if (scene)
			return scene;
	}
	return nullptr;
}

void Node::SetEnabled(bool enabled)
{
	if (enabled_ != enabled)
	{
		if (collision_object_)
		{
			collision_object_->SetEnabled(enabled);
		}
		enabled_ = enabled;
	}
}

bool Node::IsEnabled() const
{
	return enabled_;
}

bool Node::IsEnabledInHierarchy() const
{
	if (!enabled_)
	   return false;

   Node* node = parent_;
   while (node)
   {
	   if (!node->enabled_)
	   {
		   return false;
	   }
	   node = node->parent_;
   }
   return true;
}

void Node::Update(float elapsedTime)
{
	for (Node* node = first_child_; node != nullptr; node = node->next_sibling_)
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
	return (collision_object_ && collision_object_->IsStatic());
}

const Matrix4& Node::GetWorldMatrix() const
{
	if (dirty_bits_ & kNodeDirtyWorld)
	{
		// Clear our dirty flag immediately to prevent this block from being entered if our
		// parent calls our getWorldMatrix() method as a result of the following calculations.
		dirty_bits_ &= ~kNodeDirtyWorld;

		if (!IsStatic())
		{
			// If we have a parent, multiply our parent world transform by our local
			// transform to obtain our final resolved world transform.
			Node* parent = GetParent();
			if (parent && (!collision_object_ || collision_object_->IsKinematic()))
			{
				Matrix4::Multiply(parent->GetWorldMatrix(), GetMatrix(), &world_);
			}
			else
			{
				world_ = GetMatrix();
			}

			// Our world matrix was just updated, so call getWorldMatrix() on all child nodes
			// to force their resolved world matrices to be updated.
			for (Node* child = GetFirstChild(); child != nullptr; child = child->GetNextSibling())
			{
				child->GetWorldMatrix();
			}
		}
	}
	return world_;
}

void Node::HierarchyChanged()
{
	// When our hierarchy changes our world transform is affected, so we must dirty it.
	dirty_bits_ |= kNodeDirtyHierarchy;
	TransformChanged();
}

void Node::TransformChanged()
{
	// Our local transform was changed, so mark our world matrices dirty.
	dirty_bits_ |= kNodeDirtyWorld | kNodeDirtyBounds;

	// Notify our children that their transform has also changed (since transforms are inherited).
	for (Node* node = GetFirstChild(); node != nullptr; node = node->GetNextSibling())
	{
		if (Transform::IsTransformChangedSuspended())
		{
			// If the DIRTY_NOTIFY bit is not set
			if (!node->IsDirty(Transform::DIRTY_NOTIFY))
			{
				node->TransformChanged();
				SuspendTransformChange(n);
			}
		}
		else
		{
			node->TransformChanged();
		}
	}
	Transform::TransformChanged();
}

void Node::SetBoundsDirty()
{
	// Mark ourself and our parent nodes as dirty
	dirty_bits_ |= kNodeDirtyBounds;

	// Mark our parent bounds as dirty as well
	if (parent_)
		parent_->SetBoundsDirty();
}

Drawable * Node::GetDrawable() const
{
	return drawable_;
}

void Node::SetDrawable(Drawable* drawable)
{
	if (drawable_ != drawable)
	{
		if (drawable_)
		{
			drawable_->SetNode(nullptr);
			Ref* ref = dynamic_cast<Ref*>(drawable_);
			if (ref)
				ref->Release();
		}

		drawable_ = drawable;

		if (drawable_)
		{
			Ref* ref = dynamic_cast<Ref*>(drawable_);
			if (ref)
				ref->AddRef();
			drawable_->SetNode(this);
		}
	}
	SetBoundsDirty();
}

const BoundingSphere& Node::GetBoundingSphere() const
{
	if (dirty_bits_ & kNodeDirtyBounds)
	{
		dirty_bits_ &= ~kNodeDirtyBounds;

		const Matrix4& worldMatrix = GetWorldMatrix();

		// Start with our local bounding sphere
		// TODO: Incorporate bounds from entities other than mesh (i.e. particleemitters, audiosource, etc)
		bool empty = true;
		Model* model = dynamic_cast<Model*>(drawable_);
		if (model && model->GetMesh())
		{
			if (empty)
			{
				bounding_sphere_.Set(model->GetMesh()->GetBoundingSphere());
				empty = false;
			}
			else
			{
				bounding_sphere_.Merge(model->GetMesh()->GetBoundingSphere());
			}
		}
		if (empty)
		{
			// Empty bounding sphere, set the world translation with zero radius
			worldMatrix.GetTranslation(&bounding_sphere_.center);
			bounding_sphere_.radius = 0;
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
					bounding_sphere_.Transform(boundsMatrix);
					applyWorldTransform = false;
				}
			}
			*/
			if (applyWorldTransform)
			{
				bounding_sphere_.Transform(GetWorldMatrix());
			}
		}

		// Merge this world-space bounding sphere with our childrens' bounding volumes.
		for (Node * node = GetFirstChild(); node != nullptr; node = node->GetNextSibling())
		{
			const BoundingSphere& childSphere = node->GetBoundingSphere();
			if (!childSphere.IsEmpty())
			{
				if (empty)
				{
					bounding_sphere_.Set(childSphere);
					empty = false;
				}
				else
				{
					bounding_sphere_.Merge(childSphere);
				}
			}
		}
	}

	return bounding_sphere_;
}

PhysicsCollisionObject* Node::GetCollisionObject() const
{
	return collision_object_;
}

PhysicsCollisionObject* Node::SetCollisionObject(PhysicsCollisionObject::Type type, const PhysicsCollisionShape::Definition& shape, PhysicsRigidBody::Parameters* rigidBodyParameters, int group, int mask)
{
	SC_SAFE_DELETE(collision_object_);

	switch (type)
	{
	case PhysicsCollisionObject::kRigidBody:
		{
			collision_object_ = new PhysicsRigidBody(this, shape, rigidBodyParameters ? *rigidBodyParameters : PhysicsRigidBody::Parameters(), group, mask);
		}
		break;

	case PhysicsCollisionObject::kGhostObject:
		{
			collision_object_ = new PhysicsGhostObject(this, shape, group, mask);
		}
		break;

	case PhysicsCollisionObject::kCharacter:
		{
			//collision_object_ = new PhysicsCharacter(this, shape, rigidBodyParameters ? rigidBodyParameters->mass : 1.0f);
		}
		break;

	case PhysicsCollisionObject::kVehicle:
		{
			collision_object_ = new PhysicsVehicle(this, shape, rigidBodyParameters ? *rigidBodyParameters : PhysicsRigidBody::Parameters());
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
			collision_object_ = new PhysicsVehicleWheel(this, shape, rigidBodyParameters ? *rigidBodyParameters : PhysicsRigidBody::Parameters());
		}
		break;

	case PhysicsCollisionObject::kNone:
		break;  // Already deleted, Just don't add a new collision object back.
	}

	return collision_object_;
}

Ref* Node::GetUserObject() const
{
	return user_object_;
}

void Node::SetUserObject(Ref* obj)
{
	user_object_ = obj;
}

} // namespace scythe