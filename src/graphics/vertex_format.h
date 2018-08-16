#ifndef __SCYTHE_VERTEX_FORMAT_H__
#define __SCYTHE_VERTEX_FORMAT_H__

#include "common/types.h"

namespace scythe {

	const int kMaxGeneric = 16;
	const int kMaxTexcoord = 8;

	//! Vertex attribute struct
	struct VertexAttribute {

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
		
		VertexAttribute(Type type, U32 size)
		: type(type)
		, size(size)
		{
		}

		Type type;			//!< Specifies the vertex type.
		U32 size;			//!< Specifies the vertex format size.
	};

	//! Vertex format class
	class VertexFormat {
		friend class Renderer;
		friend class OpenGlRenderer;
		
		friend class Model;
		friend class Text;

	public:
		struct Attrib {
			int offset;
			int size;
		};
		
		bool operator == (const VertexFormat& vf);
		
		U32 vertex_size() const;
		const Attrib& generic(U32 index) const;

	protected:
		VertexFormat();
		virtual ~VertexFormat();
		VertexFormat(const VertexFormat&) = delete;
		void operator = (const VertexFormat&) = delete;

		void Fill(VertexAttribute *attribs, U32 num_attribs);

	private:
		Attrib generic_[kMaxGeneric];

		U32 vertex_size_;
		U8 max_generic_;
	};

} // namespace scythe

#endif