#ifndef __SCYTHE_EVENT_H__
#define __SCYTHE_EVENT_H__

#include "common/types.h"
#include "string_id.h"

namespace scythe {
	
	//! Event class
	class Event {

		struct Value {
			enum Type {
				kInteger,
				kFloat,
				kBoolean,
				kStringId
			};
			Type type;
			union {
				int as_integer;
				float as_float;
				bool as_bool;
				StringID as_string_id;
			};
			void Set(int value)
			{
				type = kInteger;
				as_integer = value;
			}
			void Set(float value)
			{
				type = kFloat;
				as_float = value;
			}
			void Set(bool value)
			{
				type = kBoolean;
				as_bool = value;
			}
			void Set(StringID value)
			{
				type = kStringId;
				as_string_id = value;
			}
		};
		struct KeyValuePair {
			StringID key;
			Value value;
		};

	public:

		explicit Event(StringID type);
		~Event();

		const StringID type() const;
		StringID type();

		const int GetValueInteger(StringID key) const;
		const float GetValueFloat(StringID key) const;
		const bool GetValueBool(StringID key) const;
		const StringID GetValueStringId(StringID key) const;

		void ResetArgs();
		void AddArg(StringID key, int value);
		void AddArg(StringID key, float value);
		void AddArg(StringID key, bool value);
		void AddArg(StringID key, StringID value);

	private:

		static constexpr U32 kMaxArgs = 4;
		StringID type_;
		U32 num_args_;
		KeyValuePair args_[kMaxArgs];
	};
	
} // namespace scythe

#endif