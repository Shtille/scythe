#include <scythe/main.h>
#include <scythe/desktop_application.h>
#include <scythe/opengl_provider.h>
#include <scythe/opengl_include.h>

class BackgroundApplication final
: public scythe::DesktopApplication
, public scythe::GraphicsController
, public scythe::KeyboardController
{
public:
	bool Initialize() override
	{
		graphics_provider_ = new scythe::OpenGLGraphicsProvider(this);
		graphics_controller_ = this;
		keyboard_controller_ = this;
		return true;
	}
	void Deinitialize() override
	{
		delete graphics_provider_;
	}
	// Derived from scythe::GraphicsController
	bool LoadGraphicsResources() override { return true; }
	void UnloadGraphicsResources() override {}
	void Render() override
	{
		// turquoise background color
		glClearColor(0.25f, 0.88f, 0.81f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	// Derived from scythe::KeyboardController
	void OnKeyDown(scythe::KeyboardKey key, scythe::KeyModifiers modifiers) override
	{
		if (keyboard_state_.IsKeyPressed(key, scythe::KeyboardKey::kEscape))
		{
			Terminate();
		}
		else if (keyboard_state_.IsKeyPressed(key, scythe::KeyboardKey::kF))
		{
			ToggleFullscreen();
		}
	}
};

SCYTHE_DECLARE_MAIN(BackgroundApplication)