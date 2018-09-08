#ifndef __SCYTHE_CUBE_MODEL_H__
#define __SCYTHE_CUBE_MODEL_H__

#include "generated_model.h"

namespace scythe {

	class CubeModel final : public GeneratedModel {
	public:
		CubeModel(Renderer * renderer);
		~CubeModel();
		
		void Create();
		
	protected:
	};

} // namespace scythe

#endif