#ifndef __SCYTHE_GRAPHICS_PROVIDER_H__
#define __SCYTHE_GRAPHICS_PROVIDER_H__

#include "types.h"
#include "non_copyable.h"

namespace scythe {

	/**
	 * @brief      This class describes a graphics provider.
	 * @details    Note that its a base class that should be created and 
	 *             initalized during Application::Initialize phase.
	 *             For example:
	 *             ```cpp
	 *             graphics_provider_ = new scythe::GraphicsProvider(this);
	 *             ```
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
		 * @brief      Gets the red bits.
		 *
		 * @return     The red bits.
		 */
		uint8_t GetRedBits() const;

		/**
		 * @brief      Gets the green bits.
		 *
		 * @return     The green bits.
		 */
		uint8_t GetGreenBits() const;

		/**
		 * @brief      Gets the blue bits.
		 *
		 * @return     The blue bits.
		 */
		uint8_t GetBlueBits() const;

		/**
		 * @brief      Gets the alpha bits.
		 *
		 * @return     The alpha bits.
		 */
		uint8_t GetAlphaBits() const;

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
		 * @brief      Sets the red bits.
		 * @details    Should be called on Application::Initialize.
		 *
		 * @param[in]  red_bits  The red bits
		 */
		void SetRedBits(uint8_t red_bits);

		/**
		 * @brief      Sets the green bits.
		 * @details    Should be called on Application::Initialize.
		 *
		 * @param[in]  green_bits  The green bits
		 */
		void SetGreenBits(uint8_t green_bits);

		/**
		 * @brief      Sets the blue bits.
		 * @details    Should be called on Application::Initialize.
		 *
		 * @param[in]  blue_bits  The blue bits
		 */
		void SetBlueBits(uint8_t blue_bits);

		/**
		 * @brief      Sets the alpha bits.
		 * @details    Should be called on Application::Initialize.
		 *
		 * @param[in]  alpha_bits  The alpha bits
		 */
		void SetAlphaBits(uint8_t alpha_bits);

		/**
		 * @brief      Sets the depth bits.
		 * @details    Should be called on Application::Initialize.
		 *
		 * @param[in]  depth_bits  The depth bits
		 */
		void SetDepthBits(uint8_t depth_bits);

		/**
		 * @brief      Sets the stencil bits.
		 * @details    Should be called on Application::Initialize.
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
		uint8_t red_bits_;				//!< number of bits for red channel in color buffer
		uint8_t green_bits_;			//!< number of bits for green channel in color buffer
		uint8_t blue_bits_;				//!< number of bits for blue channel in color buffer
		uint8_t alpha_bits_;			//!< number of bits for alpha channel in color buffer
		uint8_t depth_bits_;			//!< number of bits for depth buffer
		uint8_t stencil_bits_;			//!< number of bits for stencil buffer
	};

} // namespace scythe

#endif