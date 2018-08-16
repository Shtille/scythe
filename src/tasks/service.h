#ifndef __SCYTHE_SERVICE_H__
#define __SCYTHE_SERVICE_H__

#include "common/non_copyable.h"

#include <mutex>
#include <condition_variable>
#include <thread>
#include <list>

namespace scythe {

	class ServiceTaskInterface;

	//! Service class
	class Service : public NonCopyable {
	public:
		Service();
		virtual ~Service();

		void RunService();
		void StopService();

		void ClearTasks();
		void AddTask(ServiceTaskInterface * task);

	protected:
		virtual void ThreadFunc();

		std::mutex mutex_;
		std::condition_variable condition_variable_;
		std::thread * thread_;
		std::list<ServiceTaskInterface*> tasks_;
		bool finishing_;

	private:

		static void ThreadFuncWrapper(Service * instance);
	};

} // namespace scythe

#endif