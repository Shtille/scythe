#ifndef __SCYTHE_RESOURCE_MANAGER_H__
#define __SCYTHE_RESOURCE_MANAGER_H__

#include "common/singleton.h"
#include "resource.h"
#include "string_id.h"

#include <unordered_map>

namespace scythe {

	typedef Resource * (* ResourceLoadingFunc)(void * /*user_data*/, ResourceID);
	typedef void (* ResourceUnloadingFunc)(void * /*user_data*/, Resource * /*resource*/);

	class ResourceManager : public ManagedSingleton<ResourceManager> {
	public:
		ResourceManager();
		virtual ~ResourceManager();

		void CleanUp(); //!< releases all the resources

		ResourceID RegisterResource(StringID string_id, void* user_data, ResourceLoadingFunc loading_func, ResourceUnloadingFunc unloading_func);
		void UnregisterResource(ResourceID id);

		ResourceID GetResourceIdByName(StringID string_id);
		Resource * GetResource(ResourceID id);

		void RequestLoad(ResourceID id);
		void RequestUnload(ResourceID id);
		void Perform();
		bool PerformStep();
		int GetResourcesCountToProcess();

	private:
		struct ResourceInfo {
			int counter;
			StringID string_id;
			Resource * resource;
			void * user_data;
			ResourceLoadingFunc loading_func;
			ResourceUnloadingFunc unloading_func;
		};
		typedef std::unordered_map<ResourceID, ResourceInfo> Container;
		Container container_;
		ResourceID current_resource_id_;
	};

} // namespace scythe

#endif