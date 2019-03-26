#include "notification.h"

namespace scythe {

	void NotificationCenter::AddListener(NotificationListener * listener, NotificationFunc func)
	{
		listeners_.insert(std::make_pair(listener, func));
	}
	void NotificationCenter::RemoveListener(NotificationListener * listener)
	{
		listeners_.erase(listener);
	}
	void NotificationCenter::Notify(Notification * notification)
	{
		for (auto &pair: listeners_)
		{
			NotificationListener * listener = pair.first;
			NotificationFunc func = pair.second;
			(listener->*func)(notification);
		}
	}

} // namespace scythe