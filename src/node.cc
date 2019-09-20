#include "node.h"

// Node dirty flags
#define NODE_DIRTY_WORLD 1
#define NODE_DIRTY_BOUNDS 2
#define NODE_DIRTY_HIERARCHY 4
#define NODE_DIRTY_ALL (NODE_DIRTY_WORLD | NODE_DIRTY_BOUNDS | NODE_DIRTY_HIERARCHY)

namespace scythe {

Node::Node(const char* id)
	: _scene(NULL), _firstChild(NULL), _nextSibling(NULL), _prevSibling(NULL), _parent(NULL), _childCount(0), _enabled(true), _tags(NULL),
	_drawable(NULL), _camera(NULL), _light(NULL), _audioSource(NULL), _collisionObject(NULL), _agent(NULL), _userObject(NULL),
	  _dirtyBits(NODE_DIRTY_ALL)
{
	GP_REGISTER_SCRIPT_EVENTS();
	if (id)
	{
		_id = id;
	}
}

Node::~Node()
{
	removeAllChildren();
	if (_drawable)
		_drawable->setNode(NULL);
	if (_audioSource)
		_audioSource->setNode(NULL);
	Ref* ref = dynamic_cast<Ref*>(_drawable);
	SAFE_RELEASE(ref);
	SAFE_RELEASE(_camera);
	SAFE_RELEASE(_light);
	SAFE_RELEASE(_audioSource);
	SAFE_DELETE(_collisionObject);
	SAFE_RELEASE(_userObject);
	SAFE_DELETE(_tags);
	setAgent(NULL);
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
	GP_ASSERT(child);

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
	if (child == NULL || child->_parent != this)
	{
		// The child is not in our hierarchy.
		return;
	}
	// Call remove on the child.
	child->Remove();
	SAFE_RELEASE(child);
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
	_nextSibling = NULL;
	_prevSibling = NULL;
	_parent = NULL;

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

Node * Node::FindNode(const char* id, bool recursive, bool exactMatch) const
{
	return FindNode(id, recursive, exactMatch, false);
}

Node * Node::FindNode(const char* id, bool recursive, bool exactMatch, bool skipSkin) const
{
	GP_ASSERT(id);

	// If not skipSkin hierarchy, try searching the skin hierarchy
	if (!skipSkin)
	{
		// If the drawable is a model with a mesh skin, search the skin's hierarchy as well.
		Node* rootNode = NULL;
		Model* model = dynamic_cast<Model*>(_drawable);
		if (model)
		{
			if (model->getSkin() != NULL && (rootNode = model->getSkin()->_rootNode) != NULL)
			{
				if ((exactMatch && rootNode->_id == id) || (!exactMatch && rootNode->_id.find(id) == 0))
					return rootNode;

				Node* match = rootNode->FindNode(id, true, exactMatch, true);
				if (match)
				{
					return match;
				}
			}
		}
	}
	// Search immediate children first.
	for (Node* child = GetFirstChild(); child != NULL; child = child->GetNextSibling())
	{
		// Does this child's ID match?
		if ((exactMatch && child->_id == id) || (!exactMatch && child->_id.find(id) == 0))
		{
			return child;
		}
	}
	// Recurse.
	if (recursive)
	{
		for (Node* child = GetFirstChild(); child != NULL; child = child->GetNextSibling())
		{
			Node* match = child->FindNode(id, true, exactMatch, skipSkin);
			if (match)
			{
				return match;
			}
		}
	}
	return NULL;
}

unsigned int Node::FindNodes(const char* id, std::vector<Node*>& nodes, bool recursive, bool exactMatch) const
{
	return findNodes(id, nodes, recursive, exactMatch, false);
}

unsigned int Node::FindNodes(const char* id, std::vector<Node*>& nodes, bool recursive, bool exactMatch, bool skipSkin) const
{
	GP_ASSERT(id);

	// If the drawable is a model with a mesh skin, search the skin's hierarchy as well.
	unsigned int count = 0;

	if (!skipSkin)
	{
		Node* rootNode = NULL;
		Model* model = dynamic_cast<Model*>(_drawable);
		if (model)
		{
			if (model->getSkin() != NULL && (rootNode = model->getSkin()->_rootNode) != NULL)
			{
				if ((exactMatch && rootNode->_id == id) || (!exactMatch && rootNode->_id.find(id) == 0))
				{
					nodes.push_back(rootNode);
					++count;
				}
				count += rootNode->FindNodes(id, nodes, recursive, exactMatch, true);
			}
		}
	}

	// Search immediate children first.
	for (Node* child = GetFirstChild(); child != NULL; child = child->GetNextSibling())
	{
		// Does this child's ID match?
		if ((exactMatch && child->_id == id) || (!exactMatch && child->_id.find(id) == 0))
		{
			nodes.push_back(child);
			++count;
		}
	}
	// Recurse.
	if (recursive)
	{
		for (Node* child = GetFirstChild(); child != NULL; child = child->GetNextSibling())
		{
			count += child->FindNodes(id, nodes, recursive, exactMatch, skipSkin);
		}
	}

	return count;
}

Scene* Node::GetScene() const
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
	return NULL;
}

bool Node::HasTag(const char* name) const
{
	GP_ASSERT(name);
	return (_tags ? _tags->find(name) != _tags->end() : false);
}

const char* Node::GetTag(const char* name) const
{
	GP_ASSERT(name);

	if (!_tags)
		return NULL;

	std::map<std::string, std::string>::const_iterator itr = _tags->find(name);
	return (itr == _tags->end() ? NULL : itr->second.c_str());
}

void Node::SetTag(const char* name, const char* value)
{
	GP_ASSERT(name);

	if (value == NULL)
	{
		// Removing tag
		if (_tags)
		{
			_tags->erase(name);
			if (_tags->size() == 0)
			{
				SAFE_DELETE(_tags);
			}
		}
	}
	else
	{
		// Setting tag
		if (_tags == NULL)
		{
			_tags = new std::map<std::string, std::string>();
		}
		(*_tags)[name] = value;
	}
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
	for (Node* node = _firstChild; node != NULL; node = node->_nextSibling)
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

const Matrix& Node::GetWorldMatrix() const
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
				Matrix::Multiply(parent->GetWorldMatrix(), GetMatrix(), &_world);
			}
			else
			{
				_world = GetMatrix();
			}

			// Our world matrix was just updated, so call getWorldMatrix() on all child nodes
			// to force their resolved world matrices to be updated.
			for (Node* child = GetFirstChild(); child != NULL; child = child->GetNextSibling())
			{
				child->GetWorldMatrix();
			}
		}
	}
	return _world;
}

const Matrix& Node::GetWorldViewMatrix() const
{
	static Matrix worldView;
	Matrix::Multiply(GetViewMatrix(), GetWorldMatrix(), &worldView);
	return worldView;
}

const Matrix& Node::GetInverseTransposeWorldViewMatrix() const
{
	static Matrix invTransWorldView;
	Matrix::Multiply(GetViewMatrix(), getWorldMatrix(), &invTransWorldView);
	invTransWorldView.Invert();
	invTransWorldView.Transpose();
	return invTransWorldView;
}

const Matrix& Node::GetInverseTransposeWorldMatrix() const
{
	static Matrix invTransWorld;
	invTransWorld = getWorldMatrix();
	invTransWorld.invert();
	invTransWorld.transpose();
	return invTransWorld;
}

const Matrix& Node::GetViewMatrix() const
{
	Scene * scene = GetScene();
	Camera * camera = scene ? scene->GetActiveCamera() : NULL;
	if (camera)
	{
		return camera->GetViewMatrix();
	}
	else
	{
		return Matrix::Identity();
	}
}

const Matrix& Node::GetInverseViewMatrix() const
{
	Scene * scene = GetScene();
	Camera * camera = scene ? scene->GetActiveCamera() : NULL;
	if (camera)
	{
		return camera->GetInverseViewMatrix();
	}
	else
	{
		return Matrix::Identity();
	}
}

const Matrix& Node::getProjectionMatrix() const
{
	Scene * scene = GetScene();
	Camera * camera = scene ? scene->GetActiveCamera() : NULL;
	if (camera)
	{
		return camera->GetProjectionMatrix();
	}
	else
	{
		return Matrix::Identity();
	}
}

const Matrix& Node::GetViewProjectionMatrix() const
{
	Scene * scene = GetScene();
	Camera * camera = scene ? scene->GetActiveCamera() : NULL;
	if (camera)
	{
		return camera->GetViewProjectionMatrix();
	}
	else
	{
		return Matrix::Identity();
	}
}

const Matrix& Node::GetInverseViewProjectionMatrix() const
{
	Scene * scene = GetScene();
	Camera * camera = scene ? scene->GetActiveCamera() : NULL;
	if (camera)
	{
		return camera->GetInverseViewProjectionMatrix();
	}
	return Matrix::Identity();
}

const Matrix& Node::GetWorldViewProjectionMatrix() const
{
	// Always re-calculate worldViewProjection matrix since it's extremely difficult
	// to track whether the camera has changed (it may frequently change every frame).
	static Matrix worldViewProj;
	Matrix::Multiply(GetViewProjectionMatrix(), GetWorldMatrix(), &worldViewProj);
	return worldViewProj;
}

Vector3 Node::GetTranslationWorld() const
{
	Vector3 translation;
	GetWorldMatrix().GetTranslation(&translation);
	return translation;
}

Vector3 Node::GetTranslationView() const
{
	Vector3 translation;
	GetWorldMatrix().GetTranslation(&translation);
	GetViewMatrix().TransformPoint(&translation);
	return translation;
}

Vector3 Node::GetForwardVectorWorld() const
{
	Vector3 vector;
	GetWorldMatrix().GetForwardVector(&vector);
	return vector;
}

Vector3 Node::GetForwardVectorView() const
{
	Vector3 vector;
	GetWorldMatrix().GetForwardVector(&vector);
	GetViewMatrix().TransformVector(&vector);
	return vector;
}

Vector3 Node::GetRightVectorWorld() const
{
	Vector3 vector;
	GetWorldMatrix().GetRightVector(&vector);
	return vector;
}

Vector3 Node::GetUpVectorWorld() const
{
	Vector3 vector;
	GetWorldMatrix().GetUpVector(&vector);
	return vector;
}

Vector3 Node::GetActiveCameraTranslationWorld() const
{
	Scene * scene = GetScene();
	if (scene)
	{
		Camera * camera = scene->GetActiveCamera();
		if (camera)
		{
			Node * cameraNode = camera->GetNode();
			if (cameraNode)
			{
				return cameraNode->GetTranslationWorld();
			}
		}
	}
	return Vector3::Zero();
}

Vector3 Node::GetActiveCameraTranslationView() const
{
	Scene * scene = GetScene();
	if (scene)
	{
		Camera * camera = scene->GetActiveCamera();
		if (camera)
		{
			Node * cameraNode = camera->GetNode();
			if (cameraNode)
			{
				return cameraNode->GetTranslationView();
			}
		}
	}
	return Vector3::Zero();
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
	for (Node* n = GetFirstChild(); n != NULL; n = n->GetNextSibling())
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

Animation * Node::GetAnimation(const char* id) const
{
	/*
	Animation* animation = ((AnimationTarget*)this)->GetAnimation(id);
	if (animation)
		return animation;
	
	// See if this node has a model, then drill down.
	Model* model = dynamic_cast<Model*>(_drawable);
	if (model)
	{
		// Check to see if there's any animations with the ID on the joints.
		MeshSkin* skin = model->getSkin();
		if (skin)
		{
			Node* rootNode = skin->_rootNode;
			if (rootNode)
			{
				animation = rootNode->GetAnimation(id);
				if (animation)
					return animation;
			}
		}

		// Check to see if any of the model's material parameter's has an animation
		// with the given ID.
		Material* material = model->getMaterial();
		if (material)
		{
			// How to access material parameters? hidden on the Material::RenderState.
			std::vector<MaterialParameter*>::iterator itr = material->_parameters.begin();
			for (; itr != material->_parameters.end(); itr++)
			{
				GP_ASSERT(*itr);
				animation = ((MaterialParameter*)(*itr))->getAnimation(id);
				if (animation)
					return animation;
			}
		}
	}

	// look through form for animations.
	Form* form = dynamic_cast<Form*>(_drawable);
	if (form)
	{
		animation = form->getAnimation(id);
		if (animation)
			return animation;
	}

	// Look through this node's children for an animation with the specified ID.
	for (Node* child = GetFirstChild(); child != NULL; child = child->GetNextSibling())
	{
		animation = child->GetAnimation(id);
		if (animation)
			return animation;
	}
	*/
	
	return NULL;
}

Camera * Node::GetCamera() const
{
	return _camera;
}

void Node::SetCamera(Camera* camera)
{
	if (_camera == camera)
		return;

	if (_camera)
	{
		_camera->SetNode(NULL);
		SAFE_RELEASE(_camera);
	}

	_camera = camera;

	if (_camera)
	{
		_camera->AddRef();
		_camera->SetNode(this);
	}
}

Light * Node::GetLight() const
{
	return _light;
}

void Node::SetLight(Light* light)
{
	if (_light == light)
		return;

	if (_light)
	{
		_light->SetNode(NULL);
		SAFE_RELEASE(_light);
	}

	_light = light;

	if (_light)
	{
		_light->AddRef();
		_light->SetNode(this);
	}

	SetBoundsDirty();
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
			_drawable->SetNode(NULL);
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

		const Matrix& worldMatrix = GetWorldMatrix();

		// Start with our local bounding sphere
		// TODO: Incorporate bounds from entities other than mesh (i.e. particleemitters, audiosource, etc)
		bool empty = true;
		Terrain* terrain = dynamic_cast<Terrain*>(_drawable);
		if (terrain)
		{
			_bounds.Set(terrain->GetBoundingBox());
			empty = false;
		}
		Model* model = dynamic_cast<Model*>(_drawable);
		if (model && model->GetMesh())
		{
			if (empty)
			{
				_bounds.set(model->GetMesh()->GetBoundingSphere());
				empty = false;
			}
			else
			{
				_bounds.merge(model->GetMesh()->GetBoundingSphere());
			}
		}
		if (_light)
		{
			switch (_light->GetLightType())
			{
			case Light::POINT:
				if (empty)
				{
					_bounds.Set(Vector3::Zero(), _light->GetRange());
					empty = false;
				}
				else
				{
					_bounds.Merge(BoundingSphere(Vector3::Zero(), _light->GetRange()));
				}
				break;
			case Light::SPOT:
				// TODO: Implement spot light bounds
				break;
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
				GP_ASSERT(model->getSkin()->getRootJoint());
				Node* jointParent = model->getSkin()->getRootJoint()->getParent();
				if (jointParent)
				{
					// TODO: Should we protect against the case where joints are nested directly
					// in the node hierachy of the model (this is normally not the case)?
					Matrix boundsMatrix;
					Matrix::multiply(getWorldMatrix(), jointParent->getWorldMatrix(), &boundsMatrix);
					_bounds.transform(boundsMatrix);
					applyWorldTransform = false;
				}
			}
			if (applyWorldTransform)
			{
				_bounds.transform(getWorldMatrix());
			}
		}

		// Merge this world-space bounding sphere with our childrens' bounding volumes.
		for (Node* n = GetFirstChild(); n != NULL; n = n->GetNextSibling())
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

Node* Node::Clone() const
{
	NodeCloneContext context;
	return CloneRecursive(context);
}

Node* Node::CloneSingleNode(NodeCloneContext &context) const
{
	Node* copy = Node::Create(id());
	context.RegisterClonedNode(this, copy);
	CloneInto(copy, context);
	return copy;
}

Node* Node::CloneRecursive(NodeCloneContext &context) const
{
	Node* copy = CloneSingleNode(context);
	GP_ASSERT(copy);

	// Add child nodes
	for (Node* child = GetFirstChild(); child != NULL; child = child->GetNextSibling())
	{
		Node* childCopy = child->CloneRecursive(context);
		GP_ASSERT(childCopy);
		copy->AddChild(childCopy);
		childCopy->Release();
	}

	return copy;
}

void Node::CloneInto(Node* node, NodeCloneContext& context) const
{
	GP_ASSERT(node);

	Transform::CloneInto(node, context);

	if (Drawable* drawable = GetDrawable())
	{
		Drawable* clone = drawable->Clone(context);
		node->SetDrawable(clone);
		Ref* ref = dynamic_cast<Ref*>(clone);
		if (ref)
			ref->Release();
	}
	if (Camera* camera = GetCamera())
	{
		Camera* clone = camera->Clone(context);
		node->SetCamera(clone);
		Ref* ref = dynamic_cast<Ref*>(clone);
		if (ref)
			ref->Release();
	}
	if (Light* light = GetLight())
	{
		Light* clone = light->Clone(context);
		node->SetLight(clone);
		Ref* ref = dynamic_cast<Ref*>(clone);
		if (ref)
			ref->Release();
	}
	if (AudioSource* audio = GetAudioSource())
	{
		AudioSource* clone = audio->Clone(context);
		node->SetAudioSource(clone);
		Ref* ref = dynamic_cast<Ref*>(clone);
		if (ref)
			ref->Release();
	}
	if (_tags)
	{
		node->_tags = new std::map<std::string, std::string>(_tags->begin(), _tags->end());
	}

	node->_world = _world;
	node->_bounds = _bounds;

	// TODO: Clone the rest of the node data.
}

AudioSource* Node::GetAudioSource() const
{
	return _audioSource;
}

void Node::SetAudioSource(AudioSource* audio)
{
	if (_audioSource == audio)
		return;

	if (_audioSource)
	{
		_audioSource->SetNode(NULL);
		SAFE_RELEASE(_audioSource);
	}
		
	_audioSource = audio;

	if (_audioSource)
	{
		_audioSource->AddRef();
		_audioSource->SetNode(this);
	}
}

PhysicsCollisionObject* Node::GetCollisionObject() const
{
	return _collisionObject;
}

PhysicsCollisionObject* Node::SetCollisionObject(PhysicsCollisionObject::Type type, const PhysicsCollisionShape::Definition& shape, PhysicsRigidBody::Parameters* rigidBodyParameters, int group, int mask)
{
	SAFE_DELETE(_collisionObject);

	switch (type)
	{
	case PhysicsCollisionObject::RIGID_BODY:
		{
			_collisionObject = new PhysicsRigidBody(this, shape, rigidBodyParameters ? *rigidBodyParameters : PhysicsRigidBody::Parameters(), group, mask);
		}
		break;

	case PhysicsCollisionObject::GHOST_OBJECT:
		{
			_collisionObject = new PhysicsGhostObject(this, shape, group, mask);
		}
		break;

	case PhysicsCollisionObject::CHARACTER:
		{
			_collisionObject = new PhysicsCharacter(this, shape, rigidBodyParameters ? rigidBodyParameters->mass : 1.0f);
		}
		break;

	case PhysicsCollisionObject::VEHICLE:
		{
			_collisionObject = new PhysicsVehicle(this, shape, rigidBodyParameters ? *rigidBodyParameters : PhysicsRigidBody::Parameters());
		}
		break;

	case PhysicsCollisionObject::VEHICLE_WHEEL:
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

	case PhysicsCollisionObject::NONE:
		break;  // Already deleted, Just don't add a new collision object back.
	}

	return _collisionObject;
}

PhysicsCollisionObject* Node::SetCollisionObject(const char* url)
{
	// Load the collision object properties from file.
	Properties* properties = Properties::Create(url);
	if (properties == NULL)
	{
		GP_ERROR("Failed to load collision object file: %s", url);
		return NULL;
	}

	PhysicsCollisionObject* collisionObject = SetCollisionObject((strlen(properties->getNamespace()) > 0) ? properties : properties->getNextNamespace());
	SAFE_DELETE(properties);

	return collisionObject;
}

PhysicsCollisionObject* Node::SetCollisionObject(Properties* properties)
{
	SAFE_DELETE(_collisionObject);

	// Check if the properties is valid.
	if (!properties || !(strcmp(properties->getNamespace(), "collisionObject") == 0))
	{
		GP_ERROR("Failed to load collision object from properties object: must be non-null object and have namespace equal to 'collisionObject'.");
		return NULL;
	}

	if (const char* type = properties->getString("type"))
	{
		if (strcmp(type, "CHARACTER") == 0)
		{
			_collisionObject = PhysicsCharacter::create(this, properties);
		}
		else if (strcmp(type, "GHOST_OBJECT") == 0)
		{
			_collisionObject = PhysicsGhostObject::create(this, properties);
		}
		else if (strcmp(type, "RIGID_BODY") == 0)
		{
			_collisionObject = PhysicsRigidBody::create(this, properties);
		}
		else if (strcmp(type, "VEHICLE") == 0)
		{
			_collisionObject = PhysicsVehicle::create(this, properties);
		}
		else if (strcmp(type, "VEHICLE_WHEEL") == 0)
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
			_collisionObject = PhysicsVehicleWheel::create(this, properties);
		}
		else
		{
			GP_ERROR("Unsupported collision object type '%s'.", type);
			return NULL;
		}
	}
	else
	{
		GP_ERROR("Failed to load collision object from properties object; required attribute 'type' is missing.");
		return NULL;
	}

	return _collisionObject;
}

AIAgent* Node::GetAgent() const
{
	// Lazily create a new Agent for this Node if we don't have one yet.
	// Basically, all Nodes by default can have an Agent, we just won't
	// waste the memory unless they request one.
	if (!_agent)
	{
		_agent = AIAgent::create();
		_agent->_node = const_cast<Node*>(this);
		Game::getInstance()->getAIController()->addAgent(_agent);
	}

	return _agent;
}

void Node::SetAgent(AIAgent* agent)
{
	if (agent == _agent)
		return;

	if (_agent)
	{
		Game::getInstance()->getAIController()->removeAgent(_agent);
		_agent->SetNode(NULL);
		SAFE_RELEASE(_agent);
	}

	_agent = agent;

	if (_agent)
	{
		_agent->AddRef();
		_agent->SetNode(this);
		Game::getInstance()->getAIController()->addAgent(_agent);
	}
}

Ref* Node::GetUserObject() const
{
	return _userObject;
}

void Node::SetUserObject(Ref* obj)
{
	_userObject = obj;
}

NodeCloneContext::NodeCloneContext()
{
}

NodeCloneContext::~NodeCloneContext()
{
}

Animation* NodeCloneContext::FindClonedAnimation(const Animation* animation)
{
	GP_ASSERT(animation);

	std::map<const Animation*, Animation*>::iterator it = _clonedAnimations.find(animation);
	return it != _clonedAnimations.end() ? it->second : NULL;
}

void NodeCloneContext::RegisterClonedAnimation(const Animation* original, Animation* clone)
{
	GP_ASSERT(original);
	GP_ASSERT(clone);

	_clonedAnimations[original] = clone;
}

Node* NodeCloneContext::FindClonedNode(const Node* node)
{
	GP_ASSERT(node);

	std::map<const Node*, Node*>::iterator it = _clonedNodes.find(node);
	return it != _clonedNodes.end() ? it->second : NULL;
}

void NodeCloneContext::RegisterClonedNode(const Node* original, Node* clone)
{
	GP_ASSERT(original);
	GP_ASSERT(clone);

	_clonedNodes[original] = clone;
}

} // namespace scythe