#include "desktop_main_wrapper.h"

#include "platform_inner.h"

#include "application/desktop_application.h"
#include "time/clock.h"

int DesktopMainWrapper(int, const char**)
{
	if (!PlatformInit())
		return 1;

	if (!PlatformWindowCreate())
	{
		PlatformTerminate();
		return 2;
	}

	scythe::DesktopApplication * app = scythe::Application::GetInstance()->Upcast<scythe::DesktopApplication>();
	app->Center();

	if (app->InitApi())
	{
		app->InitializeManagers();
		if (app->Load())
		{
			app->Show();
			app->set_visible(true);

			scythe::Clock clock;
			float time_gameclock = clock.GetTime();
			float time_physics_prev, time_physics_curr;
			time_physics_prev = time_physics_curr = time_gameclock;
			const float kTickTime = 1.0f / app->GetDesiredFrameRate();

			while (!PlatformNeedQuit())
			{
				// Render a frame
				app->BeginFrame();
				app->Render();
				app->EndFrame();

				// Update physics
				time_physics_curr = clock.GetTime();
				app->UpdatePhysics(time_physics_curr - time_physics_prev);
				time_physics_prev = time_physics_curr;

				app->UpdateManagers();

				// Game clock part of the loop. Ticks for every tick_time at average.
				float dt = clock.GetTime() - time_gameclock;

				while (dt >= kTickTime)
				{
					dt -= kTickTime;
					time_gameclock += kTickTime;

					PlatformPollEvents();

					app->Update();
				}
			}
		}
		app->Unload(); // delete allocated objects (may be allocated partially)
		app->DeinitializeManagers();
        app->DeinitApi();
	}
	else
	{
		PlatformWindowDestroy();
	}

	PlatformTerminate();

	return 0;
}