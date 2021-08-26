#ifndef __SCYTHE_RENDERER_H__
#define __SCYTHE_RENDERER_H__

#include "common/types.h"
#include "common/platform.h"
#include "math/matrix4.h"

#include "vertex_format.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "texture.h"
#include "context.h"
#include "shader.h"
#include "font.h"
#include "cubemap_fill_type.h"

#include <list>
#include <stack>

namespace scythe {

	const int kMaxImageUnit = 16;
	const int kMaxMrt = 4;

	//! Base renderer class
	class Renderer {
	public:
		Renderer(int w, int h);
		virtual ~Renderer();
		
		Context * context();
		int width();
		int height();
		float aspect_ratio();

		void UpdateSizes(int w, int h);

		void Defaults();
		void CleanUp();
		void CheckForUsing(); //!< user may do not detach texture or shader in the end of the frame, pure debug feature

		// Obtain memory consumption
		U32 GetUsedVideoMemorySize(); // textures + vbo + ibo
		U32 GetUsedTexturesSize();
		U32 GetUsedVertexBuffersSize();
		U32 GetUsedIndexBuffersSize();

		bool TakeScreenshot(const char* directory_name);

		// Texture functions
		bool AddTexture(Texture* &texture, const char* filename,
			Texture::Wrap wrap = Texture::Wrap::kRepeat,
			Texture::Filter filt = Texture::Filter::kTrilinear);
		void AddTextureFromImage(Texture* &texture, const Image& image,
			Texture::Wrap wrap = Texture::Wrap::kRepeat,
			Texture::Filter filt = Texture::Filter::kTrilinear);
		bool AddTextureCubemap(Texture* &texture, const char* filename, CubemapFillType fill_type, int desired_width);
		bool AddTextureCubemap(Texture* &texture, const char* filenames[6], bool use_mipmaps = false);
		bool CreateTextureNormalMapFromHeightMap(Texture* &texture, const char* filename,
			Texture::Wrap wrap = Texture::Wrap::kRepeat,
			Texture::Filter filt = Texture::Filter::kTrilinear);
		bool CreateTextureNormalHeightMapFromHeightMap(Texture* &texture, const char* filename,
			Texture::Wrap wrap = Texture::Wrap::kRepeat,
			Texture::Filter filt = Texture::Filter::kTrilinear);
		virtual void CreateTextureColor(Texture* &texture, float r, float g, float b, float a) = 0;
		virtual void CreateTextureCubemap(Texture* &texture, int w, int h, Image::Format fmt = Image::Format::kRGB8, Texture::Filter filt = Texture::Filter::kLinear) = 0;
		virtual void CreateTextureDepth(Texture* &texture, int w, int h, U32 depthSize) = 0;
		virtual void CreateTexture(Texture* &texture, int w, int h, Image::Format fmt) = 0;
		virtual void CreateTextureFromData(Texture* &texture, int w, int h, Image::Format fmt, unsigned char *data) = 0;
		virtual void AddRenderTarget(Texture* &texture, int w, int h, Image::Format fmt,
			Texture::Filter filt = Texture::Filter::kLinear, Texture::Wrap wrap = Texture::Wrap::kClampToEdge) = 0;
		virtual void AddRenderDepthStencil(Texture* &texture, int w, int h, U32 depthSize, U32 stencilSize) = 0;
		virtual void DeleteTexture(Texture* texture) = 0;
		virtual void ChangeTexture(Texture* texture, U32 layer = 0) = 0;
		virtual void ChangeRenderTargets(U8 nTargets, Texture* *colorRTs, Texture* depthRT) = 0;
		virtual void ChangeRenderTargetsToCube(U8 nTargets, Texture* *colorRTs, Texture* depthRT, int face, int level) = 0;
		void ChangeRenderTarget(Texture* colorRT, Texture* depthRT);
		virtual void GenerateMipmap(Texture* texture) = 0;
		virtual void CopyToTexture(Texture* texture, U32 layer = 0) = 0;
		virtual void SetTextureData(Texture* texture, int x, int y, int width, int height, const void* pixels) = 0;

		// Vertex format functions
		virtual void AddVertexFormat(VertexFormat* &vf, VertexAttribute *attribs, U32 nAttribs) = 0;
		virtual void ChangeVertexFormat(VertexFormat* vf) = 0;
		virtual void DeleteVertexFormat(VertexFormat* vf) = 0;

		// Vertex buffer functions
		virtual void AddVertexBuffer(VertexBuffer* &vb, U32 size, void *data, BufferUsage usage) = 0;
		virtual void DeleteVertexBuffer(VertexBuffer* vb) = 0;

		// Index buffer functions
		virtual void AddIndexBuffer(IndexBuffer* &ib, U32 nIndices, U32 indexSize, void *data, BufferUsage usage) = 0;
		virtual void DeleteIndexBuffer(IndexBuffer* ib) = 0;

		// Shader functions
		bool AddShader(Shader* &shader, const char* filename);
		virtual bool AddShader(Shader* &shader, const ShaderInfo& info) = 0;
		virtual void DeleteShader(Shader* shader) = 0;

		// Font functions
		virtual void AddFont(Font* &font, const char* fontname) = 0;
		virtual void AddOutlinedFont(Font* &font, const char* fontname, float border_size,
			const RgbColor& base_color, const RgbColor& border_color) = 0; //!< requires another shader
		virtual void DeleteFont(Font* font) = 0;

		virtual void ReadPixels(int w, int h, U8 *data) = 0; //!< reads pixels in R8G8B8 format

		virtual void ClearColor(float r, float g, float b, float a) = 0;
		virtual void ClearColorBuffer(void) = 0;
		virtual void ClearColorAndDepthBuffers(void) = 0;
		virtual void ClearDepthBuffer(void) = 0;
		virtual void ClearStencil(int value) = 0;
		virtual void ClearStencilBuffer() = 0;
		
		void SetViewport(int w, int h);
		
		// Matrices functions
		void SetProjectionMatrix(const Matrix4& mat);
		void SetViewMatrix(const Matrix4& mat);
		void SetModelMatrix(const Matrix4& mat);
		void PushMatrix();
		void PopMatrix();
		
		const Vector4& viewport() const;
		const Matrix4& projection_matrix() const;
		const Matrix4& view_matrix() const;
		const Matrix4& model_matrix() const;
		
		// Obsolete OpenGL analogs
		void LoadMatrix(const Matrix4& matrix);
		void MultMatrix(const Matrix4& matrix);
		void Translate(float x, float y, float z);
		void Translate(const Vector3& v);
		void Scale(float x, float y, float z);
		void Scale(float s);

		virtual void ChangeBlendFunc(U32 source, U32 dest) = 0;
		virtual void EnableBlend(void) = 0;
		virtual void DisableBlend(void) = 0;

		virtual void EnableDepthTest(void) = 0;
		virtual void DisableDepthTest(void) = 0;
		virtual void EnableDepthWrite(void) = 0;
		virtual void DisableDepthWrite(void) = 0;
		
		virtual void EnableStencilTest(void) = 0;
		virtual void DisableStencilTest(void) = 0;

		virtual void EnableWireframeMode(void) = 0;
		virtual void DisableWireframeMode(void) = 0;
		
		virtual void CullFace(CullFaceType mode) = 0;

	protected:
		virtual void ApiAddTexture(Texture* &tex, const Image &img, Texture::Wrap wrap, Texture::Filter filt) = 0;
		virtual void ApiAddTextureCubemap(Texture* &tex, Image *imgs, bool use_mipmaps = false) = 0;
		virtual void ApiDeleteTexture(Texture* tex) = 0;
		virtual void ApiViewport(int width, int height) = 0;
		
		Context * context_;

		int width_;								//!< owner app's window width
		int height_;							//!< owner app's window height
		float aspect_ratio_;					//!< w/h
		
		Vector4 viewport_;						//!< viewport
		Matrix4 projection_matrix_;				//!< projection matrix
		Matrix4 view_matrix_;					//!< view matrix
		Matrix4 model_matrix_;					//!< model matrix
		std::stack<Matrix4> matrices_stack_;	//!< matrices stack

		std::list<Texture*> textures_;
		std::list<Shader*> shaders_;
		std::list<Font*> fonts_;
		std::list<VertexFormat*> vertex_formats_;
		std::list<VertexBuffer*> vertex_buffers_;
		std::list<IndexBuffer*> index_buffers_;

		// Current identifiers
		Texture* current_textures_[kMaxImageUnit];
		VertexFormat* current_vertex_format_;   //!< obsolete
		Texture* current_color_rt_[kMaxMrt];
		Texture* current_depth_rt_;
	};

} // namespace scythe

#endif