#ifndef __SCYTHE_ENUM_TABLE_H__
#define __SCYTHE_ENUM_TABLE_H__

#include <unordered_map>

namespace scythe {

	struct EnumClassHash {
		template <typename T>
		std::size_t operator()(T t) const
		{
			return static_cast<std::size_t>(t);
		}
	};

	template <typename K, typename T>
	using EnumTable = std::unordered_map<K, T, EnumClassHash>;

} // namespace scythe

#endif