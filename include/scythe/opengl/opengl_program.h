#ifndef __SCYTHE_OPENGL_PROGRAM_H__
#define __SCYTHE_OPENGL_PROGRAM_H__

#ifndef SCYTHE_USE_OPENGL
# error "OpenGL should be enabled to use this header"
#endif

#include <cstddef>

#include "opengl_include.h"

namespace scythe {

	/**
	 * @brief      This class describes an OpenGL program.
	 * @details    Should be created and initalized during Application::Initialize phase.
	 */
	class OpenGLProgram final
	{
	public:

		/**
		 * @brief      Describes render shader sources information.
		 */
		struct RenderShadersInfo
		{
			const char* vertex = nullptr;
			const char* fragment = nullptr;
			const char* geometry = nullptr;
		};

		/**
		 * @brief      Describes compute shader sources information.
		 */
		struct ComputeShadersInfo
		{
			const char* compute = nullptr;
		};

		/**
		 * @brief      Default constructor.
		 */
		OpenGLProgram() = default;

	public:

		/**
		 * @brief      Creates render program from files.
		 *
		 * @param[in]  info       Shader source information.
		 * @param[in]  use_files  Whether use files or sources.
		 *
		 * @return     True on success and false otherwise.
		 */
		bool Create(const RenderShadersInfo& info, bool use_files);

		/**
		 * @brief      Creates compute program from files.
		 *
		 * @param[in]  info       Shader source information.
		 * @param[in]  use_files  Whether use files or sources.
		 *
		 * @return     True on success and false otherwise.
		 */
		bool Create(const ComputeShadersInfo& info, bool use_files);

		/**
		 * @brief      Destroys the object.
		 */
		void Destroy();

		/**
		 * @brief      Gets program ID.
		 *
		 * @return     The program ID.
		 */
		GLuint id() const;

	private:
		static constexpr std::size_t kNumShaders = 3u;
		GLuint id_ = 0u;
	};

} // namespace scythe

#endif