#ifndef __SCYTHE_RENDERABLE_H__
#define __SCYTHE_RENDERABLE_H__

namespace scythe {

	//! Renderable class interface
	class IRenderable {
	public:
		virtual ~IRenderable() = default;
		
		virtual void Render() = 0;
	};

} // namespace scythe

#endif