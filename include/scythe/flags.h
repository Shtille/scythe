#ifndef __SCYTHE_FLAGS_H__
#define __SCYTHE_FLAGS_H__

#include "types.h"

namespace scythe {

/**
 * @brief      This class describes flags.
 *
 * @tparam     EnumType   The enum type.
 * @tparam     InnerType  The inner type of enum.
 */
template<class EnumType, class InnerType = U32>
class Flags {
public:
	Flags() = default;
	Flags(InnerType values) : values_(values) {}
	Flags(const Flags& other) : values_(other.values_) {}
	Flags(Flags&& other) : values_(other.values_) { other.values_ = 0; }
	~Flags() = default;

	Flags& operator =(const Flags& other)
	{
		values_ = other.values_;
		return *this;
	}
	Flags& operator =(Flags&& other)
	{
		values_ = other.values_;
		other.values_ = 0;
		return *this;
	}
	void operator |=(EnumType value)
	{
		Add(value);
	}
	Flags& operator |(const Flags& other)
	{
		values_ |= other.values_;
		return *this;
	}

	void Clear()
	{
		values_ = 0;
	}
	void Add(EnumType value)
	{
		values_ |= InnerCast(value);
	}
	void Remove(EnumType value)
	{
		values_ &= ~InnerCast(value);
	}

private:

	InnerType InnerCast(EnumType value)
	{
		return static_cast<InnerType>(value);
	}

	InnerType values_ = 0;
};

} // namespace scythe

#endif