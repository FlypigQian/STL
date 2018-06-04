#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>

/* += -= 的返回类型应不应该是引用呢？ */
namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template<typename T>
class vector {
protected:
	T *head;
	size_t logicLen, allocLen;
	void doubleSpace() {
		T *tmp = head;
		allocLen *= 2;
		head = (T *)malloc(allocLen * sizeof(T));
		T *curPtr = &head[0];
		for(int i = 0; i < logicLen; ++i){
			new (curPtr++) T(tmp[i]);
			tmp[i].~T();
		}
		free(tmp);
	}
public:
	/**
	 * TODO
	 * a type for actions of the elements of a vector, and you should write
	 *   a class named const_iterator with same interfaces.
	 */

	/**
	 * you can see RandomAccessIterator at CppReference for help.
	 */

	/***/
	class const_iterator;
	class iterator {
		friend const_iterator;
	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */

		 /* An iterator is implemented as a pointer to T. */
		T *ptr;
		const T *headAddress;
	public:
		/* The iterator is constructed by pointer, which takes the advantage of
		 * pointer arithmetic. */
		iterator(T *p = nullptr, const T *headPtr = nullptr): ptr(p), headAddress(headPtr) {}

		iterator(const iterator &other):ptr(other.ptr), headAddress(other.headAddress){}
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, just return the answer.
		 * as well as operator-
		 */
		/* Increment is done by pointer arithmetic, and the iterator is
		 * constructed by the single-parameter constructor. */
		iterator operator+(const int &n) const {
			//TODO
			return iterator(ptr + n, headAddress);
		}
		iterator operator-(const int &n) const {
			//TODO
			return iterator(ptr - n, headAddress);
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.

		/* Pointer arithmetic gives both positive and negative value.
		 * There's no unsigned issue. */
		int operator-(const iterator &rhs) const {
			//TODO

			if(headAddress == nullptr ||
					rhs.headAddress == nullptr ||
					headAddress != rhs.headAddress)
				throw invalid_iterator();


			return ptr - rhs.ptr;
		}
		iterator operator+=(const int &n) {
			//TODO
			ptr += n;
			return *this;
		}
		iterator operator-=(const int &n) {
			//TODO
			ptr -= n;
			return *this;
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
			iterator tmp(*this);
			++ptr;
			return tmp;
		}
		/**
		 * TODO ++iter
		 */
		iterator& operator++() {
			++ptr;
			return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
			iterator tmp(*this);
			ptr--;
			return tmp;
		}
		/**
		 * TODO --iter
		 */
		iterator& operator--() {
			--ptr;
			return *this;
		}
		/**
		 * TODO *it
		 */
		T& operator*() const{
			return *ptr;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const { return ptr == rhs.ptr; }
		bool operator==(const const_iterator &rhs) const { return ptr == rhs.const_ptr; }
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const { return ptr != rhs.ptr; }
		bool operator!=(const const_iterator &rhs) const { return ptr != rhs.const_ptr; }
	};
	/**
	 * TODO
	 * has same function as iterator, just for a const object.
	 */
	class const_iterator {
		friend iterator;
	private:
		const T *const_ptr;
		const T *headAddress;

	public:
		/* The iterator is constructed by pointer, which takes the advantage of
		 * pointer arithmetic. */
		const_iterator(const T *p = nullptr, const T *headPtr = nullptr): const_ptr(p), headAddress(headPtr) {}
		const_iterator(const const_iterator &other):const_ptr(other.const_ptr), headAddress(other.headAddress){}
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, just return the answer.
		 * as well as operator-
		 */
		/* Increment is done by pointer arithmetic, and the iterator is
		 * constructed by the single-parameter constructor. */
		const_iterator operator+(const int &n) const {
			//TODO
			return const_iterator(const_ptr + n, headAddress);
		}
		const_iterator operator-(const int &n) const {
			//TODO
			return const_iterator(const_ptr - n, headAddress);
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.

		/* Pointer arithmetic gives both positive and negative value.
		 * There's no unsigned issue. */
		int operator-(const const_iterator &rhs) const {
			//TODO
			if(headAddress == nullptr ||
			   rhs.headAddress == nullptr ||
			   headAddress != rhs.headAddress)
				throw invalid_iterator();

			return const_ptr - rhs.const_ptr;
		}
		const_iterator operator+=(const int &n) {
			//TODO
			const_ptr += n;
			return *this;
		}
		const_iterator operator-=(const int &n) {
			//TODO
			const_ptr -= n;
			return *this;
		}
		/**
		 * TODO iter++
		 */
		const_iterator operator++(int) {
			iterator tmp(*this);
			const_ptr++;
			return tmp;
		}
		/**
		 * TODO ++iter
		 */
		const_iterator& operator++() {
			++const_ptr;
			return *this;
		}
		/**
		 * TODO iter--
		 */
		const_iterator operator--(int) {
			iterator tmp(*this);
			const_ptr--;
			return tmp;
		}
		/**
		 * TODO --iter
		 */
		const_iterator& operator--() {
			--const_ptr;
			return *this;
		}
		/**
		 * TODO *it
		 */
		const T& operator*() const{
			return *const_ptr;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const { return const_ptr == rhs.ptr; }
		bool operator==(const const_iterator &rhs) const { return const_ptr == rhs.const_ptr; }
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const { return const_ptr != rhs.ptr; }
		bool operator!=(const const_iterator &rhs) const { return const_ptr != rhs.const_ptr; }
	};
	/**
	 * TODO Constructs
	 * Atleast three: default constructor, copy constructor and a constructor for std::vector
	 */
	vector(size_t size = 10) {
		head = (T *)malloc(size * sizeof(T));
		logicLen = 0;
		allocLen = size;
	}
	vector(const vector &other) {
		head = (T *)malloc(other.allocLen * sizeof(T));
		for(int i = 0; i < other.logicLen; ++i)
			head[i] = other.head[i];
		allocLen = other.allocLen;
		logicLen = other.logicLen;
	}
	/**
	 * TODO Destructor
	 */
	~vector() {
		clear();
		free(head);
	}
	/**
	 * TODO Assignment operator
	 */
	vector &operator=(const vector &other) {
		/* Prevent assign to itself. */
		if(this == &other)
			return *this;

		clear();
		free(head);
		head = (T *)malloc(other.allocLen * sizeof(T));
		for(int i = 0; i < other.logicLen; ++i)
			head[i] = other.head[i];
		allocLen = other.allocLen;
		logicLen = other.logicLen;
		return *this;
	}
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 */
	T & at(const size_t &pos) {
		if(pos < 0 || pos >= logicLen)
			throw index_out_of_bound();

		return head[pos];
	}
	const T & at(const size_t &pos) const {
		if(pos < 0 || pos >= logicLen)
			throw index_out_of_bound();

		return head[pos];
	}
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 * !!! Pay attentions
	 *   In STL this operator does not check the boundary but I want you to do.
	 */
	T & operator[](const size_t &pos) {
		if(pos < 0 || pos >= logicLen)
			throw index_out_of_bound();

		return head[pos];
	}
	const T & operator[](const size_t &pos) const {
		if(pos < 0 || pos >= logicLen)
			throw index_out_of_bound();

		return head[pos];
	}
	/**
	 * access the first element.
	 * throw container_is_empty if size == 0
	 */
	const T & front() const {
		if(logicLen == 0)
			throw container_is_empty();

		return head[0];
	}
	/**
	 * access the last element.
	 * throw container_is_empty if size == 0
	 */
	const T & back() const {
		if(logicLen == 0)
			throw container_is_empty();

		return head[logicLen - 1];
	}
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() { return iterator(&head[0], head); }
	const_iterator cbegin() const { return const_iterator(&head[0], head); }
	/**
	 * returns an iterator to the end.
	 */
	iterator end() { return iterator(&head[logicLen], head); }
	const_iterator cend() const { return const_iterator(&head[logicLen], head); }
	/**
	 * checks whether the container is empty
	 */
	bool empty() const { return logicLen == 0; }
	/**
	 * returns the number of elements
	 */
	size_t size() const { return logicLen; }
	/**
	 * returns the number of elements that can be held in currently allocated storage.
	 */
	size_t capacity() const { return allocLen; }
	/**
	 * clears the contents
	 */
	void clear() {
		for(int i = 0; i < logicLen; ++i)
			head[i].~T();
		logicLen = 0;
	}
	/**
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value.
	 */
	iterator insert(iterator pos, const T &value) {
		/* Index version of insertion, for practice
		 * I wanna write a iterator version, too. */

		int posIdx = pos - begin();
		if(posIdx < 0 || posIdx > logicLen)
			throw index_out_of_bound();

		return insert(posIdx, value);
		/*
		if(allocLen <= logicLen + 1)
			doubleSpace();

		int posIdx = pos - begin();
		if(posIdx < 0 || posIdx > logicLen)
			throw index_out_of_bound();

		iterator itr = end();
		while(itr != pos){
			*itr = *(itr - 1);
			--itr;
		}

		*pos = value;
		++logicLen;

		return pos;
		 */
	}
	/**
	 * inserts value at index ind.
	 * after inserting, this->at(ind) == value is true
	 * returns an iterator pointing to the inserted value.
	 * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
	 */
	iterator insert(const size_t &ind, const T &value) {
		/** Because end() is given by one hop behind the last element. */
		if(allocLen <= logicLen + 1)
			doubleSpace();

		/** size_t type ensures that ind cannot be negative. */
		if(ind > logicLen)
			throw index_out_of_bound();


		// 确保为未构造对象的内存构造对象。
		T * endPtr = &head[logicLen];
		new (endPtr) T(value);
		size_t cur = logicLen;
		while(cur != ind){
			head[cur] = head[cur-1];
			--cur;
		}

		head[ind] = value;
        ++logicLen;

		return begin() + ind;
	}
	/**
	 * removes the element at pos.
	 * return an iterator pointing to the following element.
	 * If the iterator pos refers the last element, the end() iterator is returned.
	 */
	iterator erase(iterator pos) {
		int posIdx = pos - begin();
		if(posIdx < 0 || posIdx >= logicLen)
			throw index_out_of_bound();

		return erase(posIdx);
		/*
		int posIdx = pos - begin();
		if(posIdx < 0 || posIdx > logicLen)
			throw index_out_of_bound();

		iterator posCopy = pos;
		while(posCopy+1 != end()){
			*posCopy = *(posCopy + 1);
			++posCopy;
		}

		// Now posCopy points to the last element.
		(*posCopy).~T();
		--logicLen;
		return pos;
		 */
	}
	/**
	 * removes the element with index ind.
	 * return an iterator pointing to the following element.
	 * throw index_out_of_bound if ind >= size
	 */
	iterator erase(const size_t &ind) {
		if(ind >= logicLen)
			throw index_out_of_bound();

		for(int i = ind + 1; i < logicLen; ++i)
			head[i-1] = head[i];

		head[logicLen-1].~T();
		--logicLen;

		return begin() + ind;
	}
	/**
	 * adds an element to the end.
	 */
	void push_back(const T &value) {
		if(allocLen <= logicLen + 1)
			doubleSpace();

		insert(end(), value);
	}
	/**
	 * remove the last element from the end.
	 * throw container_is_empty if size() == 0
	 */
	void pop_back() {
		if(size() == 0)
			throw container_is_empty();

		erase(logicLen - 1);
	}
};
}

#endif
