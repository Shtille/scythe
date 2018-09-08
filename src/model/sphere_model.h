#ifndef __SCYTHE_SPHERE_MODEL_H__
#define __SCYTHE_SPHERE_MODEL_H__

#include "generated_model.h"

namespace scythe {

	class SphereModel final : public GeneratedModel {
	public:
		SphereModel(Renderer * renderer, U32 slices = 20, U32 loops = 10, float radius = 1.0f);
		~SphereModel();
		
		void Create();
		
	protected:
		U32 slices_;    //!< number of vertices in sphere's longitude direction
		U32 loops_;     //!< number of vertices in sphere's latitude direction
		float radius_;  //!< radius of the sphere
	};

} // namespace scythe

#endif