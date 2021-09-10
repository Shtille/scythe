#ifndef __SCYTHE_LIST_H__
#define __SCYTHE_LIST_H__

#include "common/sc_assert.h"
#include "memory/default_allocator.h"

#include <utility>

namespace scythe {

	/**
	 * Defines forward list container. Implemented as single linked list.
	 * Move semantics should be defined for used type.
	 * If no allocator is provided, default allocator's new/delete allocation/deallocation routine is used.
	 * Note: copy operations require additional allocations.
	 */
	template <typename T, class AllocatorType = DefaultAllocator>
	class ForwardList {

		/**
		 * Defines single list node that holds data.
		 */
		struct Node {
			Node * next;
			T data;
		};

	public:

		/**
		 * Default constructor.
		 */
		ForwardList()
		: head_(nullptr)
		, allocator_(new AllocatorType())
		, size_(0U)
		, owns_allocator_(true)
		{
		}

		/**
		 * Constructor with allocator.
		 * 
		 * @param[in] allocator The allocator to be used to allocate nodes.
		 */
		ForwardList(AllocatorType * allocator)
		: head_(nullptr)
		, allocator_(allocator)
		, size_(0U)
		, owns_allocator_(false)
		{
		}

		/**
		 * Copy constructor.
		 * If other does not own allocator both will share it.
		 * 
		 * @param[in] other The other list.
		 */
		ForwardList(const ForwardList& other)
		{
			_set_by_copy(other);
		}

		/**
		 * Move constructor.
		 * 
		 * @param[in] other The other list.
		 */
		ForwardList(ForwardList && other)
		{
			_set_by_move(std::forward(other));
		}

		/**
		 * Destructor.
		 */
		~ForwardList()
		{
			clear();
			if (owns_allocator_)
				delete allocator_;
		}

		/**
		 * Copy assignment.
		 * 
		 * @param[in] other The other list.
		 */
		ForwardList& operator =(const ForwardList& other)
		{
			_set_by_copy(other);
			return *this;
		}

		/**
		 * Move assignment.
		 * 
		 * @param[in] other The other list.
		 */
		ForwardList& operator =(ForwardList && other)
		{
			_set_by_move(std::forward(other));
			return *this;
		}

		/**
		 * Returns front element.
		 * 
		 * @return Returns front element or nullptr.
		 */
		T front() const
		{
			SC_ASSERT(head_ != nullptr && "Calling front() on an empty container.");
			return head_->data;
		}

		/**
		 * Checks if list is empty.
		 * 
		 * @return Returns true if empty and false otherwise.
		 */
		bool empty() const
		{
			return size_ == 0U;
		}

		/**
		 * Returns number of list elements.
		 * 
		 * @return Returns list size.
		 */
		size_t size() const
		{
			return size_;
		}

		/**
		 * Returns number of list elements.
		 * 
		 * @return Returns list size.
		 */
		size_t length() const
		{
			return size_;
		}

		/**
		 * Clears list.
		 * Data move constructor is used for data destruction.
		 */
		void clear()
		{
			while (size_ != 0U)
			{
				T data(std::move(front()));
				pop_front();
			}
		}

		/**
		 * Pushes data to the beginning of the list.
		 * Version that copies data.
		 * 
		 * @param[in] data The data.
		 */
		void push_front(const T& data)
		{
			Node * node = _allocate_node();
			node->data = data;
			node->next = head_;
			head_ = node;
			++size_;
		}

		/**
		 * Pushes data to the beginning of the list.
		 * Version that moves data.
		 * 
		 * @param[in] data The data.
		 */
		void push_front(T&& data)
		{
			Node * node = _allocate_node();
			node->data = std::move(data);
			node->next = head_;
			head_ = node;
			++size_;
		}

		/**
		 * Removes element from the beginning of the list.
		 */
		void pop_front()
		{
			Node * node = head_;
			if (node)
			{
				head_ = node->next;
				--size_;
				_free_node(node);
			}
		}

		/**
		 * Swaps list with other one.
		 * 
		 * @param[in] other The other list.
		 */
		void swap(ForwardList & other)
		{
			std::swap(head_, other.head_);
			std::swap(size_, other.size_);
			std::swap(allocator_, other.allocator_);
			std::swap(owns_allocator_, other.owns_allocator_);
		}

	private:

		Node * _allocate_node()
		{
			return reinterpret_cast<Node*>(allocator_->Allocate(sizeof(Node)));
		}
		void _free_node(Node * node)
		{
			allocator_->Free(reinterpret_cast<void*>(node));
		}
		void _set_by_copy(const ForwardList& other)
		{
			head_ = nullptr;
			size_ = 0U;
			owns_allocator_ = other.owns_allocator_;
			if (other.owns_allocator_)
				allocator_ = new AllocatorType();
			else
				allocator_ = other.allocator_;

			size_t other_size = other.size();
			if (other_size > 0U)
			{
				// Allocate storage for nodes
				Node ** nodes = new Node*[other_size];

				// Copy other list nodes
				int i = 0;
				Node * other_node = other.head_;
				while (other_node != nullptr)
				{
					nodes[i++] = other_node;
					other_node = other_node->next;
				}
				// Push data in reverse order to keep the same order
				for (i = other_size - 1; i >= 0; --i)
				{
					push_front(nodes[i]->data);
				}

				// Finally
				delete[] nodes;
			}
		}
		void _set_by_move(ForwardList && other)
		{
			// Copy other fields
			head_ = other.head_;
			allocator_ = other.allocator_;
			size_ = other.size_;
			owns_allocator_ = other.owns_allocator_;
			// Nullify other
			other.head_ = nullptr;
			other.allocator_ = nullptr;
			other.size_ = 0U;
			other.owns_allocator_ = false;
		}

		Node * head_;
		AllocatorType * allocator_;
		size_t size_;
		bool owns_allocator_;
	};

} // namespace scythe

#endif