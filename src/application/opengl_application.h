#ifndef __SCYTHE_OPENGL_APPLICATION_H__
#define __SCYTHE_OPENGL_APPLICATION_H__

#include "desktop_application.h"

namespace scythe {

	class OpenGlApplication : public DesktopApplication {
	public:
		OpenGlApplication();
		virtual ~OpenGlApplication();

		bool InitApi() final;
		void DeinitApi() final;
		void BeginFrame() final;
		void EndFrame() final;
	};

} // namespace scythe

#endif