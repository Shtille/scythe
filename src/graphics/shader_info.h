#ifndef __SCYTHE_SHADER_INFO_H__
#define __SCYTHE_SHADER_INFO_H__

#include "common/types.h"

namespace scythe {

	/**
	 * Defines shader information class
	 */
	class ShaderInfo {
		friend class Shader;
	public:

		/**
		 * Constructor
		 *
		 * @param base_filename The base file name without extention.
		 */
		ShaderInfo(const char* base_filename);

		/**
		 * Constructor
		 *
		 * @param base_filename 	The base file name without extention. If not null rest filenames are ignored.
		 * @param vertex_program 	The vertex program full file name.
		 * @param fragment_program 	The fragment program full file name.
		 * @param attributes 		The array of attributes.
		 * @param num_attributes 	The number of attributes.
		 * @param defines 			The array of defines.
		 * @param num_defines 		The number of defines.
		 */
		ShaderInfo(const char* base_filename, const char* vertex_program, const char* fragment_program,
			const char** attributes, U32 num_attributes, const char** defines, U32 num_defines);

		/**
		 * Destructor
		 */
		~ShaderInfo() = default;
		
	private:
		ShaderInfo(const ShaderInfo&) = delete;
		void operator = (const ShaderInfo&) = delete;

		const char* base_filename_;
		const char* vertex_program_;
		const char* fragment_program_;

		const char** attributes_;
		U32 num_attributes_;

		const char** defines_;
		U32 num_defines_;
	};

} // namespace scythe

#endif