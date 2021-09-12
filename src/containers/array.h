#ifndef __SCYTHE_ARRAY_H__
#define __SCYTHE_ARRAY_H__

#include "common/sc_assert.h"
#include "memory/default_allocator.h"

#include <utility>

namespace scythe {

	/**
	 * Defines list container. Implemented as double linked list.
	 * Move semantics should be defined for used type.
	 * If no allocator is provided, default new/delete allocation/deallocation routine is used.
	 */
	template <typename T, class AllocatorType = DefaultAllocator>
	class Array {
	public:

		/**
		 * Defines iterator class.
		 */
		class Iterator {
			friend class Array;

			Iterator()
			: ptr_(nullptr)
			{
			}
			Iterator(T * ptr)
			: ptr_(ptr)
			{
			}
		public:
			Iterator(const Iterator& other)
			: ptr_(other.ptr_)
			{
			}
			Iterator& operator =(const Iterator& other)
			{
				ptr_ = other.ptr_;
				return *this;
			}
			bool operator ==(const Iterator& other)
			{
				return ptr_ == other.ptr_;
			}
			bool operator !=(const Iterator& other)
			{
				return ptr_ != other.ptr_;
			}
			Iterator& operator ++() // prefix increment
			{
				SC_ASSERT(ptr_ != nullptr);
				++ptr_;
				return *this;
			}
			Iterator operator ++(int) // postfix increment
			{
				SC_ASSERT(ptr_ != nullptr);
				Iterator it(ptr_);
				++ptr_;
				return it;
			}
			Iterator& operator --() // prefix decrement
			{
				SC_ASSERT(ptr_ != nullptr);
				--ptr_;
				return *this;
			}
			void operator --(int) // postfix decrement
			{
				SC_ASSERT(ptr_ != nullptr);
				Iterator it(ptr_);
				--ptr_;
				return it;
			}
			T& operator *()
			{
				SC_ASSERT(ptr_ != nullptr);
				return *ptr_;
			}
			T* operator ->()
			{
				SC_ASSERT(ptr_ != nullptr);
				return ptr_;
			}
		private:
			T * ptr_;
		};

	public:

		/**
		 * Default constructor.
		 */
		Array()
		: buffer_(nullptr)
		, allocator_(new AllocatorType())
		, buffer_size_(0U)
		, size_(0U)
		, owns_allocator_(true)
		{
		}

		/**
		 * Constructor with allocator.
		 * 
		 * @param[in] allocator The allocator to be used to allocate nodes.
		 */
		Array(AllocatorType * allocator)
		: buffer_(nullptr)
		, allocator_(allocator)
		, buffer_size_(0U)
		, size_(0U)
		, owns_allocator_(false)
		{
		}

		/**
		 * Copy constructor.
		 * If other does not own allocator both will share it.
		 * 
		 * @param[in] other The other array.
		 */
		Array(const Array& other)
		{
			_set_by_copy(other);
		}

		/**
		 * Move constructor.
		 * 
		 * @param[in] other The other array.
		 */
		Array(Array && other)
		{
			_set_by_move(std::forward(other));
		}

		/**
		 * Destructor.
		 */
		~Array()
		{
			clear();
			if (buffer_ != nullptr)
			{
				_free_buffer(buffer_);
				buffer_ = nullptr;
			}
		}

		/**
		 * Copy assignment.
		 * 
		 * @param[in] other The other array.
		 */
		T& operator =(const Array& other)
		{
			_set_by_copy(other);
			return *this;
		}

		/**
		 * Move assignment.
		 * 
		 * @param[in] other The other array.
		 */
		T& operator =(Array && other)
		{
			_set_by_move(std::forward(other));
			return *this;
		}

		/**
		 * Accesses element at index.
		 * 
		 * @param[in] index  The index in the array.
		 * 
		 * @return Returns element at index.
		 */
		T& operator [](size_t index)
		{
			return buffer_[index];
		}

		/**
		 * Accesses element at index. Checks bounds.
		 * 
		 * @param[in] index  The index in the array.
		 * 
		 * @return Returns element at index.
		 */
		T& at(size_t index)
		{
			SC_ASSERT(0U <= index && index < size_);
			return buffer_[index];
		}

		/**
		 * Checks if container is empty.
		 * 
		 * @return Returns true if empty and false otherwise.
		 */
		bool empty() const
		{
			return size_ == 0U;
		}

		/**
		 * Gets size of container.
		 * 
		 * @return Returns size of container.
		 */
		size_t size() const
		{
			return size_;
		}

		/**
		 * Gets capacity of container.
		 * 
		 * @return Returns capacity of container.
		 */
		size_t capacity() const
		{
			return buffer_size_;
		}

		/**
		 * Gets pointer to buffer.
		 * 
		 * @return Returns pointer to buffer.
		 */
		T* data()
		{
			return buffer_;
		}

		/**
		 * Gets the first element in the array.
		 * 
		 * @return Returns the first element.
		 */
		T& front()
		{
			return buffer_[0U];
		}

		/**
		 * Gets the last element in the array.
		 * 
		 * @return Returns the last element.
		 */
		T& back()
		{
			return buffer_[size_ - 1];
		}

		/**
		 * Returns iterator to the first element.
		 * 
		 * @return Returns iterator to the first element.
		 */
		Iterator begin()
		{
			return Iterator(buffer_);
		}

		/**
		 * Returns iterator to the end.
		 * 
		 * @return Returns iterator to the end.
		 */
		Iterator end()
		{
			return Iterator(buffer_ + size_);
		}

		/**
		 * Clears all elements in the list
		 */
		void clear()
		{
			for (size_t i = 0U; i < size_; ++i)
				(buffer_ + i)->~T();
			size_ = 0U;
		}

		/**
		 * Reserves buffer for number of elements.
		 * 
		 * @param[in] size  The size to reserve.
		 */
		void reserve(size_t size)
		{
			if (size == 0U)
			{
				if (buffer_ != nullptr)
				{
					_free_buffer(buffer_);
					buffer_ = nullptr;
				}
			}
			else if (buffer_size_ < size)
			{
				if (buffer_ != nullptr)
				{
					// Allocate a new buffer
					T * new_buffer = _allocate_buffer(size);
					// Call move constructor on old elements
					for (size_t i = 0U; i < size_; ++i)
					{
						new (new_buffer + i) T(std::move(buffer_[i]));
					}
					// Free old buffer
					_free_buffer(buffer_);
					// Replace buffers
					buffer_ = new_buffer;
				}
				else
				{
					// Allocate a new buffer
					buffer_ = _allocate_buffer(size);
				}
			}
			buffer_size_ = size;
		}

		/**
		 * Resizes array to the new size.
		 * 
		 * @param[in] new_size  The new size.
		 */
		void resize(size_t new_size)
		{
			size_t old_size = size_;
			size_ = new_size;
			if (new_size <= old_size)
			{
				// Destruct old elements
				for (size_t i = new_size; i < old_size; ++i)
					(buffer_ + i)->~T();
				return;
			}
			// New size is greater than old one
			reserve(new_size + (new_size >> 2));
			// Copy default T into new elements.
			for (size_t i = old_size; i < new_size; ++i)
			{
				new (buffer_ + i) T();
			}
		}

		/**
		 * Pushes data to the end of the array.
		 * Version that copies data.
		 * 
		 * @param[in] value The data.
		 */
		void push_back(const T& value)
		{
			resize(size_ + 1);
			buffer_[size_ - 1] = value;
		}

		/**
		 * Pushes data to the end of the array.
		 * Version that moves data.
		 * 
		 * @param[in] value The data.
		 */
		void push_back(T && value)
		{
			resize(size_ + 1);
			buffer_[size_ - 1] = std::move(value);
		}

		/**
		 * Removes element from the end of the array.
		 */
		void pop_back()
		{
			if (size_ != 0U)
				resize(size_ - 1);
		}

		/**
		 * Swaps content with the other container.
		 * 
		 * @param[in] other  The other container.
		 */
		void swap(Array& other)
		{
			std::swap(buffer_, other.buffer_);
			std::swap(allocator_, other.allocator_);
			std::swap(buffer_size_, other.buffer_size_);
			std::swap(size_, other.size_);
			std::swap(owns_allocator_, other.owns_allocator_);
		}

	private:

		T * _allocate_buffer(size_t size)
		{
			return reinterpret_cast<T*>(allocator_->Allocate(sizeof(T) * size));
		}
		void _free_buffer(T * buffer)
		{
			allocator_->Free(reinterpret_cast<void*>(buffer));
		}
		void _set_by_copy(const Array& other)
		{
			owns_allocator_ = other.owns_allocator_;
			if (other.owns_allocator_)
				allocator_ = new AllocatorType();
			else
				allocator_ = other.allocator_;
			buffer_size_ = other.buffer_size_;
			if (other.buffer_size_ > 0U)
				buffer_ = _allocate_buffer(other.buffer_size_);
			else
				buffer_ = nullptr;
			size_ = other.size_;
			// Copy data
			for (size_t i = 0U; i < size_; ++i)
			{
				new (buffer_ + i) T(other.buffer_[i]);
			}
		}
		void _set_by_move(Array && other)
		{
			// Copy other fields
			buffer_ = other.buffer_;
			allocator_ = other.allocator_;
			buffer_size_ = other.buffer_size_;
			size_ = other.size_;
			owns_allocator_ = other.owns_allocator_;
			// Nullify other
			other.buffer_ = nullptr;
			other.allocator_ = nullptr;
			other.buffer_size_ = 0U;
			other.size_ = 0U;
			other.owns_allocator_ = false;
		}

		T * buffer_;
		AllocatorType * allocator_;
		size_t buffer_size_;
		size_t size_;
		bool owns_allocator_;
	};

} // namespace scythe

#endif