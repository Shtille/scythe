#ifndef __SCYTHE_TETRAHEDRON_MODEL_H__
#define __SCYTHE_TETRAHEDRON_MODEL_H__

#include "generated_model.h"

namespace scythe {

	class TetrahedronModel final : public GeneratedModel {
	public:
		TetrahedronModel(Renderer * renderer);
		~TetrahedronModel();
		
		void Create();
		
	protected:
	};

} // namespace scythe

#endif