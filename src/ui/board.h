#ifndef __SCYTHE_BOARD_H__
#define __SCYTHE_BOARD_H__

#include "rect.h"

namespace scythe {

	//! Colored board class
	class ColoredBoard : public RectColored {
	public:
		explicit ColoredBoard(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader,
			const vec4& color, F32 width, F32 height, F32 other_coord, F32 value_min, F32 value_max,
			F32 velocity, bool min_position, bool is_vertical, U32 flags);

		bool IsMoving() const;
		bool IsPosMin() const;
		bool IsPosMax() const;

		void Move();

		virtual void Update(F32 sec) override;

	protected:
		F32 min_value_;
		F32 max_value_;
		F32 velocity_;
		bool is_vertical_;
		bool move_positive_;
		bool move_active_;
	};

} // namespace scythe

#endif