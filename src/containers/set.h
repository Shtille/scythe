#ifndef __SCYTHE_SET_H__
#define __SCYTHE_SET_H__

#include "common/sc_assert.h"
#include "memory/default_allocator.h"

#include <utility>

namespace scythe {

	/**
	 * Defines set container. Implemented as red-black tree.
	 * Move semantics should be defined for used type.
	 * If no allocator is provided, default new/delete allocation/deallocation routine is used.
	 * PoolAllocator is the best solution for custom allocator.
	 * @see PoolAllocator
	 */
	template <typename T, class AllocatorType = DefaultAllocator>
	class Set {

		/**
		 * Defines single tree node that holds data.
		 */
		struct Node {
			Node * parent;
			Node * left;
			Node * right;
			bool red; // if red = false then the node is black
			T data;

			Node(Node * node)
			: parent(node)
			, left(node)
			, right(node)
			, red(false)
			, data()
			{
			}
		};

	public:

		/**
		 * Defines iterator class.
		 */
		class Iterator {
			friend class Set;

			Iterator(Set const * set, Node * node)
			: set_(set)
			, node_(node)
			{
			}
			Node * _next(Node * prev)
			{
				Node * x = prev;
				Node * y;

				if (set_->nil_ != (y = x->right)) { /* assignment to y is intentional */
					while (y->left != set_->nil_) { /* returns the minium of the right subtree of x */
						y = y->left;
					}
					return y;
				} else {
					y = x->parent;
					while (y != set_->nil_ && x == y->right) { /* sentinel used instead of checking for nil */
						x = y;
						y = y->parent;
					}
					if (y == set_->root_)
						return set_->nil_;
					return y;
				}
			}
		public:
			Iterator(const Iterator& other)
			: set_(other.set_)
			, node_(other.node_)
			{
			}
			Iterator& operator =(const Iterator& other)
			{
				set_ = other.set_;
				node_ = other.node_;
				return *this;
			}
			bool operator ==(const Iterator& other) const
			{
				return set_ == other.set_ && node_ == other.node_;
			}
			bool operator !=(const Iterator& other) const
			{
				return set_ != other.set_ || node_ != other.node_;
			}
			Iterator& operator ++() // prefix increment
			{
				SC_ASSERT(node_ != nullptr);
				node_ = _next(node_);
				return *this;
			}
			Iterator operator ++(int) // postfix increment
			{
				SC_ASSERT(node_ != nullptr);
				Iterator it(*this);
				node_ = _next(node_);
				return it;
			}
			T& operator *()
			{
				SC_ASSERT(node_ != nullptr);
				return node_->data;
			}
			const T& operator *() const
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
			Set const * set_;
			Node * node_;
		};

	public:

		/**
		 * Default constructor.
		 */
		Set()
		: nil_(nullptr)
		, root_(nullptr)
		, allocator_(new AllocatorType())
		, size_(0U)
		, owns_allocator_(true)
		{
			nil_ = _make_nil_node();
			root_ = _make_root_node();
		}

		/**
		 * Constructor with allocator.
		 * 
		 * @param[in] allocator The allocator to be used to allocate nodes.
		 */
		Set(AllocatorType * allocator)
		: nil_(nullptr)
		, root_(nullptr)
		, allocator_(allocator)
		, size_(0U)
		, owns_allocator_(false)
		{
			nil_ = _make_nil_node();
			root_ = _make_root_node();
		}

		/**
		 * Copy constructor.
		 * If other does not own allocator both will share it.
		 * 
		 * @param[in] other The other set.
		 */
		Set(const Set& other)
		: nil_(nullptr)
		, root_(nullptr)
		, allocator_(nullptr)
		, size_(0U)
		, owns_allocator_(false)
		{
			_set_by_copy(other);
		}

		/**
		 * Move constructor.
		 * 
		 * @param[in] other The other set.
		 */
		Set(Set && other)
		: nil_(nullptr)
		, root_(nullptr)
		, allocator_(nullptr)
		, size_(0U)
		, owns_allocator_(false)
		{
			_set_by_move(std::forward(other));
		}

		/**
		 * Destructor.
		 */
		~Set()
		{
			_clean();
		}

		/**
		 * Copy assignment.
		 * 
		 * @param[in] other The other set.
		 */
		Set& operator =(const Set& other)
		{
			_set_by_copy(other);
			return *this;
		}

		/**
		 * Move assignment.
		 * 
		 * @param[in] other The other set.
		 */
		Set& operator =(Set && other)
		{
			_set_by_move(std::forward(other));
			return *this;
		}

		/**
		 * Checks if set is empty.
		 * 
		 * @return Returns true if empty and false otherwise.
		 */
		bool empty() const
		{
			return size_ == 0U;
		}

		/**
		 * Returns number of set elements.
		 * 
		 * @return Returns set size.
		 */
		size_t size() const
		{
			return size_;
		}

		/**
		 * Clears set.
		 * Data move constructor is used for data destruction.
		 */
		void clear()
		{
			_destroy_helper(root_->left);
			root_->left = nil_;
			size_ = 0U;
		}

		/**
		 * Returns iterator to the first element.
		 * 
		 * @return Returns iterator to the first element.
		 */
		Iterator begin()
		{
			Node * x = root_->left;
			if (x != nil_)
			{
				while (x->left != nil_)
					x = x->left;
				return Iterator(this, x);
			}
			else
				return end();
		}

		/**
		 * Returns iterator to the end.
		 * 
		 * @return Returns iterator to the end.
		 */
		Iterator end()
		{
			return Iterator(this, nil_);
		}

		/**
		 * Inserts element in the set.
		 * Version that copies data.
		 * If find call precedes insert call then you should prefer
		 * trusted_insert over regular insert call for performance reason.
		 * @see trusted_insert
		 * 
		 * @param[in] value The value.
		 * 
		 * @return Returns a pair consisting of an iterator to the inserted element 
		 *         (or to the element that prevented the insertion) and 
		 *         a bool value set to true if the insertion took place.
		 */
		std::pair<Iterator, bool> insert(const T& value)
		{
			Node * existing = _search(value);
			if (existing != nil_)
				return std::pair<Iterator, bool>(Iterator(this, existing), false);

			Node * x;
			Node * new_node;

			x = _allocate_node();
			x->data = value;

			new_node = _insert(x);
			return std::pair<Iterator, bool>(Iterator(this, new_node), true);
		}

		/**
		 * Inserts element in the set.
		 * Version that moves data.
		 * If find call precedes insert call then you should prefer
		 * trusted_insert over regular insert call for performance reason.
		 * @see trusted_insert
		 * 
		 * @param[in] value The value.
		 * 
		 * @return Returns a pair consisting of an iterator to the inserted element 
		 *         (or to the element that prevented the insertion) and 
		 *         a bool value set to true if the insertion took place.
		 */
		std::pair<Iterator, bool> insert(T && value)
		{
			Node * existing = _search(value);
			if (existing != nil_)
				return std::pair<Iterator, bool>(Iterator(this, existing), false);

			Node * x;
			Node * new_node;

			x = _allocate_node();
			x->data = std::move(value);

			new_node = _insert(x);
			return std::pair<Iterator, bool>(Iterator(this, new_node), true);
		}

		/**
		 * Inserts element in the set.
		 * Version that copies data.
		 * It's guaranteed that element is not in the set (mostly had find call previously).
		 * 
		 * @param[in] value The value.
		 * 
		 * @return Returns an iterator to the inserted element.
		 */
		Iterator trusted_insert(const T& value)
		{
			Node * x;
			Node * new_node;

			x = _allocate_node();
			x->data = value;

			new_node = _insert(x);
			return Iterator(this, new_node);
		}

		/**
		 * Inserts element in the set.
		 * Version that moves data.
		 * It's guaranteed that element is not in the set (mostly had find call previously).
		 * 
		 * @param[in] value The value.
		 * 
		 * @return Returns an iterator to the inserted element.
		 */
		Iterator trusted_insert(T && value)
		{
			Node * x;
			Node * new_node;

			x = _allocate_node();
			x->data = std::move(value);

			new_node = _insert(x);
			return Iterator(this, new_node);
		}

		/**
		 * Finds element in the set.
		 * 
		 * @param[in] value The value.
		 * 
		 * @return Returns an iterator to the found element or end iterator.
		 */
		Iterator find(const T& value)
		{
			Node * node = _search(value);
			if (node != nil_)
				return Iterator(this, node);
			else
				return end();
		}

		/**
		 * Removes element from the set.
		 * 
		 * @param[in] pos The iterator to the element to remove.
		 */
		void erase(Iterator pos)
		{
			SC_ASSERT(pos.node_ != nil_);
			_delete(pos.node_);
		}

		/**
		 * Removes all elements with value from the set.
		 * 
		 * @param[in] value The value to match removable elements.
		 * 
		 * @return Number of elements removed (0 or 1).
		 */
		size_t erase(const T& value)
		{
			size_t count = 0U;
			Node * node;
			for (Iterator it = begin(); it != end(); ++it)
			{
				node = it.node_;
				if (node->data == value)
				{
					_delete(node);
					++count;
				}
			}
			return count;
		}

		/**
		 * Swaps set with other one.
		 * 
		 * @param[in] other The other set.
		 */
		void swap(Set & other)
		{
			std::swap(nil_, other.nil_);
			std::swap(root_, other.root_);
			std::swap(size_, other.size_);
			std::swap(allocator_, other.allocator_);
			std::swap(owns_allocator_, other.owns_allocator_);
		}

	private: // Helpers

		void _left_rotate(Node * x)
		{
			Node * y;

			y = x->right;
			x->right = y->left;

			if (y->left != nil_) y->left->parent = x; /* used to use sentinel here */
			/* and do an unconditional assignment instead of testing for nil */

			y->parent = x->parent;

			/* instead of checking if x->parent is the root as in the book, we */
			/* count on the root sentinel to implicitly take care of this case */
			if (x == x->parent->left) {
				x->parent->left = y;
			} else {
				x->parent->right = y;
			}
			y->left = x;
			x->parent = y;
		}

		void _right_rotate(Node * y)
		{
			Node * x;

			x = y->left;
			y->left = x->right;

			if (nil_ != x->right) x->right->parent = y; /*used to use sentinel here */
			/* and do an unconditional assignment instead of testing for nil */

			/* instead of checking if x->parent is the root as in the book, we */
			/* count on the root sentinel to implicitly take care of this case */
			x->parent = y->parent;
			if (y == y->parent->left) {
				y->parent->left = x;
			} else {
				y->parent->right = x;
			}
			x->right = y;
			y->parent = x;
		}

		Node * _search(const T& data)
		{
			Node * x = root_->left;
			if (x == nil_)
				return nil_;
			while (!(data == x->data)) {
				if (data < x->data)
					x = x->left;
				else
					x = x->right;
				if (x == nil_)
					return nil_;
			}
			return x;
		}

		void _insert_help(Node * z)
		{
			Node * x;
			Node * y;

			z->left = z->right = nil_;
			y = root_;
			x = root_->left;
			while (x != nil_) {
				y = x;
				if (z->data < x->data) { /* x.data > z.data */
					x = x->left;
				} else { /* x.data <= z.data */
					x = x->right;
				}
			}
			z->parent = y;
			if ((y == root_) ||
				(z->data < y->data)) { /* y.data > z.data */
				y->left = z;
			} else {
				y->right = z;
			}
		}

		Node * _insert(Node * x)
		{
			Node * y;
			Node * new_node;

			_insert_help(x);
			new_node = x;
			x->red = true;
			while (x->parent->red)
			{ /* use sentinel instead of checking for root */
				if (x->parent == x->parent->parent->left)
				{
					y = x->parent->parent->right;
					if (y->red)
					{
						x->parent->red = false;
						y->red = false;
						x->parent->parent->red = true;
						x = x->parent->parent;
					}
					else
					{
						if (x == x->parent->right)
						{
							x = x->parent;
							_left_rotate(x);
						}
						x->parent->red = false;
						x->parent->parent->red = true;
						_right_rotate(x->parent->parent);
					}
				}
				else
				{ /* case for x->parent == x->parent->parent->right */
					y = x->parent->parent->left;
					if (y->red)
					{
						x->parent->red = false;
						y->red = false;
						x->parent->parent->red = true;
						x = x->parent->parent;
					}
					else
					{
						if (x == x->parent->left)
						{
							x = x->parent;
							_right_rotate(x);
						}
						x->parent->red = false;
						x->parent->parent->red = true;
						_left_rotate(x->parent->parent);
					}
				}
			}
			root_->left->red = false;
			++size_;
			return new_node;
		}

		void _destroy_helper(Node * x)
		{
			if (x != nil_) {
				_destroy_helper(x->left);
				_destroy_helper(x->right);
				// Destroy data
				x->data.~T(); // or: T data(std::move(x->data));
				_free_node(x);
			}
		}

		/* Performs rotations and changes colors to restore red-black properties after a node is deleted */
		void _delete_fix_up(Node * x)
		{
			Node * root = root_->left;
			Node * w;

			while ((!x->red) && (root != x))
			{
				if (x == x->parent->left)
				{
					w = x->parent->right;
					if (w->red)
					{
						w->red = false;
						x->parent->red = true;
						_left_rotate(x->parent);
						w = x->parent->right;
					}
					if ((!w->right->red) && (!w->left->red) )
					{
						w->red = true;
						x = x->parent;
					} else {
						if (!w->right->red) {
							w->left->red = false;
							w->red = true;
							_right_rotate(w);
							w = x->parent->right;
						}
						w->red = x->parent->red;
						x->parent->red = false;
						w->right->red = false;
						_left_rotate(x->parent);
						x = root; /* this is to exit while loop */
					}
				} else { /* the code below is has left and right switched from above */
					w = x->parent->left;
					if (w->red) {
						w->red = false;
						x->parent->red = true;
						_right_rotate(x->parent);
						w = x->parent->left;
					}
					if ( (!w->right->red) && (!w->left->red) ) { 
						w->red = true;
						x = x->parent;
					} else {
						if (!w->left->red) {
							w->right->red = false;
							w->red = true;
							_left_rotate(w);
							w = x->parent->left;
						}
						w->red = x->parent->red;
						x->parent->red = false;
						w->left->red = false;
						_right_rotate(x->parent);
						x = root; /* this is to exit while loop */
					}
				}
			}
			x->red = false;
		}

		Node * _tree_successor(Node * x)
		{
			Node * y;

			if (nil_ != (y = x->right)) { /* assignment to y is intentional */
				while (y->left != nil_) { /* returns the minium of the right subtree of x */
					y = y->left;
				}
				return y;
			} else {
				y = x->parent;
				while (x == y->right) { /* sentinel used instead of checking for nil */
					x = y;
					y = y->parent;
				}
				if (y == root_)
					return nil_;
				return y;
			}
		}

		void _delete(Node * z)
		{
			Node * y;
			Node * x;
			Node * root = root_;

			y = ((z->left == nil_) || (z->right == nil_)) ? z : _tree_successor(z);
			x = (y->left == nil_) ? y->right : y->left;
			if (root == (x->parent = y->parent)) { /* assignment of y->p to x->p is intentional */
				root->left = x;
			} else {
				if (y == y->parent->left) {
					y->parent->left = x;
				} else {
					y->parent->right = x;
				}
			}
			if (y != z) { /* y should not be nil in this case */

				/* y is the node to splice out and x is its child */
				if (!(y->red))
					_delete_fix_up(x);

				z->data.~T(); // call destructor
				y->left = z->left;
				y->right = z->right;
				y->parent = z->parent;
				y->red = z->red;
				z->left->parent = z->right->parent = y;
				if (z == z->parent->left) {
					z->parent->left = y; 
				} else {
					z->parent->right = y;
				}
				_free_node(z);
			} else {
				y->data.~T(); // call destructor
				if (!(y->red))
					_delete_fix_up(x);
				_free_node(y);
			}
			--size_;
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
		Node * _make_nil_node()
		{
			Node * node = _allocate_node();
			new (node) Node(node);
			return node;
		}
		Node * _make_root_node()
		{
			// NIL node should be initialized first
			SC_ASSERT(nil_ != nullptr);
			Node * node = _allocate_node();
			new (node) Node(nil_);
			return node;
		}
		void _clean()
		{
			if (root_ != nullptr)
			{
				_destroy_helper(root_->left);
				_free_node(root_);
				root_ = nullptr;
			}
			if (nil_ != nullptr)
			{
				_free_node(nil_);
				nil_ = nullptr;
			}
			if (owns_allocator_)
			{
				delete allocator_;
				allocator_ = nullptr;
			}
		}
		void _set_by_copy(const Set& other)
		{
			// Clean old data
			_clean();

			// Set values
			nil_ = _make_nil_node();
			root_ = _make_root_node();
			size_ = 0U;
			owns_allocator_ = other.owns_allocator_;
			if (other.owns_allocator_)
				allocator_ = new AllocatorType();
			else
				allocator_ = other.allocator_;

			// Copy other set nodes
			Node * x;
			for (Iterator it = other.begin(); it != other.end(); ++it)
			{
				x = _allocate_node();
				x->data = it.node_->data;
				(void)_insert(x);
			}
		}
		void _set_by_move(Set && other)
		{
			// Clean old data
			_clean();
			// Copy other fields
			nil_ = other.nil_;
			root_ = other.root_;
			allocator_ = other.allocator_;
			size_ = other.size_;
			owns_allocator_ = other.owns_allocator_;
			// Nullify other
			other.nil_ = nullptr;
			other.root_ = nullptr;
			other.allocator_ = nullptr;
			other.size_ = 0U;
			other.owns_allocator_ = false;
		}

		Node * nil_;
		Node * root_;
		AllocatorType * allocator_;
		size_t size_;
		bool owns_allocator_;
	};

} // namespace scythe

#endif