#ifndef __SCYTHE_OPENGL_FRAMEBUFFER_CONFIG_H__
#define __SCYTHE_OPENGL_FRAMEBUFFER_CONFIG_H__

namespace scythe {

	/**
	 * @brief      Declares OpenGL framebuffer configuration
	 */
	struct OpenGLFramebufferConfiguration
	{
		int red_bits;
		int green_bits;
		int blue_bits;
		int alpha_bits;
		int depth_bits;
		int stencil_bits;
		int accum_red_bits;
		int accum_green_bits;
		int accum_blue_bits;
		int accum_alpha_bits;
		int aux_buffers;
		int samples;
		bool stereo;
		bool sRGB;
		bool doublebuffer;
		bool transparent;
		int handle;
	};

} // namespace scythe

#endif // __SCYTHE_OPENGL_FRAMEBUFFER_CONFIG_H__