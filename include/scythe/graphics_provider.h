#ifndef __SCYTHE_GRAPHICS_PROVIDER_H__
#define __SCYTHE_GRAPHICS_PROVIDER_H__

#include "types.h"
#include "non_copyable.h"

// Default color buffer bits
#ifndef SCYTHE_DEFAULT_COLOR_BITS
# define SCYTHE_DEFAULT_COLOR_BITS 32
#endif

// Default depth buffer bits
#ifndef SCYTHE_DEFAULT_DEPTH_BITS
# define SCYTHE_DEFAULT_DEPTH_BITS 24
#endif

// Default stencil buffer bits
#ifndef SCYTHE_DEFAULT_STENCIL_BITS
# define SCYTHE_DEFAULT_STENCIL_BITS 8
#endif

namespace scythe {

	/**
	 * @brief      This class describes a graphics provider.
	 */
	class GraphicsProvider
	: public NonCopyable
	{
		friend class Application;

	public:

		/**
		 * @brief      Default constructor is not allowed.
		 */
		GraphicsProvider() = delete;
		
		/**
		 * @brief      Constructs a new instance with default parameters.
		 * @details    The default parameters are:
		 *               - color buffer bits:       SCYTHE_DEFAULT_COLOR_BITS
		 *               - depth buffer bits:       SCYTHE_DEFAULT_DEPTH_BITS
		 *               - stencil buffer bits:     SCYTHE_DEFAULT_STENCIL_BITS
		 *             These parameters can be changed manually or via defines.
		 *             
		 * @param[in]  application  The application
		 */
		explicit GraphicsProvider(Application* application);

		/**
		 * @brief      Destroys the object.
		 */
		virtual ~GraphicsProvider() = default;

	public:

		/**
		 * @brief      Determines if initialized.
		 *
		 * @return     True if initialized, False otherwise.
		 */
		bool IsInitialized() const;

		/**
		 * @brief      Gets the color bits.
		 *
		 * @return     The color bits.
		 */
		uint8_t GetColorBits() const;

		/**
		 * @brief      Gets the depth bits.
		 *
		 * @return     The depth bits.
		 */
		uint8_t GetDepthBits() const;

		/**
		 * @brief      Gets the stencil bits.
		 *
		 * @return     The stencil bits.
		 */
		uint8_t GetStencilBits() const;

		/**
		 * @brief      Sets the color bits.
		 * @details    Overrides SCYTHE_DEFAULT_COLOR_BITS value.
		 *             Should be called on Application::Initialize.
		 *
		 * @param[in]  color_bits  The color bits
		 */
		void SetColorBits(uint8_t color_bits);

		/**
		 * @brief      Sets the depth bits.
		 * @details    Overrides SCYTHE_DEFAULT_DEPTH_BITS value.
		 *             Should be called on Application::Initialize.
		 *
		 * @param[in]  depth_bits  The depth bits
		 */
		void SetDepthBits(uint8_t depth_bits);

		/**
		 * @brief      Sets the stencil bits.
		 * @details    Overrides SCYTHE_DEFAULT_STENCIL_BITS value.
		 *             Should be called on Application::Initialize.
		 *
		 * @param[in]  stencil_bits  The stencil bits
		 */
		void SetStencilBits(uint8_t stencil_bits);

	protected:

		/**
		 * @brief      Initializes the graphics API.
		 *
		 * @return     True on success and False otherwise.
		 */
		virtual bool Initialize();

		/**
		 * @brief      Deinitializes the graphics API.
		 */
		virtual void Deinitialize();

		/**
		 * @brief      Begins a frame.
		 */
		virtual void BeginFrame();

		/**
		 * @brief      Ends a frame.
		 */
		virtual void EndFrame();
		
	protected:
		Application* application_;		//!< pointer to owner application
		bool initialized_;				//!< whether application been initalized
		uint8_t color_bits_;			//!< number of bits for color buffer
		uint8_t depth_bits_;			//!< number of bits for depth buffer
		uint8_t stencil_bits_;			//!< number of bits for stencil buffer
	};

} // namespace scythe

#endif