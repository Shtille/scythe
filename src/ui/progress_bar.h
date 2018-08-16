#ifndef __SCYTHE_PROGRESS_BAR_H__
#define __SCYTHE_PROGRESS_BAR_H__

#include "rect.h"

namespace scythe {

	//! Standart horizontal progress bar class
	class ProgressBar : public RectTextured {
	public:
		explicit ProgressBar(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader,
			sht::graphics::Texture * texture, F32 progress_max,
			F32 x, F32 y, F32 width, F32 height, U32 flags);

		void PositionIncrease(F32 value);
		void PositionToBeginning();
		void PositionToEnd();
		
		bool IsDone() const;

	protected:
		F32 progress_;
		F32 progress_end_;
	};

} // namespace scythe

#endif