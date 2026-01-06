#ifndef __SCYTHE_GRAPHICS_PROVIDER_H__
#define __SCYTHE_GRAPHICS_PROVIDER_H__

#include "types.h"

namespace scythe {

	struct GraphicsCreationParams
	{
		uint8_t color_bits;
		uint8_t depth_bits;
		uint8_t stencil_bits;
		uint8_t pad;
	};

	/**
	 * @brief      This class describes a graphics provider.
	 */
	class GraphicsProvider
	{
		friend class Application;

	public:
		
		explicit GraphicsProvider(const GraphicsCreationParams& params);
		virtual ~GraphicsProvider() = default;

	protected:

		virtual bool Initialize() = 0;
		virtual void Deinitialize() = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		
		uint8_t color_bits_;			//!< number of bits for color buffer
		uint8_t depth_bits_;			//!< number of bits for depth buffer
		uint8_t stencil_bits_;			//!< number of bits for stencil buffer
		uint8_t pad;
	};

} // namespace scythe

#endif