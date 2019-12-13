#include "physics_collision_shape.h"

#include "common/sc_assert.h"
#include "common/sc_delete.h"

#include "model/mesh.h"

#include "bullet_include.h"

#include <cstring>

namespace scythe {

	PhysicsCollisionShape::PhysicsCollisionShape(Type type, btCollisionShape* shape, btStridingMeshInterface* mesh_interface)
	: type_(type)
	, shape_(shape)
	, mesh_interface_(mesh_interface)
	{

	}
	PhysicsCollisionShape::~PhysicsCollisionShape()
	{
		if (shape_)
		{
			if (type_ == kMesh)
			{
				// Also need to delete the btTriangleIndexVertexArray, if it exists.
				SC_SAFE_DELETE(mesh_interface_);
			}
			else if (type_ == kHeightfield)
			{
				//
			}
			// Free the bullet shape.
			SC_SAFE_DELETE(shape_);
		}
	}
	PhysicsCollisionShape::Type PhysicsCollisionShape::type() const
	{
		return type_;
	}
	btCollisionShape * PhysicsCollisionShape::shape() const
	{
		return shape_;
	}
	PhysicsCollisionShape::Definition PhysicsCollisionShape::DefineBox()
	{
		Definition d;
		d.type = kBox;
		d.is_explicit = false;
		d.center_absolute = false;
		return d;
	}
	PhysicsCollisionShape::Definition PhysicsCollisionShape::DefineBox(const Vector3& extents, const Vector3& center, bool absolute)
	{
		Definition d;
		d.type = kBox;
		memcpy(d.data.box.extents, &extents.x, sizeof(float) * 3);
		memcpy(d.data.box.center, &center.x, sizeof(float) * 3);
		d.is_explicit = true;
		d.center_absolute = absolute;
		return d;
	}

	PhysicsCollisionShape::Definition PhysicsCollisionShape::DefineSphere()
	{
		Definition d;
		d.type = kSphere;
		d.is_explicit = false;
		d.center_absolute = false;
		return d;
	}
	PhysicsCollisionShape::Definition PhysicsCollisionShape::DefineSphere(float radius, const Vector3& center, bool absolute)
	{
		Definition d;
		d.type = kSphere;
		d.data.sphere.radius = radius;
		memcpy(d.data.sphere.center, &center.x, sizeof(float) * 3);
		d.is_explicit  = true;
		d.center_absolute = absolute;
		return d;
	}
	PhysicsCollisionShape::Definition PhysicsCollisionShape::DefineCapsule()
	{
		Definition d;
		d.type = kCapsule;
		d.is_explicit = false;
		d.center_absolute = false;
		return d;
	}
	PhysicsCollisionShape::Definition PhysicsCollisionShape::DefineCapsule(float radius, float height, const Vector3& center, bool absolute)
	{
		Definition d;
		d.type = kCapsule;
		d.data.capsule.radius = radius;
		d.data.capsule.height = height;
		memcpy(d.data.capsule.center, &center.x, sizeof(float) * 3);
		d.is_explicit = true;
		d.center_absolute = absolute;
		return d;
	}
	// PhysicsCollisionShape::Definition PhysicsCollisionShape::DefineHeightfield()
	// {
	// 	Definition d;
	// 	d.type = kHeightfield;
	// 	d.is_explicit = false;
	// 	d.center_absolute = false;
	// 	return d;
	// }
	// PhysicsCollisionShape::Definition PhysicsCollisionShape::DefineHeightfield(HeightField* heightfield)
	// {
	// 	SC_ASSERT(heightfield);

	// 	heightfield->addRef();

	// 	Definition d;
	// 	d.type = kHeightfield;
	// 	d.data.heightfield = heightfield;
	// 	d.is_explicit = true;
	// 	d.center_absolute = false;
	// 	return d;
	// }
	PhysicsCollisionShape::Definition PhysicsCollisionShape::DefineMesh(Mesh* mesh)
	{
		SC_ASSERT(mesh);

		mesh->AddRef();

		Definition d;
		d.type = kMesh;
		d.data.mesh = mesh;
		d.is_explicit = true;
		d.center_absolute = false;
		return d;
	}

	PhysicsCollisionShape::Definition::Definition()
	: type(kNone)
	, is_explicit(false)
	, center_absolute(false)
	{
		memset(&data, 0, sizeof(data));
	}
	PhysicsCollisionShape::Definition::Definition(const Definition& definition)
	{
		// Bitwise-copy the definition object (equivalent to default copy constructor).
		memcpy(this, &definition, sizeof(PhysicsCollisionShape::Definition));

		// Handle the types that have reference-counted members.
		if (type == PhysicsCollisionShape::kMesh)
		{
			SC_ASSERT(data.mesh);
			data.mesh->AddRef();
		}
		else if (type == PhysicsCollisionShape::kHeightfield)
		{
			// 	if (data.heightfield)
			// 		data.heightfield->AddRef();
		}
	}
	PhysicsCollisionShape::Definition::~Definition()
	{
		if (type == PhysicsCollisionShape::kMesh)
		{
			SC_SAFE_RELEASE(data.mesh);
		}
		else if (type == PhysicsCollisionShape::kHeightfield)
		{
			// 	SC_SAFE_RELEASE(data.heightfield);
		}
	}
	PhysicsCollisionShape::Definition& PhysicsCollisionShape::Definition::operator=(const Definition& definition)
	{
		if (this != &definition)
		{
			// Bitwise-copy the definition object (equivalent to default copy constructor).
			memcpy(this, &definition, sizeof(PhysicsCollisionShape::Definition));

			// Handle the types that have reference-counted members.
			if (type == PhysicsCollisionShape::kMesh)
			{
				SC_ASSERT(data.mesh);
				data.mesh->AddRef();
			}
			else if (type == PhysicsCollisionShape::kHeightfield)
			{
				// 	if (data.heightfield)
				// 		data.heightfield->AddRef();
			}
		}

		return *this;
	}
	bool PhysicsCollisionShape::Definition::IsEmpty() const
	{
		return type == kNone;
	}

} // namespace scythe