#include <scythe/graphics_provider.h>

namespace scythe {

GraphicsProvider::GraphicsProvider(const GraphicsCreationParams& params)
: color_bits_(params.color_bits)
, depth_bits_(params.depth_bits)
, stencil_bits_(params.stencil_bits)
, pad(0)
{
}

} // namespace scythe