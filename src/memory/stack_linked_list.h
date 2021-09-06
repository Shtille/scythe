#ifndef __SCYTHE_STACK_LINKED_LIST_H__
#define __SCYTHE_STACK_LINKED_LIST_H__

namespace scythe {

	/**
	 * Utilitary class for allocators.
	 * Represented as single linked list.
	 */
	class StackLinkedList {
	public:

		/**
		 * Structure that represents single node of list
		 */
		struct Node {
			Node * next;
		};

		/**
		 * Constructor
		 */
		StackLinkedList()
		: head_(nullptr)
		{
		}

		/**
		 * Move constructor
		 */
		StackLinkedList(StackLinkedList&& other)
		: head_(other.head_)
		{
			other.head_ = nullptr;
		}

		/**
		 * Pushes new node to top of the list
		 *
		 * @param[in] new_node A new node
		 */
		void push(Node * new_node)
		{
			new_node->next = head_;
			head_ = new_node;
		}

		/**
		 * Pops top element from the list
		 *
		 * @return A node at top
		 */
		Node * pop()
		{
			Node * top = head_;
			if (head_)
				head_ = head_->next;
			return top;
		}

	private:

		/**
		 * Disallow copy
		 */
		StackLinkedList(const StackLinkedList& stackLinkedList) = delete;

		Node * head_;
	};

} // namespace scythe

#endif