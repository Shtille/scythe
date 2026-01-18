#include <scythe/main.h>
#include <scythe/desktop_application.h>
#include <scythe/graphics_provider.h>

class MinimalApplication final
: public scythe::DesktopApplication
, public scythe::GraphicsController
{
public:
	bool Initialize() override
	{
		graphics_provider_ = new scythe::GraphicsProvider(this);
		graphics_controller_ = this;
		return true;
	}
	void Deinitialize() override
	{
		delete graphics_provider_;
	}
	// Derived from scythe::GraphicsController
	bool LoadGraphicsResources() { return true; }
	void UnloadGraphicsResources() {}
	void Render() {}
};

SCYTHE_DECLARE_MAIN(MinimalApplication)