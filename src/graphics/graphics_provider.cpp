#include <scythe/graphics_provider.h>

namespace scythe {

	GraphicsProvider::GraphicsProvider(Application* application)
	: application_(application)
	, initialized_(false)
	, color_bits_(SCYTHE_DEFAULT_COLOR_BITS)
	, depth_bits_(SCYTHE_DEFAULT_DEPTH_BITS)
	, stencil_bits_(SCYTHE_DEFAULT_STENCIL_BITS)
	{
	}
	bool GraphicsProvider::IsInitialized() const
	{
		return initialized_;
	}
	uint8_t GraphicsProvider::GetColorBits() const
	{
		return color_bits_;
	}
	uint8_t GraphicsProvider::GetDepthBits() const
	{
		return depth_bits_;
	}
	uint8_t GraphicsProvider::GetStencilBits() const
	{
		return stencil_bits_;
	}
	void GraphicsProvider::SetColorBits(uint8_t color_bits)
	{
		if (initialized_)
			return;
		color_bits_ = color_bits;
	}
	void GraphicsProvider::SetDepthBits(uint8_t depth_bits)
	{
		if (initialized_)
			return;
		depth_bits_ = depth_bits;
	}
	void GraphicsProvider::SetStencilBits(uint8_t stencil_bits)
	{
		if (initialized_)
			return;
		stencil_bits_ = stencil_bits;
	}
	bool GraphicsProvider::Initialize()
	{
		initialized_ = true;
		return true;
	}
	void GraphicsProvider::Deinitialize()
	{
		initialized_ = false;
	}
	void GraphicsProvider::BeginFrame()
	{
	}
	void GraphicsProvider::EndFrame()
	{
	}

} // namespace scythe