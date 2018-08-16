#ifndef __SCYTHE_DRAWABLE_H__
#define __SCYTHE_DRAWABLE_H__

#include "renderable.h"

#include "renderer/renderer.h"

#include <vector>

namespace scythe {

	// Forward declarations
	class Widget;

	//! Drawable class
	class Drawable : public virtual IRenderable {
	public:
		sht::graphics::Renderer * renderer();
		
		virtual void Render() override;

	protected:
		Drawable(sht::graphics::Renderer * renderer, sht::graphics::Shader * shader, sht::graphics::Texture * texture);
		virtual ~Drawable();
		
		void MakeRenderable();
		
		virtual void BindConstUniforms() = 0;
		virtual void FillVertexAttribs() = 0;
		virtual void FillVertices() = 0;
		
		sht::graphics::Renderer * renderer_;
		sht::graphics::Shader * shader_;
		sht::graphics::Texture * texture_;
		sht::graphics::VertexFormat * vertex_format_;
		sht::graphics::VertexBuffer * vertex_buffer_;
		U32 vertex_array_object_;
		
		U32 num_vertices_;
		U8 * vertices_array_;
		
		std::vector<sht::graphics::VertexAttribute> attribs_;

	private:
		void FreeArrays();
		
		bool can_render_;
	};

} // namespace scythe

#endif