/**
 * @file main.cpp
 * @brief Entry point of the application.
 *
 * This example shows how to partition items with compute shader.
 * 
 * Originally we have single array of objects with different opacity.
 * We want opaque objects to be rendered first, transparent ones after.
 * Opacity is controlled by color alpha.
 */

#include <scythe/main.h>
#include <scythe/desktop_application.h>
#include <scythe/opengl/opengl_provider.h>
#include <scythe/opengl/opengl_include.h>

#include "drawer.h"

class ComputeApplication final
: public scythe::DesktopApplication
, public scythe::GraphicsController
, public scythe::KeyboardController
{
public:
	// Derived from scythe::Application
	bool Initialize() override
	{
		graphics_provider_ = new scythe::OpenGLGraphicsProvider(this);
		auto* graphics_provider = dynamic_cast<scythe::OpenGLGraphicsProvider*>(graphics_provider_);
		graphics_provider->SetContextVersion(4, 6);
		graphics_controller_ = this;
		keyboard_controller_ = this;
		return true;
	}
	void Deinitialize() override
	{
		delete graphics_provider_;
	}
	const char* GetTitle() const override
	{
		return "Compute partition";
	}
	// Derived from scythe::GraphicsController
	bool LoadGraphicsResources() override
	{
		drawer_ = new Drawer(20);
		if (!drawer_)
			return false;
		if (!drawer_->CreateData())
			return false;
		if (!drawer_->Load())
			return false;

		return true;
	}
	void UnloadGraphicsResources() override
	{
		if (drawer_)
		{
			drawer_->Unload();
			delete drawer_;
			drawer_ = nullptr;
		}
	}
	void Render() override
	{
		// turquoise background color
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		drawer_->Render(render_filter_);
	}
	// Derived from scythe::KeyboardController
	void OnKeyDown(scythe::KeyboardKey key, scythe::KeyModifiers modifiers) override
	{
		if (keyboard_state_.IsKeyPressed(key, scythe::KeyboardKey::kSpace))
		{
			render_filter_ = ::NextRenderFilter(render_filter_);
		}
		else if (keyboard_state_.IsKeyPressed(key, scythe::KeyboardKey::kEscape))
		{
			Terminate();
		}
		else if (keyboard_state_.IsKeyPressed(key, scythe::KeyboardKey::kF))
		{
			ToggleFullscreen();
		}
	}
private:
	Drawer* drawer_ = nullptr;
	RenderFilter render_filter_ = RenderFilter::kAll;
};

SCYTHE_DECLARE_MAIN(ComputeApplication)