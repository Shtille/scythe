#ifndef __SCYTHE_APPLICATION_H__
#define __SCYTHE_APPLICATION_H__

#include "common/types.h"

#include <string>

namespace scythe {

	// Predeclarations
	class Renderer;

	//! Base class for application
	class Application {
	public:
		Application();
		virtual ~Application();
		
		static Application* GetInstance(); //!< it's not a singleton function

		template<class T>
		T * Upcast()
		{
			return dynamic_cast<T*>(this);
		}
		
		// Clipboard operations
		void SetClipboardText(const char *text);
		std::string GetClipboardText();

		// API specific functions
		virtual bool InitApi() = 0;
		virtual void DeinitApi() = 0;
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		int width();
		int height();
		U32 color_bits();
		U32 depth_bits();
		U32 stencil_bits();

		const float GetFrameTime(); //!< returns fixed frame time (1 / fps desired)
		const float GetFrameRate(); //!< returns real FPS (not desired)

		void InitializeManagers();
		void DeinitializeManagers();
		void UpdateManagers();

		// --- User defined functions ---

		// User data loading/unloading
		virtual bool PreStartInit(); //!< All window initialization goes here (OpenGL hasn't been initialized)
		virtual bool Load();
		virtual void Unload();

		virtual void Update();
		virtual void UpdatePhysics(float sec);
		virtual void Render();

		// Application parameters
		virtual const char* GetTitle();
		virtual const float GetDesiredFrameRate(); //!< average frame rate for application that we desire

	protected:

		void ComputeFramebufferSize();

		Renderer *renderer_;			//!< our renderer object
		int width_;						//!< width of the window
		int height_;					//!< height of the window
		float aspect_ratio_;			//!< aspect ratio of window
		unsigned char color_bits_;		//!< number of bits for color buffer
		unsigned char depth_bits_;		//!< number of bits for depth buffer
		unsigned char stencil_bits_;    //!< number of bits for stencil buffer
		bool need_take_screenshot_;		//!< do we need to take a screenshot?
		int framebuffer_size_;			//!< optimal size for framebuffer
		float inv_framebuffer_size_;	//!< inverted framebuffer size for shaders

	protected:

		static Application * app_;
	};

} // namespace scythe

#endif