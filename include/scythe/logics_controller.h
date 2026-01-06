#ifndef __SCYTHE_LOGICS_CONTROLLER_H__
#define __SCYTHE_LOGICS_CONTROLLER_H__

namespace scythe {

	/**
	 * @brief      Controls how application logics is going to be updated.
	 */
	class LogicsController
	{
	public:
		virtual ~LogicsController() = default;

		virtual void UpdateLogics() = 0;
	};

} // namespace scythe

#endif