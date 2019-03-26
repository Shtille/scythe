#ifndef __SCYTHE_NOTIFICATION_H__
#define __SCYTHE_NOTIFICATION_H__

#include <unordered_map>

namespace scythe {

	/**
	 * Base class for any notification
	 */
	class Notification {
	public:
		Notification() = default;
		virtual ~Notification() = default;
	};

	/**
	 * Base class for notification listener
	 */
	class NotificationListener {
	public:
		NotificationListener() = default;
		virtual ~NotificationListener() = default;
	};

	/**
	 * Notification function declaration
	 */
	typedef void (NotificationListener::*NotificationFunc)(Notification*);

	/**
	 * Notification center class
	 */
	class NotificationCenter {
	public:
		NotificationCenter() = default;
		~NotificationCenter() = default;
		
		/**
		 * Adds listener to list.
		 * Complexity: O(1).
		 * @param[in] listener 	A listener.
		 * @param[in] func 		Callback function.
		 */
		void AddListener(NotificationListener * listener, NotificationFunc func);

		/**
		 * Removes listener from list.
		 * Complexity: O(1).
		 * @param[in] listener 	A listener.
		 */
		void RemoveListener(NotificationListener * listener);

		/**
		 * Notifies listeners about event.
		 * Complexity: O(N).
		 * @param[in] notification 	A notification to be passed into every listener.
		 */
		void Notify(Notification * notification);

	private:
		std::unordered_map<NotificationListener*, NotificationFunc> listeners_;
	};

} // namespace scythe

#endif