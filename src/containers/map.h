#ifndef __SCYTHE_MAP_H__
#define __SCYTHE_MAP_H__

#include "common/sc_assert.h"
#include "memory/default_allocator.h"

#include <utility>

namespace scythe {

	/**
	 * Defines map container. Implemented as red-black tree.
	 * Move semantics should be defined for used type.
	 * If no allocator is provided, default new/delete allocation/deallocation routine is used.
	 * PoolAllocator is the best solution for custom allocator.
	 * @see PoolAllocator
	 */
	template <typename Key, typename T, class AllocatorType = DefaultAllocator>
	class Map {

		typedef std::pair<Key, T> KeyValuePair;

		/**
		 * Defines single tree node that holds data.
		 */
		struct Node {
			Node * parent;
			Node * left;
			Node * right;
			bool red; // if red = false then the node is black
			KeyValuePair data; // key value pair

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
			friend class Map;

			Iterator(Map const * map, Node * node)
			: map_(map)
			, node_(node)
			{
			}
			Node * _next(Node * prev)
			{
				Node * x = prev;
				Node * y;

				if (map_->nil_ != (y = x->right)) { /* assignment to y is intentional */
					while (y->left != map_->nil_) { /* returns the minium of the right subtree of x */
						y = y->left;
					}
					return y;
				} else {
					y = x->parent;
					while (y != map_->nil_ && x == y->right) { /* sentinel used instead of checking for nil */
						x = y;
						y = y->parent;
					}
					if (y == map_->root_)
						return map_->nil_;
					return y;
				}
			}
		public:
			Iterator(const Iterator& other)
			: map_(other.map_)
			, node_(other.node_)
			{
			}
			Iterator& operator =(const Iterator& other)
			{
				map_ = other.map_;
				node_ = other.node_;
				return *this;
			}
			bool operator ==(const Iterator& other) const
			{
				return map_ == other.map_ && node_ == other.node_;
			}
			bool operator !=(const Iterator& other) const
			{
				return map_ != other.map_ || node_ != other.node_;
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
			KeyValuePair& operator *()
			{
				SC_ASSERT(node_ != nullptr);
				return node_->data;
			}
			const KeyValuePair& operator *() const
			{
				SC_ASSERT(node_ != nullptr);
				return node_->data;
			}
			KeyValuePair& operator ->()
			{
				SC_ASSERT(node_ != nullptr);
				return node_->data;
			}
		private:
			Map const * map_;
			Node * node_;
		};

	public:

		/**
		 * Default constructor.
		 */
		Map()
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
		Map(AllocatorType * allocator)
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
		 * @param[in] other The other map.
		 */
		Map(const Map& other)
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
		 * @param[in] other The other map.
		 */
		Map(Map && other)
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
		~Map()
		{
			_clean();
		}

		/**
		 * Copy assignment.
		 * 
		 * @param[in] other The other map.
		 */
		Map& operator =(const Map& other)
		{
			_set_by_copy(other);
			return *this;
		}

		/**
		 * Move assignment.
		 * 
		 * @param[in] other The other map.
		 */
		Map& operator =(Map && other)
		{
			_set_by_move(std::forward(other));
			return *this;
		}

		/**
		 * Value access by key.
		 * 
		 * @param[in] key  The key.
		 * 
		 * @return Returns reference to found value.
		 */
		T& operator [](const Key& key)
		{
			Node * existing = _search(key);
			if (existing != nil_)
				return existing->data.second;

			Node * x;
			Node * new_node;

			x = _allocate_node();
			x->data = KeyValuePair(key, T());

			new_node = _insert(x);
			return new_node->data.second;
		}

		/**
		 * Checks if map is empty.
		 * 
		 * @return Returns true if empty and false otherwise.
		 */
		bool empty() const
		{
			return size_ == 0U;
		}

		/**
		 * Returns number of map elements.
		 * 
		 * @return Returns map size.
		 */
		size_t size() const
		{
			return size_;
		}

		/**
		 * Clears map.
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
		 * Inserts element in the map.
		 * Version that copies data.
		 * If find call precedes insert call then you should prefer
		 * trusted_insert over regular insert call for performance reason.
		 * @see trusted_insert
		 * 
		 * @param[in] value The value.
		 * 
		 * @return Returns a pair consisting of an iterator to the inserted element 
		 *         (or to the element that prevented the insertion) and 
		 *         a bool value map to true if the insertion took place.
		 */
		std::pair<Iterator, bool> insert(const KeyValuePair& value)
		{
			Node * existing = _search(value.first);
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
		 * Inserts element in the map.
		 * Version that moves data.
		 * If find call precedes insert call then you should prefer
		 * trusted_insert over regular insert call for performance reason.
		 * @see trusted_insert
		 * 
		 * @param[in] value The value.
		 * 
		 * @return Returns a pair consisting of an iterator to the inserted element 
		 *         (or to the element that prevented the insertion) and 
		 *         a bool value map to true if the insertion took place.
		 */
		std::pair<Iterator, bool> insert(KeyValuePair && value)
		{
			Node * existing = _search(value.first);
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
		 * Inserts element in the map.
		 * Version that copies data.
		 * It's guaranteed that element is not in the map (mostly had find call previously).
		 * 
		 * @param[in] value The value.
		 * 
		 * @return Returns an iterator to the inserted element.
		 */
		Iterator trusted_insert(const KeyValuePair& value)
		{
			Node * x;
			Node * new_node;

			x = _allocate_node();
			x->data = value;

			new_node = _insert(x);
			return Iterator(this, new_node);
		}

		/**
		 * Inserts element in the map.
		 * Version that moves data.
		 * It's guaranteed that element is not in the map (mostly had find call previously).
		 * 
		 * @param[in] value The value.
		 * 
		 * @return Returns an iterator to the inserted element.
		 */
		Iterator trusted_insert(KeyValuePair && value)
		{
			Node * x;
			Node * new_node;

			x = _allocate_node();
			x->data = std::move(value);

			new_node = _insert(x);
			return Iterator(this, new_node);
		}

		/**
		 * Finds element in the map.
		 * 
		 * @param[in] key The key.
		 * 
		 * @return Returns an iterator to the found element or end iterator.
		 */
		Iterator find(const Key& key)
		{
			Node * node = _search(key);
			if (node != nil_)
				return Iterator(this, node);
			else
				return end();
		}

		/**
		 * Removes element from the map.
		 * 
		 * @param[in] pos The iterator to the element to remove.
		 */
		void erase(Iterator pos)
		{
			SC_ASSERT(pos.node_ != nil_);
			_delete(pos.node_);
		}

		/**
		 * Removes all elements with key from the map.
		 * 
		 * @param[in] key The key to match removable elements.
		 * 
		 * @return Number of elements removed (0 or 1).
		 */
		size_t erase(const Key& key)
		{
			size_t count = 0U;
			Node * node;
			for (Iterator it = begin(); it != end(); ++it)
			{
				node = it.node_;
				if (node->data.first == key)
				{
					_delete(node);
					++count;
				}
			}
			return count;
		}

		/**
		 * Swaps map with other one.
		 * 
		 * @param[in] other The other map.
		 */
		void swap(Map & other)
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

		Node * _search(const Key& key)
		{
			Node * x = root_->left;
			if (x == nil_)
				return nil_;
			while (!(key == x->data.first)) {
				if (key < x->data.first)
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
				if (z->data.first < x->data.first) { /* x.data > z.data */
					x = x->left;
				} else { /* x.data <= z.data */
					x = x->right;
				}
			}
			z->parent = y;
			if ((y == root_) ||
				(z->data.first < y->data.first)) { /* y.data > z.data */
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
				x->data.~KeyValuePair(); // or: T data(std::move(x->data));
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

				z->data.~KeyValuePair(); // call destructor
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
				y->data.~KeyValuePair(); // call destructor
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
		void _set_by_copy(const Map& other)
		{
			// Clean old data
			_clean();

			// Map values
			nil_ = _make_nil_node();
			root_ = _make_root_node();
			size_ = 0U;
			owns_allocator_ = other.owns_allocator_;
			if (other.owns_allocator_)
				allocator_ = new AllocatorType();
			else
				allocator_ = other.allocator_;

			// Copy other map nodes
			Node * x;
			for (Iterator it = other.begin(); it != other.end(); ++it)
			{
				x = _allocate_node();
				x->data = it.node_->data;
				(void)_insert(x);
			}
		}
		void _set_by_move(Map && other)
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