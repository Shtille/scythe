#include <scythe/main.h>
#include <scythe/desktop_application.h>
#include <scythe/graphics_provider.h>

class NoGraphicsProvider final
: public scythe::GraphicsProvider
{
public:
	NoGraphicsProvider()
	: scythe::GraphicsProvider(scythe::GraphicsCreationParams{24,0,0})
	{}
protected:
	bool Initialize() override { return true; }
	void Deinitialize() override {}
	void BeginFrame() override {}
	void EndFrame() override {}
};

class MinimalApplication final
: public scythe::DesktopApplication
, public scythe::GraphicsController
{
public:
	bool Initialize() override
	{
		graphics_provider_ = new NoGraphicsProvider();
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