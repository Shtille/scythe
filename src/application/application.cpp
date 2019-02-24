#include "application.h"

#include "platform/window_controller.h"

#include "time/time_manager.h"
#include "resource_manager.h"

#include <cstdlib>
#include <algorithm> // for std::max

namespace scythe {

	Application * Application::app_ = nullptr;

	Application::Application()
	: input_listener_(nullptr)
	, renderer_(nullptr)
	, width_(800)
	, height_(600)
	, aspect_ratio_(1.0f)
	, color_bits_(32)
	, depth_bits_(24)
	, stencil_bits_(0)
	, need_take_screenshot_(false)
	, framebuffer_size_(0)
	, inv_framebuffer_size_(0.0f)
	{

	}
	Application::~Application()
	{

	}
	Application* Application::GetInstance()
	{
		return app_;
	}
	InputListener * Application::GetInputListener()
	{
		return input_listener_;
	}
	void Application::SetInputListener(InputListener * listener)
	{
		input_listener_ = listener;
	}
	void Application::SetClipboardText(const char *text)
	{
		PlatformSetClipboardText(text);
	}
	std::string Application::GetClipboardText()
	{
		return PlatformGetClipboardText();
	}
	void Application::ComputeFramebufferSize()
	{
		int size = std::max(width_, height_);
		if (size <= 1024)
			framebuffer_size_ = 1024;
		else if (size <= 2048)
			framebuffer_size_ = 2048;
		else
			framebuffer_size_ = 4096;
		inv_framebuffer_size_ = 1.0f / (float)framebuffer_size_;
	}
	const float Application::GetFrameTime()
	{
		return 1.0f / GetDesiredFrameRate();
	}
	const float Application::GetFrameRate()
	{
		return TimeManager::GetInstance()->GetFrameRate();
	}
	void Application::InitializeManagers()
	{
		TimeManager::CreateInstance();
		ResourceManager::CreateInstance();

		// Our engine uses fixed time steps, so make it shared for any consumer
		TimeManager::GetInstance()->SetFixedFrameTime(GetFrameTime());
	}
	void Application::DeinitializeManagers()
	{
		ResourceManager::DestroyInstance();
		TimeManager::DestroyInstance();
	}
	void Application::UpdateManagers()
	{
		// Update time manager
		TimeManager::GetInstance()->Update();
	}
	bool Application::PreStartInit()
	{
		return true;
	}
	bool Application::Load()
	{
		return true;
	}
	void Application::Unload()
	{
	}
	void Application::Update()
	{
	}
	void Application::UpdatePhysics(float sec)
	{
	}
	void Application::Render()
	{
	}
	const char* Application::GetTitle()
	{
		return "scythe application";
	}
	const float Application::GetDesiredFrameRate()
	{
		return 60.0f;
	}
	int Application::width()
	{
		return width_;
	}
	int Application::height()
	{
		return height_;
	}
	U32 Application::color_bits()
	{
		return static_cast<U32>(color_bits_);
	}
	U32 Application::depth_bits()
	{
		return static_cast<U32>(depth_bits_);
	}
	U32 Application::stencil_bits()
	{
		return static_cast<U32>(stencil_bits_);
	}

} // namespace scythe