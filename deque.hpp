#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
#include <iostream>
#include <cstddef>

/** My deque is implemented by a block linked list.
 * The maintainance of a block linked list is utterly important for
 * a sustainable data structure.
 *
 * I decide to use dummy cell, just like doubly linked list.
 * I cannot pass the second test! */
namespace sjtu {
    
    class pop_empty_block{};
    
    template<class T>
    class deque {
    private:
        struct node{
            T *arr;
            size_t allocLen, logicLen;
            node *prev;
            node *next;
            
            node(size_t alloc, node *prev = nullptr, node *next = nullptr) {
                arr = (T *)malloc(alloc * sizeof(T));
                allocLen = alloc;
                logicLen = 0;
                this->prev = prev;
                this->next = next;
            }
            /** Copy constructor. */
            node(node *other, node *prev = nullptr, node *next = nullptr):
            prev(prev), next(next) {
                allocLen = other->allocLen;
                logicLen = other->logicLen;
                arr = (T *)malloc(allocLen * sizeof(T));
                
                T *tmp;
                for(int i = 0; i < logicLen; ++i){
                    tmp = &arr[i];
                    new (tmp) T(other->arr[i]);
                }
            }
            ~node(){
                free(arr);
            }
        };
        
        node *head, *tail;
        size_t len;
        size_t blockSize;
        size_t blockNum;
        int counter;
        
        const static int resizeConstant = 300;
        
        /**
         * This function is a utility for erase public member function.
         * It assures curNode has enough space to hold extra elements in
         * curNode->next.
         * It appends curNode->next's content to curNode, and delete
         * curNode->next as well as maintaining linked list structure. */
        void merge(node *curNode){
            // Simplify notation.
            size_t curNum = curNode->logicLen;
            size_t nextNum = curNode->next->logicLen;
            
            // Construct.
            T *tmp;
            for(int i = 0; i < nextNum; ++i){
                tmp = &curNode->arr[curNum+i];
                new (tmp) T(curNode->next->arr[i]);
                curNode->next->arr[i].~T();
            }
            
            // Invoke destructor manually.
            curNode->logicLen += nextNum;
            node *del = curNode->next;
            curNode->next->next->prev = curNode;
            curNode->next = curNode->next->next;
            
            delete del;
        }
        
        int mySqrt(size_t n){
            size_t i = 1;
            while(i * i < n)
                ++i;
            return i;
        }
        
    public:
        
        // Utility
        int length(){ return len; }
        
        class const_iterator;
        class iterator {
            friend deque;
            friend const_iterator;
        private:
            /**
             * TODO add data members
             *   just add whatever you want.
             */
            node *posNode;
            int posIdx;
            const node *headAddress;
        public:
            
            void log() {
                node *finder = posNode;
                int count = 0;
                while(finder->prev->prev != nullptr){
                    count++;
                    finder = finder->prev;
                }
                printf("Iterator in %dth index of %dth node. \n", posIdx, count);
            }
            /* Constructor. */
            iterator(){ posNode = nullptr; headAddress = nullptr; posIdx = 0;}
            iterator(node *n, int i, node *h): posNode(n), posIdx(i), headAddress(h) {}
            iterator(const iterator &other):
            posNode(other.posNode),
            posIdx(other.posIdx),
            headAddress(other.headAddress) {}
            /**
             * return a new iterator which pointer n-next elements
             *   even if there are not enough elements, the behaviour is **undefined**.
             * as well as operator-
             *
             * To keep consistency, we regulate that the range of iterator is limited
             * within begin() and end().
             */
            iterator operator+(const int &n) const {
                if(n < 0)
                    return operator-(-n);
                if(n == 0)
                    return *this;
                //TODO
                iterator itr(*this);
                /* If this is end() iterator, it can't be added. */
                if(itr.posNode->next == nullptr){
                    itr.posIdx = 0;
                    return itr;
                }
                
                
                /* Note that empty, head iterator case is included. */
                /* CumulativeIdx is where we wanna go. If we cannot
                 * go there because of limited block index, we subtract
                 * its logicLen from total index and move one hop next.
                 * If we move to the tail block, the behaviour is undefined. */
                int cumulativeIdx = itr.posIdx + n;
                while(cumulativeIdx > itr.posNode->logicLen - 1){
                    cumulativeIdx -= itr.posNode->logicLen;
                    itr.posNode = itr.posNode->next;
                    /** Break at tail node.
                     * Since posNode cannot be nullptr, this expression is valid. */
                    if(itr.posNode->next == nullptr){
                        itr.posIdx = 0;
                        return itr;
                    }
                }
                itr.posIdx = cumulativeIdx;
                return itr;
            }
            iterator operator-(const int &n) const {
                if(n < 0)
                    return operator+(-n);
                
                if(n == 0)
                    return *this;
                //TODO
                iterator itr(*this);
                /* If this is begin() iterator, it can be substracted.
                 * for the purpose of the test. */
                if(posNode->prev->prev == nullptr && posIdx == 0){
                    itr.posNode = itr.posNode->prev;
                    return itr;
                }
                
                int cumulativeIdx = itr.posIdx - n;
                while(cumulativeIdx < 0){
                    itr.posNode = itr.posNode->prev;
                    cumulativeIdx += itr.posNode->logicLen;
                    /** Break at head node. */
                    if(itr.posNode->prev == nullptr){
                        itr.posNode = itr.posNode->next;
                        itr.posIdx = 0;
                        return itr;
                    }
                }
                itr.posIdx = cumulativeIdx;
                return itr;
            }
            // return th distance between two iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
            // Add an additional minus sign because of test requirement.
            // Iterators formmer substracted by latter gives minus number.
            int operator-(const iterator &rhs) const {
                /** There is a lot to consider.
                 1. Invalid_iterator.*/
                
                if(headAddress == nullptr ||
                   rhs.headAddress == nullptr ||
                   headAddress != rhs.headAddress)
                    throw invalid_iterator();
                
                /** 2. If points to the same block. */
                if(posNode == rhs.posNode)
                    return posIdx - rhs.posIdx;
                
                /** 3. thisBeforeOther */
                bool thisBeforeOther = false;
                node *explorer = posNode->next;
                /* Explore untill hitting tail.
                 * Note that since we haven't imposed any restriction
                 * on explorer, it may be nullptr. */
                while(explorer != nullptr){
                    if(explorer == rhs.posNode)
                        thisBeforeOther = true;
                    explorer = explorer->next;
                }
                if(!thisBeforeOther)
                    return - (rhs - *this);
                
                /** 4. If this is head pointer, and empty list. */
                if(posNode->prev->prev == nullptr && posNode->logicLen == 0)
                    return 0;
                
                /** 5. If the other is end() iterator. */
                if(rhs.posNode->next == nullptr){
                    int dist = 0;
                    node *curNode = posNode;
                    while(curNode != rhs.posNode){
                        dist += curNode->logicLen;
                        curNode = curNode->next;
                    }
                    dist -= (posIdx + 1);
                    return -(dist + 1);
                }
                
                /** 6. Common case. */
                int dist = 0;
                node *curNode = posNode;
                while(curNode != rhs.posNode){
                    dist += curNode->logicLen;
                    curNode = curNode->next;
                }
                dist = dist - (posIdx + 1) + (rhs.posIdx + 1);
                return -dist;
            }
            iterator operator+=(const int &n) {
                //TODO
                *this = *this + n;
                return *this;
            }
            iterator operator-=(const int &n) {
                //TODO
                *this = *this - n;
                return *this;
            }
            /**
             * TODO iter++
             * If there's not enough elements, this operation is still valid.
             * But the result is undefined.
             */
            iterator operator++(int) {
                iterator tmp(*this);
                *this = *this + 1;
                return tmp;
            }
            /**
             * TODO ++iter
             */
            iterator& operator++() {
                *this = *this + 1;
                return *this;
            }
            /**
             * TODO iter--
             */
            iterator operator--(int) {
                iterator tmp(*this);
                *this = *this - 1;
                return tmp;
            }
            /**
             * TODO --iter
             */
            iterator& operator--() {
                *this = *this - 1;
                return *this;
            }
            /**
             * TODO *it
             */
            T& operator*() const {
                if(posNode->prev == nullptr || posNode->next == nullptr)
                    throw invalid_iterator();
                
                if(posNode->prev->prev == nullptr &&
                   posNode->logicLen == 0 &&
                   posIdx == 0)
                    throw invalid_iterator();
                
                return posNode->arr[posIdx];
            }
            /**
             * TODO it->field
             */
            T* operator->() const noexcept {
                T *ptr = &posNode->arr[posIdx];
                return ptr;
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             * Because substract can throw invalid_iterator exception, we catch it automatically
             * inside this function because we can simply handle that.
             */
            bool operator==(const iterator &rhs) const {
                try{
                    return (*this - rhs) == 0;
                }
                catch(invalid_iterator &ex){
                    return false;
                }
            }
            bool operator==(const const_iterator &rhs) const {
                try{
                    return (*this - rhs) == 0;
                }
                catch(invalid_iterator &ex){
                    return false;
                }
            }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                try{
                    return (*this - rhs) != 0;
                }
                catch(invalid_iterator &ex){
                    return false;
                }
            }
            bool operator!=(const const_iterator &rhs) const {
                try{
                    return (*this - rhs) != 0;
                }
                catch(invalid_iterator &ex){
                    return false;
                }
            }
        };
        class const_iterator {
            friend deque;
            friend iterator;
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
        private:
            // data members.
            const node *posNode;
            int posIdx;
            const node *headAddress;
            
        public:
            
            void log() {
                node *finder = posNode;
                int count = 0;
                while(finder->prev->prev != nullptr){
                    count++;
                    finder = finder->prev;
                }
                printf("Iterator in %dth index of %dth node. \n", posIdx, count);
            }
            /* Constructor. */
            const_iterator(){ posNode = nullptr; headAddress = nullptr; posIdx = 0;}
            const_iterator(node *n, int i, node *h): posNode(n), posIdx(i), headAddress(h) {}
            const_iterator(const iterator &other):posNode(other.posNode),posIdx(other.posIdx),
            headAddress(other.headAddress) {}
            /**
             * return a new iterator which pointer n-next elements
             *   even if there are not enough elements, the behaviour is **undefined**.
             * as well as operator-
             *
             * To keep consistency, we regulate that the range of iterator is limited
             * within begin() and end().
             */
            const_iterator operator+(const int &n) const {
                if(n < 0)
                    return operator-(-n);
                if(n == 0)
                    return *this;
                
                //TODO
                const_iterator itr(*this);
                /* If this is end() iterator, it can't be added. */
                if(itr.posNode->next == nullptr){
                    itr.posIdx = 0;
                    return itr;
                }
                
                
                /* Note that empty, head iterator case is included. */
                /* CumulativeIdx is where we wanna go. If we cannot
                 * go there because of limited block index, we subtract
                 * its logicLen from total index and move one hop next.
                 * If we move to the tail block, the behaviour is undefined. */
                int cumulativeIdx = itr.posIdx + n;
                while(cumulativeIdx > itr.posNode->logicLen - 1){
                    cumulativeIdx -= itr.posNode->logicLen;
                    itr.posNode = itr.posNode->next;
                    /** Break at tail node.
                     * Since posNode cannot be nullptr, this expression is valid. */
                    if(itr.posNode->next == nullptr){
                        itr.posIdx = 0;
                        return itr;
                    }
                }
                itr.posIdx = cumulativeIdx;
                return itr;
            }
            const_iterator operator-(const int &n) const {
                if(n < 0)
                    return operator+(-n);
                if(n == 0)
                    return *this;
                //TODO
                const_iterator itr(*this);
                /* If this is begin() iterator, it can be substracted.
                 * for the purpose of the test. */
                if(posNode->prev->prev == nullptr && posIdx == 0){
                    itr.posNode = itr.posNode->prev;
                    return itr;
                }
                
                
                int cumulativeIdx = itr.posIdx - n;
                while(cumulativeIdx < 0){
                    itr.posNode = itr.posNode->prev;
                    cumulativeIdx += itr.posNode->logicLen;
                    /** Break at head node. */
                    if(itr.posNode->prev == nullptr){
                        itr.posNode = itr.posNode->next;
                        itr.posIdx = 0;
                        return itr;
                    }
                }
                itr.posIdx = cumulativeIdx;
                return itr;
            }
            // return th distance between two iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
            int operator-(const const_iterator &rhs) const {
                /** There is a lot to consider.
                 1. Invalid_iterator. */
                if(headAddress == nullptr ||
                   rhs.headAddress == nullptr ||
                   headAddress != rhs.headAddress)
                    throw invalid_iterator();
                
                /** 2. If points to the same block. */
                if(posNode == rhs.posNode)
                    return posIdx - rhs.posIdx;
                
                /** 3. thisBeforeOther */
                bool thisBeforeOther = false;
                const node *explorer = posNode->next;
                /* Explore untill hitting tail.
                 * Note that since we haven't imposed any restriction
                 * on explorer, it may be nullptr. */
                while(explorer != nullptr){
                    if(explorer == rhs.posNode)
                        thisBeforeOther = true;
                    explorer = explorer->next;
                }
                if(!thisBeforeOther)
                    return - (rhs - *this);
                
                /** 4. If this is head pointer, and empty list. */
                if(posNode->prev->prev == nullptr && posNode->logicLen == 0)
                    return 0;
                
                /** 5. If the other is end() iterator. */
                if(rhs.posNode->next == nullptr){
                    int dist = 0;
                    const node *curNode = posNode;
                    while(curNode != rhs.posNode){
                        dist += curNode->logicLen;
                        curNode = curNode->next;
                    }
                    dist -= (posIdx + 1);
                    return -(dist + 1);
                }
                
                /** 6. Common case. */
                int dist = 0;
                const node *curNode = posNode;
                while(curNode != rhs.posNode){
                    dist += curNode->logicLen;
                    curNode = curNode->next;
                }
                dist = dist - (posIdx + 1) + (rhs.posIdx + 1);
                return -dist;
            }
            const_iterator operator+=(const int &n) {
                //TODO
                *this = *this + n;
                return *this;
            }
            const_iterator operator-=(const int &n) {
                //TODO
                *this = *this - n;
                return *this;
            }
            /**
             * TODO iter++
             * If there's not enough elements, this operation is still valid.
             * But the result is undefined.
             */
            const_iterator operator++(int) {
                const_iterator tmp(*this);
                *this = *this + 1;
                return tmp;
            }
            /**
             * TODO ++iter
             */
            const_iterator& operator++() {
                *this = *this + 1;
                return *this;
            }
            /**
             * TODO iter--
             */
            const_iterator operator--(int) {
                const_iterator tmp(*this);
                *this = *this - 1;
                return tmp;
            }
            /**
             * TODO --iter
             */
            const_iterator& operator--() {
                *this = *this - 1;
                return *this;
            }
            /**
             * TODO *it
             */
            T& operator*() const {
                if(posNode->prev->prev == nullptr &&
                   posNode->logicLen == 0 &&
                   posIdx == 0)
                    throw invalid_iterator();
                
                if(posNode->next == nullptr)
                    throw invalid_iterator();
                
                return posNode->arr[posIdx];
            }
            /**
             * TODO it->field
             */
            T* operator->() const noexcept {
                T *ptr = &posNode->arr[posIdx];
                return ptr;
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator &rhs) const {
                try{
                    return (*this - rhs) == 0;
                }
                catch(invalid_iterator &ex){
                    return false;
                }
            }
            bool operator==(const const_iterator &rhs) const {
                try{
                    return (*this - rhs) == 0;
                }
                catch(invalid_iterator &ex){
                    return false;
                }
            }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                try{
                    return (*this - rhs) != 0;
                }
                catch(invalid_iterator &ex){
                    return false;
                }
            }
            bool operator!=(const const_iterator &rhs) const {
                try{
                    return (*this - rhs) != 0;
                }
                catch(invalid_iterator &ex){
                    return false;
                }
            }
        };
        /**
         * TODO Constructors
         */
        explicit deque(size_t blockSize = 30) {
            head = new node(blockSize);
            tail = new node(blockSize);
            head->next = new node(blockSize, head, tail);
            tail->prev = head->next;
            len = 0;
            blockNum = 1;
            this->blockSize = blockSize;
            counter = 0;
        }
        
        /** Wait for more member functions.
         * Or maybe I can write a prototype. */
        deque(const deque &other) {
            head = new node(other.head);
            tail = new node(other.tail);
            
            /** Running copy construct every node.
             * Everything detail is encapsulated in the copy constructor. */
            node *thisNode = head, *otherNode = other.head->next;
            
            while(otherNode != other.tail){
                node *newNode = new node(otherNode, thisNode);
                thisNode->next = newNode;
                otherNode = otherNode->next;
                thisNode = thisNode->next;
            }
            thisNode->next = tail;
            tail->prev = thisNode;
            
            /** Update fields. */
            blockNum = other.blockNum;
            blockSize = other.blockSize;
            len = other.len;
            counter = other.counter;
        }
        /**
         * TODO Deconstructor
         */
        ~deque() {
            clear();
            delete head->next;
            delete head;
            delete tail;
            head = tail = nullptr;
        }
        /**
         * TODO assignment operator
         */
        deque &operator=(const deque &other) {
            if(this == &other)
                return *this;
            this->~deque();
            head = new node(other.head);
            tail = new node(other.tail);
            
            /** Running copy construct every node.
             * Everything detail is encapsulated in the copy constructor. */
            node *thisNode = head, *otherNode = other.head->next;
            
            while(otherNode != other.tail){
                node *newNode = new node(otherNode, thisNode);
                thisNode->next = newNode;
                otherNode = otherNode->next;
                thisNode = thisNode->next;
            }
            thisNode->next = tail;
            tail->prev = thisNode;
            
            /** Update fields. */
            blockNum = other.blockNum;
            blockSize = other.blockSize;
            len = other.len;
            counter = other.counter;
            return *this;
        }
        /**
         * access specified element with bounds checking
         * throw index_out_of_bound if out of bound.
         */
        T & at(const size_t &pos) {
            if(pos < 0 || pos >= len)
                throw index_out_of_bound();
            
            iterator itr = begin() + pos;
            return *itr;
        }
        const T & at(const size_t &pos) const {
            if(pos < 0 || pos >= len)
                throw index_out_of_bound();
            
            const_iterator itr = cbegin() + pos;
            return *itr;
        }
        T & operator[](const size_t &pos) { return at(pos); }
        const T & operator[](const size_t &pos) const { return at(pos); }
        /**
         * access the first element
         * throw container_is_empty when the container is empty.
         */
        const T & front() const {
            if(empty())
                throw container_is_empty();
            
            return *(cbegin());
            
        }
        /**
         * access the last element
         * throw container_is_empty when the container is empty.
         */
        const T & back() const {
            if(empty())
                throw container_is_empty();
            return tail->prev->arr[tail->prev->logicLen - 1];
            
        }
        /**
         * returns an iterator to the beginning.
         */
        iterator begin() { return iterator(head->next, 0, head); }
        const_iterator cbegin() const { return const_iterator(head->next, 0, head); }
        /**
         * returns an iterator to the end.
         */
        iterator end() { return iterator(tail, 0, head); }
        const_iterator cend() const { return const_iterator(tail, 0, head); }
        /**
         * checks whether the container is empty.
         */
        bool empty() const { return len == 0; }
        /**
         * returns the number of elements
         */
        size_t size() const { return len; }
        /**
         * clears the contents
         */
        void clear() {
            while(len > 0)
                pop_back(false);
            counter = 0;
        }
        /**
         * inserts elements at the specified location in the container.
         * inserts value before pos
         * returns an iterator pointing to the inserted value
         *     throw if the iterator is invalid or it point to a wrong place.
         */
        
        /** An iterator is invalid if
         * 1. It has different headAddress with current deque.
         * 2. Its posIdx isn't smaller than allocLen.
         * 3. It points to an invalid place in deque.
         * ----------------------------------------------------------
         * Since iterator is the inner class of deque, its value can only
         * be modified by some member functions, which puts some invariance
         * on it.
         * We only need to consider 1 invalid cases.
         * Note that a pointer can also points to end(), where we need to consider
         * separately. */
        iterator insert(iterator pos, const T &value) {
            /** Note that these are two pointers. */
            if(pos.headAddress != head)
                throw invalid_iterator();
            
            if(pos.posNode->next == nullptr){
                push_back(value);
                return iterator(tail->prev, tail->prev->logicLen - 1, head);
            }
            
            /** Insert consists of three conditions:
             * 1. Current node isn't full.
             * 2. Next node isn't full.
             * 3. Next node is full. */
            node *posNode = pos.posNode;
            int posIdx = pos.posIdx;
            /** There's still empty slot free to insert. */
            if(posNode->logicLen < blockSize){
                /** Construct in advance. */
                T *tmp = &posNode->arr[posNode->logicLen];
                new (tmp) T(value);
                /** Shift until target slot. */
                for(int i = posNode->logicLen; i > posIdx; --i)
                    posNode->arr[i] = posNode->arr[i-1];
                posNode->arr[posIdx] = value;
                /** Update fields */
                ++len;
                ++posNode->logicLen;
            }
            /** No free space for insert, check the next node. */
            else{
                /** Remember the last element.*/
                T tmp = posNode->arr[blockSize-1];
                /** Shift until target slot. */
                for(int i = posNode->logicLen - 1; i > posIdx; --i)
                    posNode->arr[i] = posNode->arr[i-1];
                posNode->arr[posIdx] = value;
                
                /** If the next isn't full. */
                if(posNode->next->logicLen < blockSize && posNode->next != tail){
                    /** Copy code in push_front. */
                    node *curNode = posNode->next;
                    /** construct the very last element. */
                    T *ptrTmp = &curNode->arr[curNode->logicLen];
                    new (ptrTmp) T(tmp);
                    /** Shift array elements. */
                    for(int i = curNode->logicLen; i > 0; --i)
                        curNode->arr[i] = curNode->arr[i-1];
                    /** push_front the remembered last element. */
                    curNode->arr[0] = tmp;
                    /** Update field. */
                    ++(curNode->logicLen);
                    ++len;
                }
                /** If the next is full, we create a new block
                 * to hold it, because every adjacent 2 blocks
                 * have more than blockSize elements. This
                 * satisfy requirements. */
                else {
                    /** Create a new block. */
                    node *newNode = new node(blockSize, posNode, posNode->next);
                    posNode->next->prev = newNode;
                    posNode->next = newNode;
                    /** Add the previous last element to the first. */
                    T *ptrTmp = &newNode->arr[0];
                    new (ptrTmp) T(tmp);
                    /** Update fields. */
                    ++newNode->logicLen;
                    ++blockNum;
                    ++len;
                }
            }
            
            if(counter == resizeConstant){
                int offset = pos - begin();
                counter = 0;
                resize();
                pos = begin() + offset;
            }
            else{
                ++counter;
            }
            
            /** pos won't change. */
            return pos;
        }
        /**
         * removes specified element at pos.
         * removes the element at pos.
         * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
         * throw if the container is empty, the iterator is invalid or it points to a wrong place.
         */
        iterator erase(iterator pos) {
            /** Note that these are two pointers. */
            if(pos.headAddress != head || pos == end())
                throw invalid_iterator();
            if(len == 0)
                throw container_is_empty();
            
            
            
            
            /* Use offset to return fresh iterator. */
            int offset = pos - begin();
            /** Delete within posNode. */
            node *curNode = pos.posNode;
            
            
            
            for(int i = pos.posIdx; i < curNode->logicLen - 1; ++i)
                curNode->arr[i] = curNode->arr[i + 1];
            
            curNode->arr[curNode->logicLen-1].~T();
            
            /** Update fields. */
            --curNode->logicLen;
            --len;
            
            /** Check merge. Note that */
            if(curNode != head && curNode->prev != head && curNode != tail && // head->next != curNode &&
               (curNode->logicLen + curNode->prev->logicLen) <= blockSize){
                /*
                 if(curNode == head->next)
                 std::cout << "haha";
                 
                 std::cout << curNode->logicLen << ' ' << curNode->allocLen << std::endl;
                 for(int i = 0; i < curNode->logicLen; ++i){
                 std::cout << &curNode->arr[i] << ' ' ;
                 }
                 std::cout << std::endl << std::endl;
                 
                 traverse();
                 
                 std::cout << curNode << std::endl;*/
                merge(curNode->prev);
                
                // std::cout << "merge" << ' ';
                --blockNum;
            }
            else if(curNode != tail && curNode->next != tail && curNode != head &&
                    (curNode->logicLen + curNode->next->logicLen) <= blockSize){
                merge(curNode);
                // std::cout << "merge" << ' ';
                --blockNum;
            }
            
            if(counter == resizeConstant){
                counter = 0;
                resize();
            }
            else{
                ++counter;
            }
            
            return begin() + offset;
        }
        
        
        /** Comments for push_back, pop_back, push_front, pop_front:
         * They add or remove elements as described.
         * If there's enough space, they shift elements and insert naively.
         * If there's no space left, create another node.
         * If node is empty after delete, destroy that node. */
        
        /**
         * adds an element to the end
         */
        void push_back(const T &value, bool resizeFlag = false) {
            /** Create another node. */
            if(tail->prev->logicLen == tail->prev->allocLen){
                node *newNode = new node(blockSize, tail->prev, tail);
                tail->prev->next = newNode;
                tail->prev = newNode;
                T *tmp = &newNode->arr[0];
                new (tmp) T(value);
                ++newNode->logicLen;
                ++blockNum;
                ++len;
            }
            /** Push_back naively. */
            else{
                node *curNode = tail->prev;
                T *tmp = &curNode->arr[curNode->logicLen];
                new (tmp) T(value);
                ++curNode->logicLen;
                ++len;
            }
            
            if(!resizeFlag){
                if(counter == resizeConstant){
                    counter = 0;
                    resize();
                }
                else{
                    ++counter;
                }
            }
        }
        /**
         * removes the last element
         *     throw when the container is empty.
         */
        void pop_back(bool resizeFlag = true) {
            /** If block is empty! */
            if(len == 0)
                throw container_is_empty();
            if(tail->prev->logicLen == 0){
                // traverse();
                throw pop_empty_block();
            }
            /** Otherwise we delete anyway. */
            node *cur = tail->prev;
            cur->arr[cur->logicLen-1].~T();
            --cur->logicLen;
            --len;
            /** If this gives an empty block */
            if(cur->logicLen == 0 && blockNum != 1){
                tail->prev->prev->next = tail;
                tail->prev = tail->prev->prev;
                delete cur;
                --blockNum;
            }
            
            if(resizeFlag){
                if(counter == resizeConstant){
                    counter = 0;
                    resize();
                }
                else{
                    ++counter;
                }
            }
        }
        /**
         * inserts an element to the beginning.
         */
        void push_front(const T &value) {
            if(head->next->logicLen < head->next->allocLen){
                /** In this case, we don't need to add node.
                 * Make sure that every element is properly constructed.
                 * And define a pointer to simplify expression. */
                node *cur = head->next;
                /** construct the very last element. */
                T *ptrTmp = &cur->arr[cur->logicLen];
                new (ptrTmp) T(value);
                /** Shift array elements. */
                for(int i = cur->logicLen; i > 0; --i)
                    cur->arr[i] = cur->arr[i-1];
                /** push_front val. */
                cur->arr[0] = value;
                /** Update field. */
                ++(cur->logicLen);
                ++len;
            }
            else{
                /** Create an additional node. */
                node *newNode = new node(blockSize, head, head->next);
                head->next->prev = newNode;
                head->next = newNode;
                /** Insert the first element. */
                T *ptrTmp = &newNode->arr[newNode->logicLen];
                new (ptrTmp) T(value);
                /** Update field. */
                ++(newNode->logicLen);
                ++len;
                ++blockNum;
            }
            
            if(counter == resizeConstant){
                counter = 0;
                resize();
            }
            else{
                ++counter;
            }
        }
        /**
         * removes the first element.
         *     throw when the container is empty.
         */
        void pop_front() {
            /** If block is empty! */
            if(len == 0)
                throw container_is_empty();
            if(head->next->logicLen == 0)
                throw pop_empty_block();
            
            /** Otherwise we delete anyway. */
            node *curNode = head->next;
            for(int i = 1; i < curNode->logicLen; ++i)
                curNode->arr[i-1] = curNode->arr[i];
            
            curNode->arr[curNode->logicLen-1].~T();
            --curNode->logicLen;
            --len;
            /** If this gives an empty block */
            if(curNode->logicLen == 0 && blockNum != 1){
                head->next->next->prev = head;
                head->next = head->next->next;
                delete curNode;
                --blockNum;
            }
            
            if(counter == resizeConstant){
                counter = 0;
                resize();
            }
            else{
                ++counter;
            }
        }
        
        /**
         * Function for test. Output logs. */
        void traverse() const {
            std::cout << "There are " << len << " elements "<< blockNum << " blocks " << "in this deque. " << std::endl;
            std::cout << "Traverse in sequential order: " << std::endl;
            node *cur = head->next;
            for(int i = 0; i < 5; ++i) {
                std::cout << "The " << i << "th block: " << &cur << std::endl;
                for(int j = 0; j < cur->logicLen; ++j){
                    std::cout << &cur->arr[j] << ' ';
                }
                std::cout << std::endl;
                cur = cur->next;
            }
            std::cout << std::endl;
            
            /*
             cur = head->next;
             while(cur != tail){
             for(int i = 0; i < cur->logicLen; ++i)
             std::cout << cur->arr[i] << ' ';
             std::cout << std::endl;
             cur = cur->next;
             }
             */
            /*
             std::cout << "Traverse in reverse order: " << std::endl;
             cur = tail->prev;
             for(int i = 0; i < blockNum; ++i) {
             std::cout << "The " << blockNum - 1 - i << "th block: " << std::endl;
             for(int j = cur->logicLen - 1; j >= 0; --j){
             std::cout << cur->arr[j] << ' ';
             }
             std::cout << std::endl;
             cur = cur->prev;
             }
             std::cout << std::endl;
             */
        }
        
        /** This function waste a copy. */
        void resize(){
            size_t newBlockSize = mySqrt(blockSize * blockNum);
            if(blockSize > newBlockSize/2 && blockSize < newBlockSize*2)
                return;
            
            //std::cout << "resize" << ' ';
            deque newDq(newBlockSize);
            
            iterator itr = begin();
            
            for(int i = 0; i < len; ++i){
                newDq.push_back(*itr, true);
                ++itr;
            }
            *this = newDq;
        }
        
        void test(){
            node *cur = head;
            bool flag = true;
            while(cur != tail){
                if(cur != cur->next->prev){
                    flag = false;
                    break;
                }
                cur = cur->next;
            }
            if(!flag)
                std::cout << "Shit!" << std::endl;
        }
    };
}

#endif
