#include "ref.h"

#ifdef SC_USE_MEM_LEAK_DETECTION
#include <cstdlib>
#include <typeinfo>
#endif

namespace scythe {

#ifdef SC_USE_MEM_LEAK_DETECTION

	struct RefAllocationRecord
	{
		Ref* ref;
		RefAllocationRecord* next;
		RefAllocationRecord* prev;
	};

	static RefAllocationRecord* s_ref_allocations = 0;
	static int s_ref_allocation_count = 0;

	void Ref::PrintLeaks()
	{
		// Dump Ref object memory leaks
		if (s_ref_allocation_count == 0)
		{
			print("[memory] All Ref objects successfully cleaned up (no leaks detected).\n");
		}
		else
		{
			print("[memory] WARNING: %d Ref objects still active in memory.\n", s_ref_allocation_count);
			for (RefAllocationRecord* rec = s_ref_allocations; rec != NULL; rec = rec->next)
			{
				Ref* ref = rec->ref;
				SC_ASSERT(ref);
				const char* type = typeid(*ref).name();
				print("[memory] LEAK: Ref object '%s' still active with reference count %d.\n", (type ? type : ""), ref->getRefCount());
			}
		}
	}

	static void* TrackRef(Ref* ref)
	{
		SC_ASSERT(ref);

		// Create memory allocation record.
		RefAllocationRecord* rec = (RefAllocationRecord*)malloc(sizeof(RefAllocationRecord));
		rec->ref = ref;
		rec->next = s_ref_allocations;
		rec->prev = 0;

		if (s_ref_allocations)
			s_ref_allocations->prev = rec;
		s_ref_allocations = rec;
		++s_ref_allocation_count;

		return rec;
	}

	static void UntrackRef(Ref* ref, void* record)
	{
		if (!record)
		{
			print("[memory] ERROR: Attempting to free null ref tracking record.\n");
			return;
		}

		RefAllocationRecord* rec = (RefAllocationRecord*)record;
		if (rec->ref != ref)
		{
			print("[memory] CORRUPTION: Attempting to free Ref with invalid ref tracking record.\n");
			return;
		}

		// Link this item out.
		if (s_ref_allocations == rec)
			s_ref_allocations = rec->next;
		if (rec->prev)
			rec->prev->next = rec->next;
		if (rec->next)
			rec->next->prev = rec->prev;
		free((void*)rec);
		--s_ref_allocation_count;
	}

#endif

	Ref::Ref()
	: ref_count_(1)
	{
#ifdef SC_USE_MEM_LEAK_DETECTION
		record_ = TrackRef(this);
#endif
	}
	Ref::Ref(const Ref& copy)
	: ref_count_(1)
	{
#ifdef SC_USE_MEM_LEAK_DETECTION
		record_ = TrackRef(this);
#endif
	}
	Ref::~Ref()
	{
	}
	void Ref::AddRef()
	{
		SC_ASSERT(ref_count_ > 0 && ref_count_ < 1000000);
		++ref_count_;
	}
	void Ref::Release()
	{
		SC_ASSERT(ref_count_ > 0 && ref_count_ < 1000000);
		if ((--ref_count_) <= 0)
		{
#ifdef SC_USE_MEM_LEAK_DETECTION
			UntrackRef(this, record_);
#endif
			delete this;
		}
	}
	unsigned int Ref::GetRefCount() const
	{
		return ref_count_;
	}

} // namespace scythe