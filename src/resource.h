#ifndef __SCYTHE_RESOURCE_H__
#define __SCYTHE_RESOURCE_H__

namespace scythe {

	// Resource identifier definition
	typedef int ResourceID;

	//! Base graphics resource class for using in resource manager
	class Resource {
	public:
		virtual ~Resource() = default;
	};

} // namespace scythe

#endif