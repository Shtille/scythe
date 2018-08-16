#ifndef __SCYTHE_CUBEMAP_FACE_FILLER_H__
#define __SCYTHE_CUBEMAP_FACE_FILLER_H__

namespace scythe {

	// Forward declarations
	class Image;

	//! Cubemap face filler base class
	class CubemapFaceFiller {
	public:
		CubemapFaceFiller(Image * source_image);
		virtual ~CubemapFaceFiller();

		virtual bool Fill(int face, Image * image) = 0;

	protected:
		Image * source_image_;
	};

	//! Cross cubemap face filler
	class CrossCubemapFaceFiller : public CubemapFaceFiller {
	public:
		CrossCubemapFaceFiller(Image * source_image);

		bool Fill(int face, Image * image);
	};

	//! Sphere cubemap face filler
	class SphereCubemapFaceFiller : public CubemapFaceFiller {
	public:
		SphereCubemapFaceFiller(Image * source_image, int face_width);

		bool Fill(int face, Image * image);

	private:
		int face_width_;
	};

} // namespace scythe

#endif