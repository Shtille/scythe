#ifndef __SCYTHE_SERVICE_TASK_INTERFACE_H__
#define __SCYTHE_SERVICE_TASK_INTERFACE_H__

namespace scythe {

	//! Service task class interface
	class ServiceTaskInterface {
	public:
		ServiceTaskInterface() = default;
		virtual ~ServiceTaskInterface() = default;

		virtual bool Execute() = 0;
		virtual void Notify(bool success) = 0;

	private:
		ServiceTaskInterface(const ServiceTaskInterface&) = delete;
		ServiceTaskInterface& operator =(const ServiceTaskInterface&) = delete;
	};

} // namespace scythe

#endif