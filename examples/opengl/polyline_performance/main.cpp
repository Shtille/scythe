/**
 * @file main.cpp
 * @brief Entry point of the application.
 *
 * This example shows how to render polyline with shader.
 */

#include <scythe/main.h>
#include <scythe/desktop_application.h>
#include <scythe/opengl/opengl_provider.h>
#include <scythe/opengl/opengl_include.h>

#include "drawer.h"

class PolylineApplication final
: public scythe::DesktopApplication
, public scythe::GraphicsController
, public scythe::KeyboardController
, public scythe::WindowController
{
public:
	bool Initialize() override
	{
		graphics_provider_ = new scythe::OpenGLGraphicsProvider(this);
		auto* graphics_provider = dynamic_cast<scythe::OpenGLGraphicsProvider*>(graphics_provider_);
		graphics_provider->SetContextVersion(3, 3);
		graphics_controller_ = this;
		keyboard_controller_ = this;
		window_controller_ = this;
		return true;
	}
	void Deinitialize() override
	{
		delete graphics_provider_;
	}
	// Derived from scythe::GraphicsController
	bool LoadGraphicsResources() override
	{
		viewport_.width = GetWidth();
		viewport_.height = GetHeight();
		viewport_.aspect_ratio = GetAspectRatio();

		const PointArray points = {
			{-0.5f,  0.0f},
			{ 0.0f,  0.5f},
			{ 0.0f, -0.5f},
			{ 0.5f,  0.0f},
		};

		drawer_ = new QuadDrawer(&viewport_);
		if (!drawer_)
			return false;
		if (!drawer_->CreateData(points))
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
		glClearColor(1.f, 1.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		drawer_->Render();
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
	// Derived from scythe::WindowController
	void OnResize(int width, int height) override
	{
		viewport_.width = GetWidth();
		viewport_.height = GetHeight();
		viewport_.aspect_ratio = GetAspectRatio();
	}
private:
	Viewport viewport_;
	QuadDrawer* drawer_;
};

SCYTHE_DECLARE_MAIN(PolylineApplication)