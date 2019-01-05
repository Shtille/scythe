#ifndef __SCYTHE_MATERIAL_H__
#define __SCYTHE_MATERIAL_H__

#include "math/vector3.h"

namespace scythe {

	//! Base material class
	struct Material
	{
		Vector3 ambient;
		Vector3 diffuse;
		Vector3 specular;
		Vector3 transmittance;
		Vector3 emission;
		float shininess;
		float dissolve;  // 1 == opaque; 0 == fully transparent
	};

	//! Interface class for material binder
	class MaterialBinderInterface {
	public:
		virtual ~MaterialBinderInterface() = default;

		virtual void Bind(Material * material) = 0;
	};

} // namespace scythe

#endif