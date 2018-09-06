#include "event.h"

#include "common/sc_assert.h"

namespace scythe {

	Event::Event(StringID type)
	: type_(type)
	, num_args_(0)
	{

	}
	Event::~Event()
	{

	}
	const StringID Event::type() const
	{
		return type_;
	}
	StringID Event::type()
	{
		return type_;
	}
	const int Event::GetValueInteger(StringID key) const
	{
		for (U32 i = 0; i < num_args_; ++i)
		{
			const KeyValuePair & pair = args_[i];
			if (pair.key == key)
			{
				SC_ASSERT(pair.value.type == Value::kInteger);
				return pair.value.as_integer;
			}
		}
		SC_ASSERT(!"Haven't found event key");
		return 0;
	}
	const float Event::GetValueFloat(StringID key) const
	{
		for (U32 i = 0; i < num_args_; ++i)
		{
			const KeyValuePair & pair = args_[i];
			if (pair.key == key)
			{
				SC_ASSERT(pair.value.type == Value::kFloat);
				return pair.value.as_float;
			}
		}
		SC_ASSERT(!"Haven't found event key");
		return 0.0f;
	}
	const bool Event::GetValueBool(StringID key) const
	{
		for (U32 i = 0; i < num_args_; ++i)
		{
			const KeyValuePair & pair = args_[i];
			if (pair.key == key)
			{
				SC_ASSERT(pair.value.type == Value::kBoolean);
				return pair.value.as_bool;
			}
		}
		SC_ASSERT(!"Haven't found event key");
		return false;
	}
	const StringID Event::GetValueStringId(StringID key) const
	{
		for (U32 i = 0; i < num_args_; ++i)
		{
			const KeyValuePair & pair = args_[i];
			if (pair.key == key)
			{
				SC_ASSERT(pair.value.type == Value::kStringId);
				return pair.value.as_string_id;
			}
		}
		SC_ASSERT(!"Haven't found event key");
		return 0;
	}
	void Event::ResetArgs()
	{
		num_args_ = 0;
	}
	void Event::AddArg(StringID key, int value)
	{
		if (num_args_ < kMaxArgs)
		{
			KeyValuePair & pair = args_[num_args_];
			pair.key = key;
			pair.value.Set(value);
			++num_args_;
		}
		else
		{
			SC_ASSERT(!"Need larger args buffer");
		}
	}
	void Event::AddArg(StringID key, float value)
	{
		if (num_args_ < kMaxArgs)
		{
			KeyValuePair & pair = args_[num_args_];
			pair.key = key;
			pair.value.Set(value);
			++num_args_;
		}
		else
		{
			SC_ASSERT(!"Need larger args buffer");
		}
	}
	void Event::AddArg(StringID key, bool value)
	{
		if (num_args_ < kMaxArgs)
		{
			KeyValuePair & pair = args_[num_args_];
			pair.key = key;
			pair.value.Set(value);
			++num_args_;
		}
		else
		{
			SC_ASSERT(!"Need larger args buffer");
		}
	}
	void Event::AddArg(StringID key, StringID value)
	{
		if (num_args_ < kMaxArgs)
		{
			KeyValuePair & pair = args_[num_args_];
			pair.key = key;
			pair.value.Set(value);
			++num_args_;
		}
		else
		{
			SC_ASSERT(!"Need larger args buffer");
		}
	}

} // namespace scythe