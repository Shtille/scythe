#ifndef __SCYTHE_EXPANDABLE_H__
#define __SCYTHE_EXPANDABLE_H__

#include <type_traits>

namespace scythe {

	/**
	 * @brief      This class describes an expandable.
	 *
	 * @tparam     Base  The base class.
	 */
	template<class Base>
	class Expandable
	{
	public:
		Expandable() = default;
		virtual ~Expandable() = default;

		/**
		 * @brief      Upcast to successor
		 *
		 * @tparam     Derived  The class derived from base.
		 *
		 * @return     Pointer to derived class instance.
		 */
		template <class Derived>
		Derived* Upcast()
		{
			static_assert(std::is_convertible<Derived*, Base*>::value, "Can't convert to derived type");
			return dynamic_cast<Derived*>(this);
		}
	};

} // namespace scythe

#endif