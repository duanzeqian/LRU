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
	};

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
	size_t siz() const
	{
		return size;
	}

	bool empty() {
		return size == 0;
	}
};

template<class Key, class T, class Hash = std::hash<Key>, class Equal = std::equal_to<Key>>
class hashmap {
public:
	using value_type = pair<const Key, T>;

private:
	/**
	 * elements
	 * add whatever you want
	 */
	std::vector< double_list<value_type> > buckets;
    size_t bucket_cnt;
    size_t size;
    float factor;
    Hash hasher;
    Equal equal;
	 void swap(hashmap& other)
	 {
        std::swap(buckets, other.buckets);
        std::swap(bucket_cnt, other.bucket_cnt);
        std::swap(size, other.size);
        std::swap(factor, other.factor);
        std::swap(hasher, other.hasher);
        std::swap(equal, other.equal);
    }

public:
	// --------------------------

	/**
	 * the follows are constructors and destructors
	 * you can also add some if needed.
	 */
	hashmap() : bucket_cnt(16), size(0), factor(0.75f)
	{
        buckets.resize(bucket_cnt);
    }
	hashmap(const hashmap &other) : bucket_cnt(other.bucket_cnt), size(other.size), factor(other.factor), hasher(other.hasher), equal(other.equal) {
		buckets.resize(bucket_cnt);
		for (int i = 0; i < bucket_cnt; ++i)
		{
			buckets[i] = other.buckets[i];
		}
	}
	~hashmap()  = default;
	hashmap &operator=(const hashmap &other) {
		if (this != &other)
		{
            hashmap tmp(other);
            swap(tmp);
        }
        return *this;
	}

	class iterator {
	private:
        friend class hashmap;
        hashmap* map;
        size_t idx;
        typename double_list<value_type>::iterator iter;
	
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
		iterator() : map(nullptr), idx(0), iter() {}
		iterator(hashmap* m, size_t idx, typename double_list<value_type>::iterator it)
            : map(m), idx(idx), iter(it) {}
		iterator(const iterator &t) : map(t.map), idx(t.idx), iter(t.iter) {}
		~iterator() = default;

		/**
		 * if point to nothing
		 * throw
		 */
		value_type &operator*() const {
			if (map == nullptr || idx >= map->bucket_cnt || iter == map->buckets[idx].end())
                throw("invalid");
            return *iter;
		}

		/**
		 * other operation
		 */
		value_type *operator->() const noexcept {
			return &(*iter);
		}
		bool operator==(const iterator &rhs) const {
			if (map != rhs.map) return false;
            if (idx != rhs.idx) return false;
            if (idx == map->bucket_cnt) return true; // index of end()
            return iter == rhs.iter;
		}
		bool operator!=(const iterator &rhs) const {
			return !(*this == rhs);
		}
	};

	void clear() {
		hashmap empty;
		swap(empty);
	}
	/**
	 * you need to expand the hashmap dynamically
	 */
	void expand() {
		size_t new_cnt = bucket_cnt << 1;
		if (new_cnt == 0) new_cnt = 1;
		std::vector< double_list<value_type> > new_buckets(new_cnt);

		for (int i = 0; i < bucket_cnt; ++i)
		{
			auto& bucket = buckets[i];
			auto it = bucket.begin();
            while (it != bucket.end())
			{
                new_buckets[hasher(it->first) % new_cnt].insert_tail(*it);
            	it = buckets[i].erase(it);
            }
		}
		buckets.swap(new_buckets);
		bucket_cnt = new_cnt;
	}

	/**
	 * the iterator point at nothing
	 */
	iterator end() const {
		return iterator(const_cast<hashmap*>(this), bucket_cnt, {});
	}
	/**
	 * find, return a pointer point to the value
	 * not find, return the end (point to nothing)
	 */
	iterator find(const Key &key) const {
		size_t idx = hasher(key) % bucket_cnt;
        auto& bucket = const_cast<double_list<value_type>&>(buckets[idx]);
        for (auto it = bucket.begin(); it != bucket.end(); ++it)
		{
            if (equal(it->first, key))
                return iterator(const_cast<hashmap*>(this), idx, it);
        }
        return end();
	}
	/**
	 * already have a value_pair with the same key
	 * -> just update the value, return false
	 * not find a value_pair with the same key
	 * -> insert the value_pair, return true
	 */
	sjtu::pair<iterator, bool> insert(const value_type &value_pair) {
		const Key& key = value_pair.first;
        size_t idx = hasher(key) % bucket_cnt;
        auto& bucket = buckets[idx];

        for (auto it = bucket.begin(); it != bucket.end(); ++it)
		{
            if (equal(it->first, key))
			{
                it->second = value_pair.second;
                return {iterator(this, idx, it), false};
            }
        }

        if (size + 1 > bucket_cnt * factor) 
		{
            expand();
            idx = hasher(key) % bucket_cnt; // new index in expanded buckets
        }

		auto& new_bucket = buckets[idx];
        new_bucket.insert_tail(value_pair);
        size++;
        auto it = --new_bucket.end();
        return {iterator(this, idx, it), true};
	}
	/**
	 * the value_pair exists, remove and return true
	 * otherwise, return false
	 */
	bool remove(const Key &key) {
		size_t idx = hasher(key) % bucket_cnt;
        auto& bucket = buckets[idx];

		for (auto it = bucket.begin(); it != bucket.end(); ++it)
		{
            if (equal(it->first, key))
			{
                bucket.erase(it);
				size--;
                return true;
            }
        }
		return false;
	}

	bool empty()
	{
		return size == 0;
	}
};

template<class Key, class T, class Hash = std::hash<Key>, class Equal = std::equal_to<Key>>
class linked_hashmap : public hashmap<Key, T, Hash, Equal> {
public:
	typedef pair<const Key, T> value_type;

private:
	double_list<value_type> list;
    using order_iterator = typename double_list<value_type>::iterator;
    hashmap<Key, order_iterator, Hash, Equal> map;
	/**
	 * elements
	 * add whatever you want
	 */

public:
	// --------------------------
	class const_iterator;
	class iterator {
	private:
		friend class linked_hashmap;
        order_iterator iter;
	
	public:
		// --------------------------
		iterator() = default;
		iterator(const order_iterator& it) : iter(it) {}
		iterator(const iterator &other) : iter(other.iter) {}
		~iterator() = default;

		/**
		 * iter++
		 */
		iterator operator++(int) {
			iterator tmp = *this;
			iter++;
			return tmp;
		}
		/**
		 * ++iter
		 */
		iterator &operator++() {
			iter++;
			return *this;
		}
		/**
		 * iter--
		 */
		iterator operator--(int) {
			iterator tmp = *this;
			iter--;
			return tmp;
		}
		/**
		 * --iter
		 */
		iterator &operator--() {
			iter--;
			return *this;
		}

		/**
		 * if the iter didn't point to a value
		 * throw "star invalid"
		 */
		value_type &operator*() const {
			try
			{
				return *iter;
			}
			catch(const char*)
			{
				throw("star invalid");
			}
		}
		value_type *operator->() const noexcept {
			return &(*iter);
		}

		/**
		 * operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const { return iter == rhs.iter; }
		bool operator!=(const iterator &rhs) const { return iter != rhs.iter; }
		bool operator==(const const_iterator &rhs) const { return iter == rhs.iter; }
		bool operator!=(const const_iterator &rhs) const { return iter != rhs.iter; }
	};

	class const_iterator {
	private:
		friend class linked_hashmap;
        order_iterator iter; 
	
	public:
		// --------------------------
		const_iterator() = default;
        const_iterator(const order_iterator& it) : iter(it) {}
		const_iterator(const iterator &other) : iter(other.iter) {}
		~const_iterator() = default;

		/**
		 * iter++
		 */
		const_iterator operator++(int) {
			const_iterator tmp = *this;
			iter++;
			return tmp;
		}
		/**
		 * ++iter
		 */
		const_iterator &operator++() {
			iter++;
			return *this;
		}
		/**
		 * iter--
		 */
		const_iterator operator--(int) {
			const_iterator tmp = *this;
			iter--;
			return tmp;
		}
		/**
		 * --iter
		 */
		const_iterator &operator--() {
			iter--;
			return *this;
		}

		/**
		 * if the iter didn't point to a value
		 * throw
		 */
		const value_type &operator*() const {
			try
			{
                return *iter;
            }
			catch (const char*)
			{
                throw("star invalid");
            }
		}
		const value_type *operator->() const noexcept {
			return &(*iter);
		}

		/**
		 * operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const { return iter == rhs.iter; }
		bool operator!=(const iterator &rhs) const { return iter != rhs.iter; }
		bool operator==(const const_iterator &rhs) const { return iter == rhs.iter; }
		bool operator!=(const const_iterator &rhs) const { return iter != rhs.iter; }
	};

	linked_hashmap() = default;
	linked_hashmap(const linked_hashmap &other) {
		list = other.list; // copy order_list
        auto it = list.begin(); // copy hash_map (in the order of order_list)
        while (it != list.end())
		{
            map.insert(sjtu::pair<const Key, order_iterator>(it->first, it));
            ++it;
        }
	}
	~linked_hashmap() = default;
	linked_hashmap &operator=(const linked_hashmap &other) {
		if (this != &other)
		{
            list = other.list;
			map.clear();
			auto it = list.begin();
			while (it != list.end())
			{
				map.insert(sjtu::pair<const Key, order_iterator>(it->first, it));
				++it;
			}
        }
        return *this;
	}

	/**
	 * return the value connected with the Key(O(1))
	 * if the key not found, throw
	 */
	T &at(const Key &key) {
		auto it = map.find(key);
        if (it == map.end()) throw("key not found");

        order_iterator order_iter = it->second; // touched
        value_type val = *order_iter;
        list.erase(order_iter);
        list.insert_head(val);
        auto new_it = list.begin();
        map.insert(sjtu::pair<const Key, order_iterator>(key, new_it));
        return new_it->second;
	}
	const T &at(const Key &key) const {
		auto it = map.find(key);
        if (it == map.end()) throw("key not found");

        return it->second->second;
	}
	T &operator[](const Key &key) {
		return at(key);
	}
	const T &operator[](const Key &key) const {
		return at(key); // the same as const at
	}

	/**
	 * return an iterator point to the first
	 * inserted and existed element
	 */
	iterator begin() {
		return iterator(list.begin());
	}
	const_iterator cbegin() const {
		return const_iterator(const_cast<double_list<value_type>&>(list).begin());
	}
	/**
	 * return an iterator after the last inserted element
	 */
	iterator end() {
		return iterator(list.end());
	}
	const_iterator cend() const {
		return const_iterator(const_cast<double_list<value_type>&>(list).end());
	}
	/**
	 * if didn't contain anything, return true,
	 * otherwise false.
	 */
	bool empty() const {
		return list.empty();
	}

	void clear() {
		list.clear();
		map.clear();
	}

	size_t size() const {
		return list.siz();
	}
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
	pair<iterator, bool> insert(const value_type &value) {
		const Key& key = value.first;
        auto it = map.find(key);
        if (it != map.end()) // has this key in hash_map
		{
            order_iterator order_iter = it->second;
            order_iter->second = value.second;
            value_type val = *order_iter;
            list.erase(order_iter); // delete the old
            list.insert_head(val); // insert the latest
            auto new_it = list.begin();
            map.insert(sjtu::pair<const Key, order_iterator>(key, new_it));
            return sjtu::pair<iterator, bool>(iterator(new_it), false);
        }
		else
		{
            list.insert_head(value);
            auto new_it = list.begin();
            map.insert(sjtu::pair<const Key, order_iterator>(key, new_it));
            return sjtu::pair<iterator, bool>(iterator(new_it), true);
        }
	}
	/**
	 * erase the value_pair pointed by the iterator
	 * if the iterator points to nothing
	 * throw
	 */
	void remove(iterator pos) {
		if (pos == end()) throw("invalid");
		map.remove(pos->first);
		list.erase(pos.iter);
	}
	/**
	 * return how many value_pairs consist of key
	 * this should only return 0 or 1
	 */
	size_t count(const Key &key) const {
		return map.find(key) != map.end();
	}
	/**
	 * find the iterator points at the value_pair
	 * which consist of key
	 * if not find, return the iterator
	 * point at nothing
	 */
	iterator find(const Key &key) {
		auto it = map.find(key);
		if (it == map.end()) return end();
		return iterator(it->second);
	}
};

class lru {
	using lmap = sjtu::linked_hashmap<Integer, Matrix<int>, Hash, Equal>;
	using value_type = sjtu::pair<const Integer, Matrix<int>>;
	using liter = lmap::iterator;

private:
	lmap* map;
	size_t size;

public:
	lru(int size) :size(size) {
		map = new lmap;
	}
	~lru() {
		delete map;
	}
	/**
	 * save the value_pair in the memory
	 * delete something in the memory if necessary
	 */
	void save(const value_type &v) const {
		if (map->find(v.first) == map->end() && map->size() == size) // 不存在且已满 
		{
			liter it = map->begin();
			map->remove(it);
		}
		map->insert(v);
	}
	/**
	 * return a pointer contain the value
	 */
	Matrix<int> *get(const Integer &v) const {
		liter it = map->find(v);
		if (it == map->end()) return (Matrix<int>*)nullptr;
		else
		{
			Matrix<int>* tmp = new Matrix<int>((*it).second);
            map->insert(value_type(v, *tmp));
            return tmp;
		}
	}
	/**
	 * just print everything in the memory
	 * to debug or test.
	 * this operation follows the order, but don't
	 * change the order.
	 */
	void print() {
		for (liter it = map->begin(); it != map->end(); ++it)
		{
			std::cout << (*it).first.val << " " << (*it).second << std::endl;
		}
	}
};
} // namespace sjtu

#endif
