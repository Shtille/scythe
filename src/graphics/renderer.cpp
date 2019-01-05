#include "renderer.h"

#include "cubemap_face_filler.h"
#include "filesystem/directory.h"

#include <ctime>

namespace scythe {

	Renderer::Renderer(int w, int h)
	{
		UpdateSizes(w, h);
		
		projection_matrix_.SetIdentity();
		view_matrix_.SetIdentity();
		model_matrix_.SetIdentity();
		
		Defaults();
	}
	Renderer::~Renderer()
	{
		// Don't call any functions with virtual table here!
	}
	Context * Renderer::context()
	{
		return context_;
	}
	int Renderer::width()
	{
		return width_;
	}
	int Renderer::height()
	{
		return height_;
	}
	float Renderer::aspect_ratio()
	{
		return aspect_ratio_;
	}
	void Renderer::UpdateSizes(int w, int h)
	{
		width_ = w;
		height_ = h;
		aspect_ratio_ = (float)w / (float)h;
	}
	void Renderer::Defaults(void)
	{
		for (int i = 0; i < kMaxImageUnit; ++i)
			current_textures_[i] = nullptr;
		current_shader_ = nullptr;
		current_vertex_format_ = nullptr;
		active_vertex_format_ = nullptr;
		current_vertex_buffer_ = nullptr;
		current_index_buffer_ = nullptr;

		for (int i = 0; i < kMaxMrt; ++i)
			current_color_rt_[i] = nullptr;
		current_depth_rt_ = nullptr;
	}
	void Renderer::CleanUp(void)
	{
		// Clean up textures
		for (auto &obj : textures_)
		{
			ApiDeleteTexture(obj);
			delete obj;
		}
		textures_.clear();

		// Clean up shaders
		for (auto &obj : shaders_)
		{
			delete obj;
		}
		shaders_.clear();

		// Clean up fonts
		for (auto &obj : fonts_)
		{
			delete obj;
		}
		fonts_.clear();

		// Clean up vertex formats
		for (auto &obj : vertex_formats_)
		{
			delete obj.pointer();
		}
		vertex_formats_.clear();

		// Clean up vertex buffers
		for (auto &obj : vertex_buffers_)
		{
			delete obj;
		}
		vertex_buffers_.clear();

		// Clean up index buffers
		for (auto &obj : index_buffers_)
		{
			delete obj;
		}
		index_buffers_.clear();
	}
	void Renderer::CheckForUsing(void)
	{
		assert(current_shader_ == nullptr);
		for (int i = 0; i < kMaxImageUnit; ++i)
			assert(current_textures_[i] == nullptr);
	}
	U32 Renderer::GetUsedVideoMemorySize(void)
	{
		U32 s = GetUsedTexturesSize();
		s += GetUsedVertexBuffersSize();
		s += GetUsedIndexBuffersSize();
		return s;
	}
	U32 Renderer::GetUsedTexturesSize(void)
	{
		U32 s = 0;
		for (auto &t : textures_)
			s += t->GetSize();
		return s;
	}
	U32 Renderer::GetUsedVertexBuffersSize(void)
	{
		U32 s = 0;
		for (auto &vb : vertex_buffers_)
			s += vb->GetSize();
		return s;
	}
	U32 Renderer::GetUsedIndexBuffersSize(void)
	{
		U32 s = 0;
		for (auto &ib : index_buffers_)
			s += ib->GetSize();
		return s;
	}
	bool Renderer::TakeScreenshot(const char* directory_name)
	{
		// Fill our path string
		char filename[50];
		time_t now = time(nullptr);
		strftime(filename, _countof(filename), "SS.%Y.%m.%d.%H.%M.%S.jpg", localtime(&now));
		char delimeter[2] = { GetPathDelimeter(), '\0' };
		size_t size = strlen(directory_name) + strlen(filename) + 2; // 1 is for delimeter, another 1 is for \0
		char *full_filename = new char[size];
		strcpy(full_filename, directory_name);
		strcat(full_filename, delimeter);
		strcat(full_filename, filename);

		CreateDirectory(directory_name); // create directory if it doesn't exist

		Image image;
		// allocate memory and read pixels
		U8 *data = image.Allocate(width_, height_, Image::Format::kRGB8);
		ReadPixels(width_, height_, data);
		if (!image.Save(full_filename))
		{
			delete[] full_filename;
			return false;
		}

		delete[] full_filename;
		return true;
	}
	bool Renderer::AddTexture(Texture* &texture, const char* filename, Texture::Wrap wrap, Texture::Filter filt)
	{
		Image image;
		if (image.LoadFromFile(filename))
			ApiAddTexture(texture, image, wrap, filt);
		return texture != nullptr;
	}
	void Renderer::AddTextureFromImage(Texture* &texture, const Image& image, Texture::Wrap wrap, Texture::Filter filt)
	{
		ApiAddTexture(texture, image, wrap, filt);
	}
	bool Renderer::AddTextureCubemap(Texture* &texture, const char* filename, CubemapFillType fill_type, int desired_width)
	{
		texture = nullptr;
		Image base_image;
		if (!base_image.LoadFromFile(filename))
			return false;

		CubemapFaceFiller * face_filler = nullptr;
		switch (fill_type)
		{
		case CubemapFillType::kCross:
			face_filler = new CrossCubemapFaceFiller(&base_image);
			break;
		case CubemapFillType::kSphere:
			face_filler = new SphereCubemapFaceFiller(&base_image, desired_width);
			break;
		}
		if (!face_filler)
			return false;

		bool succeed = true;
		Image *images = new Image[6];
		for (int face = 0; face < 6; ++face)
		{
			if (!face_filler->Fill(face, images + face))
			{
				succeed = false;
				break;
			}
		}
		if (succeed)
			ApiAddTextureCubemap(texture, images);
		delete[] images;
		delete face_filler;
		return succeed;
	}
	bool Renderer::AddTextureCubemap(Texture* &texture, const char* filenames[6], bool use_mipmaps)
	{
		texture = nullptr;
		bool succeed = true;
		Image *images = new Image[6];
		for (int face = 0; face < 6; ++face)
		{
			images[face].SetRowOrder(false);
			if (!images[face].LoadFromFile(filenames[face]))
			{
				succeed = false;
				break;
			}
		}
		if (succeed)
			ApiAddTextureCubemap(texture, images, use_mipmaps);
		delete[] images;
		return succeed;
	}
	bool Renderer::CreateTextureNormalMapFromHeightMap(Texture* &texture, const char* filename, Texture::Wrap wrap, Texture::Filter filt)
	{
		Image image;
		if (image.LoadNMapFromHMap(filename))
			ApiAddTexture(texture, image, wrap, filt);
		return texture != nullptr;
	}
	bool Renderer::CreateTextureNormalHeightMapFromHeightMap(Texture* &texture, const char* filename, Texture::Wrap wrap, Texture::Filter filt)
	{
		Image image;
		if (image.LoadNHMapFromHMap(filename))
			ApiAddTexture(texture, image, wrap, filt);
		return texture != nullptr;
	}
	void Renderer::ChangeRenderTarget(Texture* colorRT, Texture* depthRT)
	{
		ChangeRenderTargets(1, &colorRT, depthRT);
	}
	void Renderer::SetViewport(int w, int h)
	{
		viewport_.Set(0.0f, 0.0f, (float)w, (float)h);
		ApiViewport(w, h);
	}
	void Renderer::SetProjectionMatrix(const Matrix4& mat)
	{
		projection_matrix_ = mat;
	}
	void Renderer::SetViewMatrix(const Matrix4& mat)
	{
		view_matrix_ = mat;
	}
	void Renderer::SetModelMatrix(const Matrix4& mat)
	{
		model_matrix_ = mat;
	}
	void Renderer::PushMatrix()
	{
		matrices_stack_.push(model_matrix_);
	}
	void Renderer::PopMatrix()
	{
		assert(!matrices_stack_.empty());
		model_matrix_ = matrices_stack_.top();
		matrices_stack_.pop();
	}
	const Vector4& Renderer::viewport() const
	{
		return viewport_;
	}
	const Matrix4& Renderer::projection_matrix() const
	{
		return projection_matrix_;
	}
	const Matrix4& Renderer::view_matrix() const
	{
		return view_matrix_;
	}
	const Matrix4& Renderer::model_matrix() const
	{
		return model_matrix_;
	}
	void Renderer::LoadMatrix(const Matrix4& matrix)
	{
		model_matrix_ = matrix;
	}
	void Renderer::MultMatrix(const Matrix4& matrix)
	{
		model_matrix_ *= matrix;
	}
	void Renderer::Translate(float x, float y, float z)
	{
		Matrix4 translation;
		Matrix4::CreateTranslation(x, y, z, &translation);
		model_matrix_ *= translation;
	}
	void Renderer::Translate(const Vector3& v)
	{
		Matrix4 translation;
		Matrix4::CreateTranslation(v, &translation);
		model_matrix_ *= translation;
	}
	void Renderer::Scale(float x, float y, float z)
	{
		Matrix4 scale;
		Matrix4::CreateScale(x, y, z, &scale);
		model_matrix_ *= scale;
	}
	void Renderer::Scale(float s)
	{
		Matrix4 scale;
		Matrix4::CreateScale(s, s, s, &scale);
		model_matrix_ *= scale;
	}

} // namespace scythe