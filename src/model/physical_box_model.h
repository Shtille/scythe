#ifndef __SCYTHE_PHYSICAL_BOX_MODEL_H__
#define __SCYTHE_PHYSICAL_BOX_MODEL_H__

#include "generated_model.h"

namespace scythe {

	class PhysicalBoxModel final : public GeneratedModel {
	public:
		PhysicalBoxModel(Renderer * renderer, float size_x, float size_y, float size_z, float size_u, float size_v);
		~PhysicalBoxModel();
		
		void Create();

		const Vector3& sizes() const;
		const Vector2& texture_sizes() const;
		
	protected:
		Vector3 sizes_;
		Vector2 texture_sizes_;
	};

} // namespace scythe

#endif