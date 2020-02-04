#ifndef __SCYTHE_VERTEX_FORMAT_H__
#define __SCYTHE_VERTEX_FORMAT_H__

#include "common/types.h"
#include "common/ref.h"

namespace scythe {

	const int kMaxGeneric = 8;

	//! Vertex attribute struct
	class VertexAttribute {
		friend class VertexFormat;
	public:
		//! Vertex attrib types
		enum Type {
			kGeneric,
			kVertex,
			kNormal,
			kTexcoord,
			kColor,
			kTangent,
			kBinormal
		};
		
		VertexAttribute(Type type, U32 size);

		bool operator ==(const VertexAttribute& other);
		bool operator !=(const VertexAttribute& other);

		Type type;			//!< Specifies the vertex type.
		U32 size;			//!< Specifies the vertex format size.

	private:
		VertexAttribute() = default;
	};

	/**
	 * Vertex format class
	 */
	class VertexFormat final : public Ref {
		friend class Ref;
		friend class Renderer;
		friend class OpenGlRenderer;

	public:
		struct Attrib {
			int offset;
			int size;
		};
		
		bool operator == (const VertexFormat& vf);
		
		U32 vertex_size() const;
		const Attrib& generic(U32 index) const;
		const VertexAttribute * attributes() const;
		U32 num_attributes() const;

	protected:
		VertexFormat();
		~VertexFormat();
		VertexFormat(const VertexFormat&) = delete;
		void operator = (const VertexFormat&) = delete;

		bool IsSame(VertexAttribute *attribs, U32 num_attribs);
		void Fill(VertexAttribute *attribs, U32 num_attribs);
		
	private:
		VertexAttribute * attributes_;
		U32 num_attributes_;

		Attrib generic_[kMaxGeneric];

		U32 vertex_size_;
		U8 max_generic_;
	};

} // namespace scythe

#endif