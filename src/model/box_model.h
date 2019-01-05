#ifndef __SCYTHE_BOX_MODEL_H__
#define __SCYTHE_BOX_MODEL_H__

#include "generated_model.h"

namespace scythe {
		
	class BoxModel final : public GeneratedModel {
	public:
		BoxModel(Renderer * renderer, float size_x, float size_y, float size_z);
		~BoxModel();
		
		void Create();

		const Vector3& sizes() const;
		
	protected:
		Vector3 sizes_;
	};

} // namespace scythe

#endif