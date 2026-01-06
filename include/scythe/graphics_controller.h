#ifndef __SCYTHE_GRAPHICS_CONTROLLER_H__
#define __SCYTHE_GRAPHICS_CONTROLLER_H__

namespace scythe {

	/**
	 * @brief      Controls how graphics objects are going to be rendered.
	 */
	class GraphicsController
	{
	public:
		virtual ~GraphicsController() = default;

		/**
		 * @brief      Loads graphics resources.
		 *
		 * @return     True on success and false otherwise.
		 */
		virtual bool LoadGraphicsResources() = 0;

		/**
		 * @brief      Unloads graphics resources.
		 */
		virtual void UnloadGraphicsResources() = 0;

		/**
		 * @brief      Renders all the graphics objects.
		 */
		virtual void Render() = 0;
	};

} // namespace scythe

#endif