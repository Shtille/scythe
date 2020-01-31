#ifndef __SCYTHE_UI_DRAWABLE_H__
#define __SCYTHE_UI_DRAWABLE_H__

#include "renderable.h"

#include "graphics/renderer.h"

#include <vector>

namespace scythe {

	// Forward declarations
	class Widget;

	//! Drawable class
	class UiDrawable : public virtual IRenderable {
	public:
		Renderer * renderer();
		
		virtual void Render() override;

	protected:
		UiDrawable(Renderer * renderer, Shader * shader, Texture * texture);
		virtual ~UiDrawable();
		
		void MakeRenderable();
		
		virtual void BindConstUniforms() = 0;
		virtual void FillVertexAttribs() = 0;
		virtual void FillVertices() = 0;
		
		Renderer * renderer_;
		Shader * shader_;
		Texture * texture_;
		VertexFormat * vertex_format_;
		VertexBuffer * vertex_buffer_;
		U32 vertex_array_object_;
		
		U32 num_vertices_;
		U8 * vertices_array_;
		
		std::vector<VertexAttribute> attribs_;

	private:
		void FreeArrays();
		
		bool can_render_;
	};

} // namespace scythe

#endif