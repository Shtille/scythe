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
	bool LoadGraphicsResources() override { return true; }
	void UnloadGraphicsResources() override {}
	void Render() override {}
};

SCYTHE_DECLARE_MAIN(MinimalApplication)