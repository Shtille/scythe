#include <scythe/graphics_provider.h>

namespace scythe {

	static constexpr uint8_t kDefaultRedBits = 8;
	static constexpr uint8_t kDefaultGreenBits = 8;
	static constexpr uint8_t kDefaultBlueBits = 8;
	static constexpr uint8_t kDefaultAlphaBits = 8;
	static constexpr uint8_t kDefaultDepthBits = 24;
	static constexpr uint8_t kDefaultStencilBits = 8;

	GraphicsProvider::GraphicsProvider(Application* application)
	: application_(application)
	, initialized_(false)
	, red_bits_(kDefaultRedBits)
	, green_bits_(kDefaultGreenBits)
	, blue_bits_(kDefaultBlueBits)
	, alpha_bits_(kDefaultAlphaBits)
	, depth_bits_(kDefaultDepthBits)
	, stencil_bits_(kDefaultStencilBits)
	{
	}
	bool GraphicsProvider::IsInitialized() const
	{
		return initialized_;
	}
	uint8_t GraphicsProvider::GetRedBits() const
	{
		return red_bits_;
	}
	uint8_t GraphicsProvider::GetGreenBits() const
	{
		return green_bits_;
	}
	uint8_t GraphicsProvider::GetBlueBits() const
	{
		return blue_bits_;
	}
	uint8_t GraphicsProvider::GetAlphaBits() const
	{
		return alpha_bits_;
	}
	uint8_t GraphicsProvider::GetDepthBits() const
	{
		return depth_bits_;
	}
	uint8_t GraphicsProvider::GetStencilBits() const
	{
		return stencil_bits_;
	}
	void GraphicsProvider::SetRedBits(uint8_t red_bits)
	{
		if (initialized_)
			return;
		red_bits_ = red_bits;
	}
	void GraphicsProvider::SetGreenBits(uint8_t green_bits)
	{
		if (initialized_)
			return;
		green_bits_ = green_bits;
	}
	void GraphicsProvider::SetBlueBits(uint8_t blue_bits)
	{
		if (initialized_)
			return;
		blue_bits_ = blue_bits;
	}
	void GraphicsProvider::SetAlphaBits(uint8_t alpha_bits)
	{
		if (initialized_)
			return;
		alpha_bits_ = alpha_bits;
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