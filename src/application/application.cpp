#include <scythe/application.h>

#include <clocale>

#include <scythe/defines.h>
#include <scythe/time_manager.h>
#include <scythe/resource_manager.h>
#include <scythe/graphics_provider.h>

#include "../platform/platform_inner.h"

namespace scythe {

	Application::Application()
	: graphics_provider_(nullptr)
	, graphics_controller_(nullptr)
	, physics_controller_(nullptr)
	, logics_controller_(nullptr)
	, need_quit_(false)
	, platform_data_(nullptr)
	{
	}
	Application::~Application()
	{
	}
	void Application::InitializeManagers()
	{
		TimeManager::CreateInstance<TimeManager>();
		ResourceManager::CreateInstance<ResourceManager>();

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
	void Application::Update()
	{
		if (logics_controller_ != nullptr)
			logics_controller_->UpdateLogics();
	}
	void Application::RunMainCycle()
	{
		const Clock& clock = TimeManager::GetInstance()->GetClock();
		float time_gameclock = clock.GetTime();
		float time_physics_prev, time_physics_curr;
		time_physics_prev = time_physics_curr = time_gameclock;
		const float kTickTime = 1.0f / GetDesiredFrameRate();

		while (!need_quit_)
		{
			// Update physics
			if (physics_controller_ != nullptr)
			{
				time_physics_curr = clock.GetTime();
				physics_controller_->UpdatePhysics(time_physics_curr - time_physics_prev);
				time_physics_prev = time_physics_curr;
			}

			UpdateManagers();

			// Game clock part of the loop. Ticks for every tick_time at average.
			float dt = clock.GetTime() - time_gameclock;

			while (dt >= kTickTime)
			{
				dt -= kTickTime;
				time_gameclock += kTickTime;

				// Poll platform events
				platform::PollEvents();

				// Update application
				Update();
			}

			// Render a frame
			graphics_provider_->BeginFrame();
			graphics_controller_->Render();
			graphics_provider_->EndFrame();
		}
	}
	int Application::Run(int argc, char const** argv)
	{
		int result = 1;

		// Create platform data and store it
		platform::Data* data = platform::CreateData();
		if (!data)
			return 1;
		platform_data_ = reinterpret_cast<void*>(data);

		// Set proper text encoding to let use non-english characters
		std::setlocale(LC_CTYPE, "UTF-8");

		// This code is for Mac OS X and iOS.
		// Under these platforms all initializing code doing in coressponding classes.
		platform::ChangeDirectoryToResources();

		// Initialize window parameters and necessary controllers (graphics, physics, logics)
		if (Initialize())
		{
			// Make sure that graphics controller has been initalized previously
			SCYTHE_ASSERT(graphics_controller_ != nullptr && 
				"graphics_controller_ should be initialized at Initialize()");

			// Initialize platform stuff
			if (platform::Initialize())
			{
				// Create surface
				if (CreateSurface())
				{
					// Initialize graphics API
					if (graphics_provider_->Initialize())
					{
						// Initialize all managers
						InitializeManagers();

						// Load graphics dependent resources (textures, shaders, etc.)
						if (graphics_controller_->LoadGraphicsResources())
						{
							result = 0;

							// Show window
							Show();

							// Run main cycle routine
							RunMainCycle();
						}
						// Unload graphics dependent resources
						graphics_controller_->UnloadGraphicsResources();

						// Deinitialize all managers
						DeinitializeManagers();
					}
					// Deinitialize graphics API
					graphics_provider_->Deinitialize();
				}
				// Destroy surface
				DestroySurface();
			}
			// Deinitialize platform stuff
			platform::Deinitialize();
		}
		// Deinitialize user data
		Deinitialize();

		// Destroy platform data
		platform::DestroyData(data);

		return result;
	}
	void Application::Terminate()
	{
		need_quit_ = true;
	}
	void Application::Show()
	{
		platform::window::Show();
	}
	void Application::Hide()
	{
		platform::window::Hide();
	}
	bool Application::IsVisible() const
	{
		return platform::window::IsVisible();
	}
	const float Application::GetFrameTime() const
	{
		return 1.0f / GetDesiredFrameRate();
	}
	const float Application::GetFrameRate() const
	{
		return TimeManager::GetInstance()->GetFrameRate();
	}
	const uint8_t Application::GetColorBits() const
	{
		return graphics_provider_->color_bits_;
	}
	const uint8_t Application::GetDepthBits() const
	{
		return graphics_provider_->depth_bits_;
	}
	const uint8_t Application::GetStencilBits() const
	{
		return graphics_provider_->stencil_bits_;
	}
	const char* Application::GetTitle() const
	{
		return "scythe application";
	}
	const float Application::GetDesiredFrameRate() const
	{
		return 60.0f;
	}
	const bool Application::IsBenchmark() const
	{
		return false;
	}

} // namespace scythe