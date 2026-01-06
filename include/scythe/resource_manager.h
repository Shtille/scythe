#ifndef __SCYTHE_RESOURCE_MANAGER_H__
#define __SCYTHE_RESOURCE_MANAGER_H__

#include <unordered_map>

#include "resource.h"
#include "singleton.h"
#include "string_id.h"

namespace scythe {

	/**
	 * Resource loading function definition
	 */
	typedef Resource* (* ResourceLoadingFunc)(void* /*user_data*/, ResourceID /*resource_id*/);

	/**
	 * Resource unloading function definition
	 */
	typedef void (* ResourceUnloadingFunc)(void* /*user_data*/, Resource* /*resource*/);

	/**
	 * @brief      This class describes a resource manager.
	 * @details    Creates and manages @ref Resource class instances.
	 * @see        Resource
	 */
	class ResourceManager final
	: private ManagedSingleton<ResourceManager>
	{
		friend class ManagedSingleton<ResourceManager>;
		friend class Application;

	public:

		/**
		 * Make GetInstance be accessible.
		 */
		using ManagedSingleton<ResourceManager>::GetInstance;

		/**
		 * @brief      Cleans all the resources
		 */
		void CleanUp();

		/**
		 * @brief      Registers a new resource
		 *
		 * @param[in]  string_id       The string identifier
		 * @param      user_data       The user data
		 * @param[in]  loading_func    The loading function
		 * @param[in]  unloading_func  The unloading function
		 *
		 * @return     The created resource id.
		 */
		ResourceID RegisterResource(StringID string_id, void* user_data, ResourceLoadingFunc loading_func, ResourceUnloadingFunc unloading_func);

		/**
		 * @brief      Unregisters previously registered resource
		 *
		 * @param[in]  id    The resource identifier
		 */
		void UnregisterResource(ResourceID id);

		/**
		 * @brief      Gets the resource identifier by name.
		 *
		 * @param[in]  string_id  The string identifier
		 *
		 * @return     The resource identifier by name.
		 */
		ResourceID GetResourceIdByName(StringID string_id);

		/**
		 * @brief      Gets the resource by its identifier.
		 *
		 * @param[in]  id    The identifier
		 *
		 * @return     The resource.
		 */
		Resource* GetResource(ResourceID id);

		/**
		 * @brief      Requests loading of a resource with the specified identifier
		 *
		 * @param[in]  id    The identifier
		 */
		void RequestLoad(ResourceID id);

		/**
		 * @brief      Requests unloading of a resource with the specified identifier
		 *
		 * @param[in]  id    The identifier
		 */
		void RequestUnload(ResourceID id);

		/**
		 * @brief      Performs loading/unloading routine of all registered resources.
		 */
		void Perform();

		/**
		 * @brief      Performs a single step of loading/unloading routine of all registered resources.
		 *
		 * @return     True if loading is finished and false otherwise.
		 */
		bool PerformStep();

		/**
		 * @brief      Gets the resources count to process.
		 *
		 * @return     Number of resources that will be processed.
		 */
		int GetResourcesCountToProcess();

	private:

		struct ResourceInfo
		{
			int counter;
			StringID string_id;
			Resource* resource;
			void* user_data;
			ResourceLoadingFunc loading_func;
			ResourceUnloadingFunc unloading_func;
		};
		typedef std::unordered_map<ResourceID, ResourceInfo> Container;

		ResourceManager();
		~ResourceManager();

		Container container_;
		ResourceID unique_resource_id_;
	};

} // namespace scythe

#endif