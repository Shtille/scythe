#ifndef __SCYTHE_LIST_H__
#define __SCYTHE_LIST_H__

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
	class List {

		/**
		 * Defines single list node that holds data.
		 */
		struct Node {
			Node * prev;
			Node * next;
			T data;
		};

		/**
		 * Defines predicate for matching data
		 */
		struct DataMatchPredicate {
			DataMatchPredicate(const T& data)
			: data_(data)
			{
			}
			bool operator()(const T& data)
			{
				return data_ == data;
			}
			const T& data_; // hold data reference
		};

	public:

		/**
		 * Defines iterator class.
		 */
		class Iterator {
			friend class List;

			Iterator()
			: node_(nullptr)
			{
			}
			Iterator(Node * node)
			: node_(node)
			{
			}
		public:
			Iterator(const Iterator& other)
			: node_(other.node_)
			{
			}
			Iterator& operator =(const Iterator& other)
			{
				node_ = other.node_;
				return *this;
			}
			bool operator ==(const Iterator& other)
			{
				return node_ == other.node_;
			}
			bool operator !=(const Iterator& other)
			{
				return node_ != other.node_;
			}
			Iterator& operator ++() // prefix increment
			{
				SC_ASSERT(node_ != nullptr);
				node_ = node_->next;
				return *this;
			}
			void operator ++(int) // postfix increment
			{
				SC_ASSERT(node_ != nullptr);
				node_ = node_->next;
			}
			Iterator& operator --() // prefix decrement
			{
				SC_ASSERT(node_ != nullptr);
				node_ = node_->prev;
				return *this;
			}
			void operator --(int) // postfix decrement
			{
				SC_ASSERT(node_ != nullptr);
				node_ = node_->prev;
			}
			T& operator *()
			{
				SC_ASSERT(node_ != nullptr);
				return node_->data;
			}
			T& operator ->()
			{
				SC_ASSERT(node_ != nullptr);
				return node_->data;
			}
		private:
			Node * node_;
		};

	public:

		/**
		 * Default constructor.
		 */
		List()
		: head_(nullptr)
		, tail_(nullptr)
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
		List(AllocatorType * allocator)
		: head_(nullptr)
		, tail_(nullptr)
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
		List(const List& other)
		: head_(nullptr)
		, tail_(nullptr)
		, allocator_(nullptr)
		, size_(0U)
		, owns_allocator_(false)
		{
			_set_by_copy(other);
		}

		/**
		 * Move constructor.
		 * 
		 * @param[in] other The other list.
		 */
		List(List && other)
		: head_(nullptr)
		, tail_(nullptr)
		, allocator_(nullptr)
		, size_(0U)
		, owns_allocator_(false)
		{
			_set_by_move(std::forward(other));
		}

		/**
		 * Destructor.
		 */
		~List()
		{
			_clean();
		}

		/**
		 * Copy assignment.
		 * 
		 * @param[in] other The other list.
		 */
		List& operator =(const List& other)
		{
			_set_by_copy(other);
			return *this;
		}

		/**
		 * Move assignment.
		 * 
		 * @param[in] other The other list.
		 */
		List& operator =(List && other)
		{
			_set_by_move(std::forward(other));
			return *this;
		}

		/**
		 * Returns iterator to the first element.
		 * 
		 * @return Returns iterator to the first element.
		 */
		Iterator begin()
		{
			return Iterator(head_);
		}

		/**
		 * Returns iterator to the end.
		 * 
		 * @return Returns iterator to the end.
		 */
		static Iterator end()
		{
			return Iterator(nullptr);
		}

		/**
		 * Gets the first element.
		 * 
		 * @return Returns reference to the first element.
		 */
		T& front() const
		{
			SC_ASSERT(head_ != nullptr && "Calling front() on an empty container.");
			return head_->data;
		}

		/**
		 * Gets the last element.
		 * 
		 * @return Returns reference to the last element.
		 */
		T& back() const
		{
			SC_ASSERT(tail_ != nullptr && "Calling back() on an empty container.");
			return tail_->data;
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
			node->prev = nullptr;
			node->next = head_;
			if (node->next)
				node->next->prev = node;
			head_ = node;
			if (tail_ == nullptr)
				tail_ = node;
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
			node->prev = nullptr;
			node->next = head_;
			if (node->next)
				node->next->prev = node;
			head_ = node;
			if (tail_ == nullptr)
				tail_ = node;
			++size_;
		}

		/**
		 * Pushes data to the end of the list.
		 * Version that copies data.
		 * 
		 * @param[in] data The data.
		 */
		void push_back(const T& data)
		{
			Node * node = _allocate_node();
			node->data = data;
			node->prev = tail_;
			node->next = nullptr;
			if (node->prev)
				node->prev->next = node;
			if (head_ == nullptr)
				head_ = node;
			tail_ = node;
			++size_;
		}

		/**
		 * Pushes data to the end of the list.
		 * Version that moves data.
		 * 
		 * @param[in] data The data.
		 */
		void push_back(T&& data)
		{
			Node * node = _allocate_node();
			node->data = std::move(data);
			node->prev = tail_;
			node->next = nullptr;
			if (node->prev)
				node->prev->next = node;
			if (head_ == nullptr)
				head_ = node;
			tail_ = node;
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
				if (head_)
					head_->prev = nullptr;
				if (tail_ == node)
					tail_ = nullptr;
				--size_;
				_free_node(node);
			}
		}

		/**
		 * Removes element from the end of the list.
		 */
		void pop_back()
		{
			Node * node = tail_;
			if (node)
			{
				tail_ = node->prev;
				if (tail_)
					tail_->next = nullptr;
				if (head_ == node)
					head_ = nullptr;
				--size_;
				_free_node(node);
			}
		}

		/**
		 * Swaps list with other one.
		 * 
		 * @param[in] other The other list.
		 */
		void swap(List & other)
		{
			std::swap(head_, other.head_);
			std::swap(tail_, other.tail_);
			std::swap(size_, other.size_);
			std::swap(allocator_, other.allocator_);
			std::swap(owns_allocator_, other.owns_allocator_);
		}

		/**
		 * Inserts element before the selected position.
		 * 
		 * @param[in] pos  The position to insert before.
		 * @param[in] data The data to insert.
		 */
		void insert(Iterator pos, const T& data)
		{
			if (pos.node_ == nullptr)
			{
				push_back(data);
				return;
			}

			Node * node = _allocate_node();
			node->data = data;
			node->prev = pos.node_->prev;
			node->next = pos.node_;
			if (pos.node_->prev != nullptr)
				pos.node_->prev->next = node;
			pos.node_->prev = node;
			if (pos.node_ == head_)
				head_ = node;
			++size_;
		}

		/**
		 * Erases element at selected position.
		 * 
		 * @param[in] pos  The position to erase.
		 * 
		 * @return Returns iterator to the next element.
		 */
		Iterator erase(Iterator pos)
		{
			Node * node = pos.node_;
			if (node == nullptr)
				return Iterator(nullptr);

			Iterator next_iterator(node->next);
			if (node->prev)
				node->prev->next = node->next;
			if (node->next)
				node->next->prev = node->prev;
			if (head_ == node)
				head_ = node->next;
			if (tail_ == node)
				tail_ = node->prev;
			_free_node(node);
			--size_;
			return next_iterator;
		}

		/**
		 * Removes all elements matching passed one.
		 * 
		 * @param[in] data The data to match with.
		 */
		void remove(const T& data)
		{
			DataMatchPredicate predicate(data);
			remove_if(predicate);
		}

		/**
		 * Removes all elements matching predicate.
		 * 
		 * @param[in] predicate The predicate to be matched with.
		 */
		template <class UnaryPredicate>
		void remove_if(UnaryPredicate predicate)
		{
			Node * node = head_;
			while (node != nullptr)
			{
				if (predicate(node->data))
				{
					// Remove node
					Iterator it = erase(Iterator(node));
					node = it.node_;
				}
				else
					node = node->next;
			}
		}

		/**
		 * Finds first element in the list mathing data.
		 * 
		 * @param[in] data The data to match with.
		 * 
		 * @return Returns iterator to found element or iterator to the end otherwise.
		 */
		Iterator find(const T& data)
		{
			DataMatchPredicate predicate(data);
			Node * node = head_;
			while (node != nullptr)
			{
				if (predicate(node->data))
				{
					return Iterator(node);
				}
				node = node->next;
			}
			return Iterator(nullptr);
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
		void _set_by_copy(const List& other)
		{
			_clean();
			head_ = nullptr;
			tail_ = nullptr;
			size_ = 0U;
			owns_allocator_ = other.owns_allocator_;
			if (other.owns_allocator_)
				allocator_ = new AllocatorType();
			else
				allocator_ = other.allocator_;

			// Copy other list nodes
			Node * other_node = other.head_;
			while (other_node != nullptr)
			{
				push_back(other_node->data);
				other_node = other_node->next;
			}
		}
		void _set_by_move(List && other)
		{
			_clean();
			// Copy other fields
			head_ = other.head_;
			tail_ = other.tail_;
			allocator_ = other.allocator_;
			size_ = other.size_;
			owns_allocator_ = other.owns_allocator_;
			// Nullify other
			other.head_ = nullptr;
			other.tail_ = nullptr;
			other.allocator_ = nullptr;
			other.size_ = 0U;
			other.owns_allocator_ = false;
		}

		Node * head_;
		Node * tail_;
		AllocatorType * allocator_;
		size_t size_;
		bool owns_allocator_;
	};

} // namespace scythe

#endif