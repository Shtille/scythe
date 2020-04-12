#ifndef __SCYTHE_OPENGL_RENDERER_H__
#define __SCYTHE_OPENGL_RENDERER_H__

#include "../renderer.h"

namespace scythe {

	class OpenGlRenderer final : public Renderer {
	public:
		OpenGlRenderer(int w, int h);
		virtual ~OpenGlRenderer();

		void CreateTextureColor(Texture* &texture, float r, float g, float b, float a);
		void CreateTextureCubemap(Texture* &texture, int w, int h, Image::Format fmt = Image::Format::kRGB8, Texture::Filter filt = Texture::Filter::kLinear);
		void CreateTextureDepth(Texture* &texture, int w, int h, U32 depthSize);
		void CreateTexture(Texture* &texture, int w, int h, Image::Format fmt);
		void CreateTextureFromData(Texture* &texture, int w, int h, Image::Format fmt, unsigned char *data);
		void AddRenderTarget(Texture* &texture, int w, int h, Image::Format fmt,
			Texture::Filter filt = Texture::Filter::kLinear, Texture::Wrap wrap = Texture::Wrap::kClampToEdge);
		void AddRenderDepthStencil(Texture* &texture, int w, int h, U32 depthSize, U32 stencilSize);
		void DeleteTexture(Texture* texture);
		void ChangeTexture(Texture* texture, U32 layer = 0);
		void ChangeRenderTargets(U8 nTargets, Texture* *colorRTs, Texture* depthRT);
		void ChangeRenderTargetsToCube(U8 nTargets, Texture* *colorRTs, Texture* depthRT, int face, int level);
		void GenerateMipmap(Texture* texture);
		void CopyToTexture(Texture* texture, U32 layer = 0);

		void AddVertexFormat(VertexFormat* &vf, VertexAttribute *attribs, U32 nAttribs);
		void ChangeVertexFormat(VertexFormat* vf);
		void DeleteVertexFormat(VertexFormat* vf);

		void AddVertexBuffer(VertexBuffer* &vb, U32 size, void *data, BufferUsage usage);
		void DeleteVertexBuffer(VertexBuffer* vb);

		void AddIndexBuffer(IndexBuffer* &ib, U32 nIndices, U32 indexSize, void *data, BufferUsage usage);
		void DeleteIndexBuffer(IndexBuffer* ib);

		bool AddShader(Shader* &shader, const ShaderInfo& info);
		void DeleteShader(Shader* shader);

		void AddFont(Font* &font, const char* fontname);
		void AddOutlinedFont(Font* &font, const char* fontname, float border_size,
			const RgbColor& base_color, const RgbColor& border_color);
		void DeleteFont(Font* font);

		void ReadPixels(int w, int h, U8 *data);

		void ClearColor(float r, float g, float b, float a);
		void ClearColorBuffer(void);
		void ClearColorAndDepthBuffers(void);
		void ClearDepthBuffer(void);
		void ClearStencil(int value);
		void ClearStencilBuffer();

		void ChangeBlendFunc(U32 source, U32 dest);
		void EnableBlend(void);
		void DisableBlend(void);

		void EnableDepthTest(void);
		void DisableDepthTest(void);
		void EnableDepthWrite(void);
		void DisableDepthWrite(void);
		
		void EnableStencilTest(void);
		void DisableStencilTest(void);

		void EnableWireframeMode(void);
		void DisableWireframeMode(void);
		
		void CullFace(CullFaceType mode);

	private:
		void SetDefaultStates();
		void ApiAddTexture(Texture* &tex, const Image &img, Texture::Wrap wrap, Texture::Filter filt);
		void ApiAddTextureCubemap(Texture* &tex, Image *imgs, bool use_mipmaps = false);
		void ApiDeleteTexture(Texture* tex);
		void ApiViewport(int width, int height);
		void ChangeImageUnit(U32 unit);

		U32 framebuffer_;				//!< OpenGL framebuffer object
		U32 current_image_unit_;		//!< current image unit
		U32 current_render_targets_;	//!< current render targets count
	};

} // namespace scythe

#endif