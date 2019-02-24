#ifndef __SCYTHE_INPUT_LISTENER_H__
#define __SCYTHE_INPUT_LISTENER_H__

namespace scythe {

	/**
	 * Defines a basic input listener interface
	 */
	class InputListener {
	public:
		virtual ~InputListener() = default;

		/**
		 * Upcast to successor
		 */
		template <class T>
		T * Upcast()
		{
			return dynamic_cast<T*>(this);
		}
	};

} // namespace scythe

#endif