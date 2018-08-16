#ifndef __SCYTHE_EVENT_LISTENER_H__
#define __SCYTHE_EVENT_LISTENER_H__

namespace scythe {

	class Event;

	//! Event listener interface class
	class EventListenerInterface {
	public:
		virtual void OnEvent(const Event * event) = 0;
	};

} // namespace scythe

#endif