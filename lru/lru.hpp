#ifndef SJTU_LRU_HPP
#define SJTU_LRU_HPP

#include "class-integer.hpp"
#include "class-matrix.hpp"
#include "exceptions.hpp"
#include "utility.hpp"

class Hash {
public:
    unsigned int operator()(Integer lhs) const {
        int val = lhs.val;
        return std::hash<int>()(val);
    }
};

class Equal {
public:
    bool operator()(const Integer &lhs, const Integer &rhs) const { return lhs.val == rhs.val; }
};

namespace sjtu {
template<class T>
class double_list {
private:
	struct Node
	{
		T data;
		Node* prev;
		Node* next;
		Node(const T& val) : data(val), prev(nullptr), next(nullptr) {}
	}

	Node* head;
	Node* tail;
	size_t size;
public:
	/**
	 * elements
	 * add whatever you want
	 */

	// --------------------------
	/**
	 * the follows are constructors and destructors
	 * you can also add some if needed.
	 */
	double_list() : head(nullptr), tail(nullptr), size(0) {}
	double_list(const double_list<T> &other) : head(nullptr), tail(nullptr), size(0)
	{
		Node* pos = other.head;
		while (pos != nullptr)
		{
			insert_tail(pos->data);
			pos = pos->next;
		}
	}
	~double_list() {
		clear();
	}
	double_list& operator=(const double_list& other)
	{
		if (this != &other)
		{
			clear();
			Node* pos = other.head;
			while (pos != nullptr)
			{
				insert_tail(pos->data);
				pos = pos->next;
			}
		}
		return *this;
	}

	class iterator {
	private:
		friend class double_list;
		Node* iter;
		double_list* list;
	public:
		/**
		 * elements
		 * add whatever you want
		 */
		// --------------------------
		/**
		 * the follows are constructors and destructors
		 * you can also add some if needed.
		 */
		iterator(Node* it = nullptr, double_list* l = nullptr) : iter(it), list(l) {}
		iterator(const iterator &t) : iter(t.iter), list(t.list) {}
		~iterator() = default;
		
		/**
		 * iter++
		 */
		iterator operator++(int) {
			if (iter == nullptr) throw("invalid");
			iterator tmp = *this;
			iter = iter->next;
			return tmp;
		}
		/**
		 * ++iter
		 */
		iterator &operator++() {
			if (iter == nullptr) throw("invalid");
			iter = iter->next;
			return *this;
		}
		/**
		 * iter--
		 */
		iterator operator--(int) {
			if (iter == nullptr)
			{
				if (list == nullptr || list->tail == nullptr) throw("invalid");
				iterator tmp = *this;
				iter = list->tail;
				return tmp;
			}
			else
			{
				if (iter->prev == nullptr) throw("invalid");
				iterator tmp = *this;
				iter = iter->prev;
				return tmp;
			}
		}
		/**
		 * --iter
		 */
		iterator &operator--() {
			if (iter == nullptr)
			{
				if (list == nullptr || list->tail == nullptr) throw("invalid");
				iter = list->tail;
			}
			else
			{
				if (iter->prev == nullptr) throw("invalid");
				iter = iter->prev;
			}
			return *this;
		}
		
		/**
		 * if the iter didn't point to a value
		 * throw " invalid"
		 */
		T &operator*() const {
			if (iter == nullptr) throw("invalid");
			return iter->data;
		}
		
		/**
		 * other operation
		 */
		T *operator->() const noexcept {
			return &(iter->data);
		}
		bool operator==(const iterator &rhs) const {
			return iter == rhs.iter && list == rhs.list;
		}
		bool operator!=(const iterator &rhs) const {
			return !(*this == rhs);
		}
	};
	/**
	 * return an iterator to the beginning
	 */
	iterator begin() {
		return iterator(head, this);
	}
	/**
	 * return an iterator to the ending
	 * in fact, it returns the iterator point to nothing,
	 * just after the last element.
	 */
	iterator end() {
		return iterator(nullptr, this);
	}
	/**
	 * if the iter didn't point to anything, do nothing,
	 * otherwise, delete the element pointed by the iter
	 * and return the iterator point at the same "index"
	 * e.g.
	 * 	if the origin iterator point at the 2nd element
	 * 	the returned iterator also point at the
	 *  2nd element of the list after the operation
	 *  or nothing if the list after the operation
	 *  don't contain 2nd elememt.
	 */
	iterator erase(iterator pos) {
		Node* iter = pos.iter;
		if (iter == nullptr || pos.list != this) return pos;
		Node* nextNode = iter->next;
		
		if (iter->prev != nullptr) iter->prev->next = iter->next;
		else head = iter->next;
		if (iter->next != nullptr) iter->next->prev = iter->prev;
		else tail = iter->prev;

		delete iter;
		size--;
		return iterator(nextNode, this);
	}

	void clear()
	{
		while (head != nullptr)
		{
			Node* tmp = head;
			head = head->next;
			delete tmp;
		}
		tail = nullptr;
		size = 0;
	}

	/**
	 * the following are operations of double list
	 */
	void insert_head(const T &val) {
		Node* newNode = new Node(val);
		newNode->next = head;
		if (head != nullptr) head->prev = newNode;
		else tail = newNode;
		head = newNode;
		size++;
	}
	void insert_tail(const T &val) {
		Node* newNode = new Node(val);
		newNode->prev = tail;
		if (tail != nullptr) tail->next = newNode;
		else head = newNode;
		tail = newNode;
		size++;
	}
	void delete_head() {
		if (head == nullptr) return;
		Node* oldNode = head;
		head = head->next;
		if (head != nullptr) head->prev = nullptr;
		else tail = nullptr;
		delete oldNode;
		size--;
	}
	void delete_tail() {
		if (tail == nullptr) return;
		Node* oldNode = tail;
		tail = tail->prev;
		if (tail != nullptr) tail->next = nullptr;
		else head = nullptr;
		delete oldNode;
		size--;
	}
	/**
	 * if didn't contain anything, return true,
	 * otherwise false.
	 */
	bool empty() {
		return size == 0;
	}
};

template<class Key, class T, class Hash = std::hash<Key>, class Equal = std::equal_to<Key>>
class hashmap {
public:
	using value_type = pair<const Key, T>;
	/**
	 * elements
	 * add whatever you want
	 */

	// --------------------------

	/**
	 * the follows are constructors and destructors
	 * you can also add some if needed.
	 */
	hashmap() {}
	hashmap(const hashmap &other) {}
	~hashmap() {}
	hashmap &operator=(const hashmap &other) {}

	class iterator {
	public:
		/**
		 * elements
		 * add whatever you want
		 */
		// --------------------------
		/**
		 * the follows are constructors and destructors
		 * you can also add some if needed.
		 */
		iterator() {}
		iterator(const iterator &t) {}
		~iterator() {}

		/**
		 * if point to nothing
		 * throw
		 */
		value_type &operator*() const {}

		/**
		 * other operation
		 */
		value_type *operator->() const noexcept {}
		bool operator==(const iterator &rhs) const {}
		bool operator!=(const iterator &rhs) const {}
	};

	void clear() {}
	/**
	 * you need to expand the hashmap dynamically
	 */
	void expand() {}

	/**
	 * the iterator point at nothing
	 */
	iterator end() const {}
	/**
	 * find, return a pointer point to the value
	 * not find, return the end (point to nothing)
	 */
	iterator find(const Key &key) const {}
	/**
	 * already have a value_pair with the same key
	 * -> just update the value, return false
	 * not find a value_pair with the same key
	 * -> insert the value_pair, return true
	 */
	sjtu::pair<iterator, bool> insert(const value_type &value_pair) {}
	/**
	 * the value_pair exists, remove and return true
	 * otherwise, return false
	 */
	bool remove(const Key &key) {}
};

template<class Key, class T, class Hash = std::hash<Key>, class Equal = std::equal_to<Key>>
class linked_hashmap : public hashmap<Key, T, Hash, Equal> {
public:
	typedef pair<const Key, T> value_type;
	/**
	 * elements
	 * add whatever you want
	 */
	// --------------------------
	class const_iterator;
	class iterator {
	public:
		/**
		 * elements
		 * add whatever you want
		 */
		// --------------------------
		iterator() {}
		iterator(const iterator &other) {}
		~iterator() {}

		/**
		 * iter++
		 */
		iterator operator++(int) {}
		/**
		 * ++iter
		 */
		iterator &operator++() {}
		/**
		 * iter--
		 */
		iterator operator--(int) {}
		/**
		 * --iter
		 */
		iterator &operator--() {}

		/**
		 * if the iter didn't point to a value
		 * throw "star invalid"
		 */
		value_type &operator*() const {}
		value_type *operator->() const noexcept {}

		/**
		 * operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {}
		bool operator!=(const iterator &rhs) const {}
		bool operator==(const const_iterator &rhs) const {}
		bool operator!=(const const_iterator &rhs) const {}
	};

	class const_iterator {
	public:
		/**
		 * elements
		 * add whatever you want
		 */
		// --------------------------
		const_iterator() {}
		const_iterator(const iterator &other) {}

		/**
		 * iter++
		 */
		const_iterator operator++(int) {}
		/**
		 * ++iter
		 */
		const_iterator &operator++() {}
		/**
		 * iter--
		 */
		const_iterator operator--(int) {}
		/**
		 * --iter
		 */
		const_iterator &operator--() {}

		/**
		 * if the iter didn't point to a value
		 * throw
		 */
		const value_type &operator*() const {}
		const value_type *operator->() const noexcept {}

		/**
		 * operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {}
		bool operator!=(const iterator &rhs) const {}
		bool operator==(const const_iterator &rhs) const {}
		bool operator!=(const const_iterator &rhs) const {}
	};

	linked_hashmap() {}
	linked_hashmap(const linked_hashmap &other) {}
	~linked_hashmap() {}
	linked_hashmap &operator=(const linked_hashmap &other) {}

	/**
	 * return the value connected with the Key(O(1))
	 * if the key not found, throw
	 */
	T &at(const Key &key) {}
	const T &at(const Key &key) const {}
	T &operator[](const Key &key) {}
	const T &operator[](const Key &key) const {}

	/**
	 * return an iterator point to the first
	 * inserted and existed element
	 */
	iterator begin() {}
	const_iterator cbegin() const {}
	/**
	 * return an iterator after the last inserted element
	 */
	iterator end() {}
	const_iterator cend() const {}
	/**
	 * if didn't contain anything, return true,
	 * otherwise false.
	 */
	bool empty() const {}

	void clear() {}

	size_t size() const {}
	/**
	 * insert the value_piar
	 * if the key of the value_pair exists in the map
	 * update the value instead of adding a new element，
	 * then the order of the element moved from inner of the
	 * list to the head of the list
	 * and return false
	 * if the key of the value_pair doesn't exist in the map
	 * add a new element and return true
	 */
	pair<iterator, bool> insert(const value_type &value) {}
	/**
	 * erase the value_pair pointed by the iterator
	 * if the iterator points to nothing
	 * throw
	 */
	void remove(iterator pos) {}
	/**
	 * return how many value_pairs consist of key
	 * this should only return 0 or 1
	 */
	size_t count(const Key &key) const {}
	/**
	 * find the iterator points at the value_pair
	 * which consist of key
	 * if not find, return the iterator
	 * point at nothing
	 */
	iterator find(const Key &key) {}
};

class lru {
	using lmap = sjtu::linked_hashmap<Integer, Matrix<int>, Hash, Equal>;
	using value_type = sjtu::pair<const Integer, Matrix<int>>;

public:
	lru(int size) {}
	~lru() {}
	/**
	 * save the value_pair in the memory
	 * delete something in the memory if necessary
	 */
	void save(const value_type &v) const {}
	/**
	 * return a pointer contain the value
	 */
	Matrix<int> *get(const Integer &v) const {}
	/**
	 * just print everything in the memory
	 * to debug or test.
	 * this operation follows the order, but don't
	 * change the order.
	 */
	void print() {}
};
} // namespace sjtu

#endif
