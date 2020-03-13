#ifndef __SCYTHE_OPENGL_TEXTURE_H__
#define __SCYTHE_OPENGL_TEXTURE_H__

#include "../texture.h"

namespace scythe {

	//! Texture class for OpenGL
	class OpenGlTexture : public Texture {
	public:
		U32 GetSrcFormat();
		U32 GetSrcType();
		int GetInternalFormat();

		void SetFilter(Filter filter);
		void SetWrap(Wrap wrap);

	private:
		void ChooseTarget();
	};

} // namespace scythe

#endif