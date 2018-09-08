#ifndef __SCYTHE_SCREEN_QUAD_MODEL_H__
#define __SCYTHE_SCREEN_QUAD_MODEL_H__

#include "generated_model.h"

namespace scythe {

	class ScreenQuadModel final : public GeneratedModel {
	public:
		ScreenQuadModel(Renderer * renderer);
		~ScreenQuadModel();
		
		void Create();
		
	protected:
	};

} // namespace scythe

#endif