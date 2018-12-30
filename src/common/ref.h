#ifndef __SCYTHE_REF_H__
#define __SCYTHE_REF_H__

namespace scythe {

	/**
	 * Defines the base class for game objects that require lifecycle management.
	 *
	 * This class provides reference counting support for game objects that
	 * contain system resources or data that is normally long lived and
	 * referenced from possibly several sources at the same time. The built-in
	 * reference counting eliminates the need for programmers to manually
	 * keep track of object ownership and having to worry about when to
	 * safely delete such objects.
	 */
	class Ref
	{
	public:

		/**
		 * Increments the reference count of this object.
		 *
		 * The release() method must be called when the caller relinquishes its
		 * handle to this object in order to decrement the reference count.
		 */
		void AddRef();

		/**
		 * Decrements the reference count of this object.
		 *
		 * When an object is initially created, its reference count is set to 1.
		 * Calling addRef() will increment the reference and calling release()
		 * will decrement the reference count. When an object reaches a
		 * reference count of zero, the object is destroyed.
		 */
		void Release();

		/**
		 * Returns the current reference count of this object.
		 *
		 * @return This object's reference count.
		 */
		unsigned int GetRefCount() const;

#ifdef SC_USE_MEM_LEAK_DETECTION
		static void PrintLeaks();
#endif

	protected:

		/**
		 * Constructor.
		 */
		Ref();

		/**
		 * Copy constructor.
		 * 
		 * @param copy The Ref object to copy.
		 */
		Ref(const Ref& copy);

		/**
		 * Destructor.
		 */
		virtual ~Ref();

	private:

		unsigned int ref_count_;

		// Memory leak diagnostic data (only included when GP_USE_MEM_LEAK_DETECTION is defined)
	#ifdef SC_USE_MEM_LEAK_DETECTION
		void * record_;
	#endif
	};

} // namespace scythe

#endif