#ifndef __SCYTHE_OPENGL_GRAPHICS_PROVIDER_H__
#define __SCYTHE_OPENGL_GRAPHICS_PROVIDER_H__

#ifndef SCYTHE_USE_OPENGL
# error "OpenGL should be enabled to use this header"
#endif

#include "graphics_provider.h"

namespace scythe {

	/**
	 * @brief      This class describes an OpenGL graphics provider.
	 * @details    Should be created and initalized during Application::Initialize phase.
	 */
	class OpenGLGraphicsProvider final
	: public GraphicsProvider
	{
	public:

		/**
		 * @brief      Default constructor is not allowed.
		 */
		OpenGLGraphicsProvider() = delete;
		
		/**
		 * @brief      Constructs a new instance.
		 * @details    The following parameters can be adjusted:
		 *               - context major version
		 *               - context minor version
		 *               - multisample usage flag
		 *               - MSAA samples count
		 * 
		 * @param[in]  application  The application
		 */
		explicit OpenGLGraphicsProvider(Application* application);

	public:

		/**
		 * @brief      Gets the context major version.
		 *
		 * @return     The context major version.
		 */
		uint8_t GetContextMajorVersion() const;

		/**
		 * @brief      Gets the context minor version.
		 *
		 * @return     The context minor version.
		 */
		uint8_t GetContextMinorVersion() const;

		/**
		 * @brief      Determines if multisample.
		 *
		 * @return     True if multisample, False otherwise.
		 */
		bool IsMultisample() const;

		/**
		 * @brief      Gets the MSAA samples.
		 *
		 * @return     The MSAA samples count.
		 */
		uint8_t GetMSAASamples() const;

		/**
		 * @brief      Sets the context major version.
		 * @details    Should be called on Application::Initialize.
		 *
		 * @param[in]  context_major_version  The context major version
		 */
		void SetContextMajorVersion(uint8_t context_major_version);

		/**
		 * @brief      Sets the context minor version.
		 * @details    Should be called on Application::Initialize.
		 *
		 * @param[in]  context_minor_version  The context minor version
		 */
		void SetContextMinorVersion(uint8_t context_minor_version);

		/**
		 * @brief      Sets the multisample.
		 * @details    Should be called on Application::Initialize.
		 *
		 * @param[in]  flag  The flag
		 */
		void SetMultisample(bool flag);

		/**
		 * @brief      Sets the MSAA samples count.
		 * @details    Should be called on Application::Initialize.
		 *
		 * @param[in]  msaa_samples  The MSAA samples
		 */
		void SetMSAASamples(uint8_t msaa_samples);

		/**
		 * @brief      Sets the swap interval.
		 *
		 * @param[in]  interval  The interval
		 */
		void SetSwapInterval(int interval);

		/**
		 * @brief      Makes a context current.
		 */
		void MakeContextCurrent();

	protected:

		bool Initialize() override;
		void Deinitialize() override;

		void BeginFrame() override;
		void EndFrame() override;

	private:
		uint8_t context_major_version_;
		uint8_t context_minor_version_;
		bool is_multisample_;				//!< is multisample context
		uint8_t msaa_samples_;
	};

} // namespace scythe

#endif