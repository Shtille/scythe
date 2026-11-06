#ifndef __SCYTHE_RESOURCE_H__
#define __SCYTHE_RESOURCE_H__

namespace scythe {

	/**
	 * Resource identifier definition
	 */
	typedef int ResourceID;

	/**
	 * @brief      This class describes a resource.
	 * @details    Base graphics resource class for using in resource manager.
	 * @see        ResourceManager
	 */
	class Resource
	{
	public:
		virtual ~Resource() = default;
	};

} // namespace scythe

#endif