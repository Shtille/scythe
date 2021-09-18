#ifndef __SCYTHE_STACK_H__
#define __SCYTHE_STACK_H__

#include "common/sc_assert.h"
#include "memory/default_allocator.h"

#include <utility>

namespace scythe {

	/**
	 * Defines stack container. Implemented as single linked list.
	 * Move semantics should be defined for used type.
	 * If no allocator is provided, default allocator's new/delete allocation/deallocation routine is used.
	 * Note: copy operations require additional allocations.
	 */
	template <typename T, class AllocatorType = DefaultAllocator>
	class Stack {

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
		Stack()
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
		Stack(AllocatorType * allocator)
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
		 * @param[in] other The other stack.
		 */
		Stack(const Stack& other)
		: head_(nullptr)
		, allocator_(nullptr)
		, size_(0U)
		, owns_allocator_(false)
		{
			_set_by_copy(other);
		}

		/**
		 * Move constructor.
		 * 
		 * @param[in] other The other stack.
		 */
		Stack(Stack && other)
		: head_(nullptr)
		, allocator_(nullptr)
		, size_(0U)
		, owns_allocator_(false)
		{
			_set_by_move(std::forward(other));
		}

		/**
		 * Destructor.
		 */
		~Stack()
		{
			_clean();
		}

		/**
		 * Copy assignment.
		 * 
		 * @param[in] other The other stack.
		 */
		Stack& operator =(const Stack& other)
		{
			_set_by_copy(other);
			return *this;
		}

		/**
		 * Move assignment.
		 * 
		 * @param[in] other The other stack.
		 */
		Stack& operator =(Stack && other)
		{
			_set_by_move(std::forward(other));
			return *this;
		}

		/**
		 * Gets the top element.
		 * 
		 * @return Returns reference to the top element.
		 */
		T& top() const
		{
			SC_ASSERT(head_ != nullptr && "Calling top() on an empty container.");
			return head_->data;
		}

		/**
		 * Checks if stack is empty.
		 * 
		 * @return Returns true if empty and false otherwise.
		 */
		bool empty() const
		{
			return size_ == 0U;
		}

		/**
		 * Returns number of stack elements.
		 * 
		 * @return Returns stack size.
		 */
		size_t size() const
		{
			return size_;
		}

		/**
		 * Clears stack.
		 * Data move constructor is used for data destruction.
		 */
		void clear()
		{
			while (size_ != 0U)
			{
				T data(std::move(top()));
				pop();
			}
		}

		/**
		 * Pushes data to the top of the stack.
		 * Version that copies data.
		 * 
		 * @param[in] data The data.
		 */
		void push(const T& data)
		{
			Node * node = _allocate_node();
			node->data = data;
			node->next = head_;
			head_ = node;
			++size_;
		}

		/**
		 * Pushes data to the top of the stack.
		 * Version that moves data.
		 * 
		 * @param[in] data The data.
		 */
		void push(T&& data)
		{
			Node * node = _allocate_node();
			node->data = std::move(data);
			node->next = head_;
			head_ = node;
			++size_;
		}

		/**
		 * Removes element from the top of the stack.
		 */
		void pop()
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
		 * Swaps stack with other one.
		 * 
		 * @param[in] other The other stack.
		 */
		void swap(Stack & other)
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
		void _clean()
		{
			clear();
			if (owns_allocator_)
				delete allocator_;
		}
		void _set_by_copy(const Stack& other)
		{
			_clean();
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

				// Copy other stack nodes
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
		void _set_by_move(Stack && other)
		{
			_clean();
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