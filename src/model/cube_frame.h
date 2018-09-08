#ifndef __SCYTHE_CUBE_FRAME_H__
#define __SCYTHE_CUBE_FRAME_H__

#include "generated_model.h"

namespace scythe {
		
	class CubeFrameModel final : public GeneratedModel {
	public:
		CubeFrameModel(Renderer * renderer);
		~CubeFrameModel();
		
		void Create();
		
	protected:
	};

} // namespace scythe

#endif