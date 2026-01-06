#ifndef __SCYTHE_APPLICATION_H__
#define __SCYTHE_APPLICATION_H__

#include "types.h"
#include "singleton.h"
#include "expandable.h"
#include "graphics_controller.h"
#include "physics_controller.h"
#include "logics_controller.h"

namespace scythe {

	// Forward declarations
	class GraphicsProvider;

	/**
	 * @brief      This class describes an application.
	 */
	class Application 
	: public ManagedSingleton<Application>
	, public Expandable<Application>
	{
		friend class ManagedSingleton<Application>;

	public:
		int Run(int argc, char const** argv);
		void Terminate(); //!< terminates application

		virtual bool Initialize() = 0;
		virtual void Deinitialize() = 0;

		const float GetFrameTime() const; //!< returns fixed frame time (1 / fps desired)
		const float GetFrameRate() const; //!< returns real FPS (not desired)

		const uint8_t GetColorBits() const;
		const uint8_t GetDepthBits() const;
		const uint8_t GetStencilBits() const;

		virtual const char* GetTitle() const;
		virtual const float GetDesiredFrameRate() const; //!< average frame rate for application that we desire
		virtual const bool IsBenchmark() const;
		virtual const bool IsMultisample() const;

	public:
		void Show();
		void Hide();
		bool IsVisible() const;

	protected:
		virtual bool CreateSurface() = 0;
		virtual void DestroySurface() = 0;

		Application();
		~Application();

	private:
		void InitializeManagers();
		void DeinitializeManagers();
		void UpdateManagers();

		void Update();
		void RunMainCycle();

	protected:
		GraphicsProvider* graphics_provider_;		//!< graphics provider (should be manually created by user)
		GraphicsController* graphics_controller_;	//!< graphics controller (required)
		PhysicsController* physics_controller_;		//!< physics controller (optional)
		LogicsController* logics_controller_;		//!< logics controller (optional)
		bool need_quit_;
		uint8_t pad[3];
	};

} // namespace scythe

#endif