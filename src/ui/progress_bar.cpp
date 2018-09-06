#include "progress_bar.h"

namespace scythe {

	ProgressBar::ProgressBar(Renderer * renderer, Shader * shader,
			Texture * texture, F32 progress_max,
			F32 x, F32 y, F32 width, F32 height, U32 flags)
	: RectTextured(renderer, shader, texture, x, y, width, height, flags)
	, progress_(0.0f)
	, progress_end_(progress_max)
	{
	}
	void ProgressBar::PositionIncrease(F32 value)
	{
		progress_ += value;
	}
	void ProgressBar::PositionToBeginning()
	{
		progress_ = 0.0f;
	}
	void ProgressBar::PositionToEnd()
	{
		progress_ = progress_end_;
	}
	bool ProgressBar::IsDone() const
	{
		return progress_ >= progress_end_;
	}

} // namespace scythe