/**
 * implement a container like std::map
 *
 * implement insert with recursion and erase without recursion,
 * because insert needs to do backTrack with rotation.
 *
 * this treap is a maximum heap.
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"
#define LL long long
namespace sjtu {
    
    template<
    class Key,
    class T,
    class Compare = std::less<Key>
    > class map {
    public:
        /**
         * the internal type of data.
         * it should have a default constructor, a copy constructor.
         * You can use sjtu::map as value_type by typedef.
         */
        typedef pair<const Key, T> value_type;
    private:
        inline int rand1(){
            static int seed = 12345;
            return seed=int(seed*1103515245LL%2147483647);
        }
        
        struct node {
            value_type *data;
            int priority;
            node *parent, *lson, *rson, *prev, *next;
            
            node(const value_type *d, int p) {
                /// malloc memory and construct value_type.
                if(d != nullptr) {
                    data = (value_type *) malloc(sizeof(value_type));
                    new (data) value_type(*d);
                }
                else { data = nullptr; }
                
                priority = p;
                parent = lson = rson = prev = next = nullptr;
            }
            ~node() {
                if(data != nullptr) {
                    data->first.~Key(); data->second.~T();
                    free(data);
                    data = nullptr;
                }
            }
        };
        
        /// head and tail are sentinel nodes.
        node *root, *head, *tail;
        size_t elemSz;
        Compare cmp;
        
        bool equivalence(const Key k1, const Key k2) const {
            return !cmp(k1, k2) && !cmp(k2, k1);
        }
    public:
        
        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = map.begin(); --it;
         *       or it = map.end(); ++end();
         */
        class const_iterator;
        class iterator {
            friend const_iterator;
            friend map;
        private:
            /**
             * TODO add data members
             *   just add whatever you want.
             */
            node *p;
            node *headId;
        public:
            iterator() {
                // TODO
                p = nullptr;
                headId = nullptr;
            }
            iterator(const iterator &other) {
                // TODO
                p = other.p;
                headId = other.headId;
            }
            /**
             * return a new iterator which pointer n-next elements
             *   even if there are not enough elements, just return the answer.
             * as well as operator-
             */
            /**
             * TODO iter++
             */
            iterator operator++(int) {
                iterator tmp = *this;
                if(p == nullptr || p->next == nullptr) throw invalid_iterator();
                p = p->next;
                return tmp;
            }
            /**
             * TODO ++iter
             */
            iterator & operator++() {
                if(p == nullptr || p->next == nullptr) throw invalid_iterator();
                p = p->next;
                return *this;
            }
            /**
             * TODO iter--
             */
            iterator operator--(int) {
                iterator tmp = *this;
                if(p == nullptr || p->prev == nullptr || p->prev->prev == nullptr) throw invalid_iterator();
                p = p->prev;
                return tmp;
            }
            /**
             * TODO --iter
             */
            iterator & operator--() {
                if(p == nullptr || p->prev == nullptr || p->prev->prev == nullptr) throw invalid_iterator();
                p = p->prev;
                return *this;
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type & operator*() const {
                if(p == nullptr || p->data == nullptr) throw index_out_of_bound();
                return *p->data;
            }
            bool operator==(const iterator &rhs) const {
                return p == rhs.p && headId == rhs.headId;
            }
            bool operator==(const const_iterator &rhs) const {
                return p == rhs.p && headId == rhs.headId;
            }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return p != rhs.p || headId != rhs.headId;
            }
            bool operator!=(const const_iterator &rhs) const {
                return p != rhs.p || headId != rhs.headId;
            }
            
            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            value_type* operator->() const {
                if(p == nullptr || p->data == nullptr) throw invalid_iterator();
                return p->data;
            }
        };
        class const_iterator {
            friend iterator;
            friend map;
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
        private:
            // data members.
            const node *p;
            node *headId;
        public:
            const_iterator() {
                // TODO
                p = nullptr; headId = nullptr;
            }
            const_iterator(const const_iterator &other) {
                // TODO
                p = other.p; headId = other.headId;
            }
            const_iterator(const iterator &other) {
                // TODO
                p = other.p; headId = other.headId;
            }
            /**
             * return a new iterator which pointer n-next elements
             *   even if there are not enough elements, just return the answer.
             * as well as operator-
             */
            /**
             * TODO iter++
             */
            const_iterator operator++(int) {
                const_iterator tmp = *this;
                if(p == nullptr || p->next == nullptr) throw invalid_iterator();
                p = p->next;
                return tmp;
            }
            /**
             * TODO ++iter
             */
            const_iterator & operator++() {
                if(p == nullptr || p->next == nullptr) throw invalid_iterator();
                p = p->next;
                return *this;
            }
            /**
             * TODO iter--
             */
            const_iterator operator--(int) {
                const_iterator tmp = *this;
                if(p == nullptr || p->prev == nullptr || p->prev->prev == nullptr) throw invalid_iterator();
                p = p->prev;
                return tmp;
            }
            /**
             * TODO --iter
             */
            const_iterator & operator--() {
                if(p == nullptr || p->prev == nullptr || p->prev->prev == nullptr) throw invalid_iterator();
                p = p->prev;
                return *this;
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type & operator*() const {
                if(p == nullptr || p->data == nullptr) throw index_out_of_bound();
                return *p->data;
            }
            bool operator==(const iterator &rhs) const { return p == rhs.p && headId == rhs.headId; }
            bool operator==(const const_iterator &rhs) const { return p == rhs.p && headId == rhs.headId; }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const { return p != rhs.p || headId != rhs.headId; }
            bool operator!=(const const_iterator &rhs) const { return p != rhs.p || headId != rhs.headId; }
            
            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            const value_type* operator->() const {
                if(p == nullptr || p->data == nullptr) throw invalid_iterator();
                return p->data;
            }
        };
        /**
         * TODO two constructors
         */
        map() {
            head = new node(nullptr, rand1());
            tail = new node(nullptr, rand1());
            
            head->next = tail; tail->prev = head;
            
            root = nullptr;
            elemSz = 0;
        }
        
        map(const map &other) {

            head = new node(nullptr, rand1());
            tail = new node(nullptr, rand1());
            
            head->next = tail; tail->prev = head;
            
            root = nullptr;
            elemSz = 0;
            
            const_iterator citr = other.cbegin();
            const_iterator cend = other.cend();
            
            while(citr != cend) {
                insert(*citr); ++citr;
            }
        }
        /**
         * TODO assignment operator
         */
        map & operator=(const map &other) {
            if(&other == this) return *this;
            clear();
            const_iterator citr = other.cbegin();
            const_iterator cend = other.cend();
            
            while(citr != cend) {
                insert(*citr); ++citr;
            }
            return *this;
        }
        /**
         * TODO Destructors
         */
        ~map() { clear();
            delete head; delete tail;
        }
        /**
         * TODO
         * access specified element with bounds checking
         * Returns a reference to the mapped value of the element with key equivalent to key.
         * If no such element exists, an exception of type `index_out_of_bound'
         */
        T & at(const Key &key) {
            node *ptr = find_erase(key);
            if(!ptr) throw index_out_of_bound();
            return ptr->data->second;
        }
        const T & at(const Key &key) const {
            const node *ptr = find_erase(key);
            if(!ptr) throw index_out_of_bound();
            return ptr->data->second;
        }
        /**
         * TODO
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T & operator[](const Key &key) {
            node *ptr = find_erase(key);
            if(ptr) return ptr->data->second;
            
            // const value_type nElem; nElem.first = key;
            const value_type nElem(key, T());
            iterator itr = insert(nElem).first;
            return (*itr).second;
        }
        /**
         * behave like at() throw index_out_of_bound if such key does not exist.
         */
        const T & operator[](const Key &key) const {
            return at(key);
        }
        /**
         * return a iterator to the beginning
         */
        iterator begin() { iterator itr; itr.p = head->next; itr.headId = head; return itr; }
        const_iterator cbegin() const { const_iterator citr; citr.p = head->next; citr.headId = head; return citr; }
        /**
         * return a iterator to the end
         * in fact, it returns past-the-end.
         */
        iterator end() { iterator itr; itr.p = tail; itr.headId = head; return itr; }
        const_iterator cend() const { const_iterator citr; citr.p = tail; citr.headId = head; return citr; }
        /**
         * checks whether the container is empty
         * return true if empty, otherwise false.
         */
        bool empty() const { return elemSz == 0; }
        /**
         * returns the number of elements.
         */
        size_t size() const { return elemSz; }
        /**
         * clears the contents
         */
        void clear() {
            clear(root);
            head->next = tail; tail->prev = head;
            elemSz = 0;
        }
        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that prevented the insertion),
         *   the second one is true if insert successfully, or false.
         */
        // first version of insert.
        /**
         pair<iterator, bool> insert(const value_type &value) {
         pair<iterator, bool> ret;
         if(root == nullptr) {
         root = new node(&value, rand1());
         head->next = tail->prev = root;
         root->next = tail; root->prev = head;
         ++elemSz;
         ret.first.p = root;
         ret.second = true;
         }
         else {
         ret = insert(root, value);
         }
         return ret;
         }
         */
        /** Since I use parent field, I had better use find_insert function to
         * find the place for val to be inserted, and rotate with the guidance of
         * parent pointer. */
        pair<iterator, bool> insert(const value_type &value) {
            pair<iterator, bool> ret; ret.first.headId = head;
            
            node *iptr = find_insert(value);
            
            // if(iptr) printf("insert value: %d %d; find_insert iptr: %d %d\n", value.first, value.second,
            //            iptr->data->first, iptr->data->second);
            
            if(iptr == nullptr) {
                root = new node(&value, rand1());
                ++elemSz;
                head->next = tail->prev = root;
                root->prev = head; root->next = tail;
                ret.first.p = root; ret.second = true;
                return ret;
            }
            else if(equivalence(iptr->data->first, value.first)) {
                ret.first.p = iptr; ret.second = false;
                return ret;
            }
            /** create new node, and jump down into it.
             * back track and rotation along its parents. */
            else if(cmp(value.first, iptr->data->first)) {
                iptr->lson = new node(&value, rand1());
                iptr->lson->parent = iptr;
                ++elemSz;
                /// maintain linked list.
                iptr->lson->prev = iptr->prev;
                iptr->lson->next = iptr;
                iptr->prev->next = iptr->lson;
                iptr->prev = iptr->lson;
                /// jump down into it.
                iptr = iptr->lson;
                
                ret.first.p = iptr; ret.second = true;
            }
            else {
                iptr->rson = new node(&value, rand1());
                iptr->rson->parent = iptr;
                ++elemSz;
                /// maintain linked list.
                iptr->rson->next = iptr->next;
                iptr->rson->prev = iptr;
                iptr->next->prev = iptr->rson;
                iptr->next = iptr->rson;
                /// jump down into it.
                iptr = iptr->rson;
                
                ret.first.p = iptr; ret.second = true;
            }
            /** backTrack to rotate, be careful that root may be changed. */
            while(iptr->parent && iptr->priority > iptr->parent->priority) {
                if(iptr == iptr->parent->lson) {
                    if(iptr->parent == root) root = iptr;
                    iptr = r_rt(iptr->parent);
                }
                else {
                    if(iptr->parent == root) root = iptr;
                    iptr = l_rt(iptr->parent);
                }
            }
            return ret;
        }
        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */
        /** erase a node with one or no child is easy.
         * erase a node with two children, I have to first rotate this node to previous two cases.
         * but first, I have to find the node.
         *
         * if root is erased, care must be taken.
         * */
        void erase(iterator pos) {
            if(pos.headId != head) throw invalid_iterator();
            if(pos.p == head || pos.p == tail || pos.p == nullptr) throw index_out_of_bound();
            
            node *ptr = pos.p;
            /**
             /// adjust the node to erase so that it has fewer then 2 sons.
             while(ptr->lson && ptr->rson) {
             /// high priority goes up.
             if(ptr->lson->priority > ptr->rson->priority) r_rt(ptr);
             else l_rt(ptr);
             }
             
             if(!ptr->lson && !ptr->rson) {
             if(ptr == root) {
             delete ptr; --elemSz;
             head->next = tail->prev = nullptr;
             }
             else {
             /// parent
             if(ptr == ptr->parent->lson) ptr->parent->lson = nullptr;
             else ptr->parent->rson = nullptr;
             /// linked list
             ptr->prev->next = ptr->next;
             ptr->next->prev = ptr->prev;
             /// delete
             delete ptr; --elemSz;
             }
             }
             else if(ptr->lson && !ptr->rson) {
             if(ptr == root) {
             root = ptr->lson;
             }
             } */
            
            /// keep rotating until have no child.
            while(ptr->lson || ptr->rson) {
                if(ptr->rson && ptr->lson) {
                    if(ptr->lson->priority > ptr->rson->priority) {
                        if(ptr == root) root = r_rt(ptr);
                        else r_rt(ptr);
                    }
                    else {
                        if(ptr == root) root = l_rt(ptr);
                        else l_rt(ptr);
                    }
                }
                else if(ptr->lson && !ptr->rson) {
                    if(ptr == root) root = ptr->lson;
                    r_rt(ptr);
                }
                else {
                    if(ptr == root) root = ptr->rson;
                    l_rt(ptr);
                }
            }
            
            if(ptr == root) {
                delete ptr; --elemSz;
                head->next = tail; tail->prev = head;
                root = nullptr;
            }
            else {
                /// parent
                if (ptr == ptr->parent->lson) ptr->parent->lson = nullptr;
                else ptr->parent->rson = nullptr;
                /// linked list
                ptr->prev->next = ptr->next;
                ptr->next->prev = ptr->prev;
                /// delete
                delete ptr;
                --elemSz;
            }
        }
        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         *   which is either 1 or 0
         *     since this container does not allow duplicates.
         * The default method of check the equivalence is !(a < b || b > a)
         */
        size_t count(const Key &key) const {
            node *ptr = find_erase(key);
            if(ptr) return 1; else return 0;
        }
        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is returned.
         */
        iterator find(const Key &key) {
            node *ptr = find_erase(key);
            if(ptr) {
                iterator itr; itr.p = ptr; itr.headId = head; return itr;
            }
            else return end();
        }
        const_iterator find(const Key &key) const {
            node *ptr = find_erase(key);
            if(ptr) {
                const_iterator citr; citr.p = ptr; citr.headId = head; return citr;
            }
            else return cend();
        }
        
        int height() { return height(root); }
        
        void check() {
            printf("map check starts: \n");
            if(empty()) {
                printf("map is empty. \n\n");
                return;
            }
            node *ptr = head->next; bool error = false;
            while(ptr != nullptr && ptr != head && ptr != tail) {
                if(ptr->prev->next != ptr) {
                    error = true;
                    printf("ptr->prev->next != ptr: ");
                    std::cout << ptr->data->first << ' ' << ptr->data->second << std::endl;
                }
                if(ptr->next->prev != ptr) {
                    error = true;
                    printf("ptr->next->prev != ptr: ");
                    std::cout << ptr->data->first << ' ' << ptr->data->second << std::endl;
                }
                if(ptr != root) {
                    if((ptr->parent->lson && ptr->parent->lson != ptr) && (ptr->parent->rson && ptr->parent->rson != ptr)) {
                        error = true;
                        printf("ptr->parent->son != ptr: ");
                        std::cout << ptr->data->first << ' ' << ptr->data->second << std::endl;
                    }
                }
                if(ptr->lson && ptr->lson->parent != ptr) {
                    error = true;
                    printf("ptr->lson->parent != ptr: ");
                    std::cout << ptr->data->first << ' ' << ptr->data->second << std::endl;
                }
                if(ptr->rson && ptr->rson->parent != ptr) {
                    error = true;
                    printf("ptr->rson->parent != ptr: ");
                    std::cout << ptr->data->first << ' ' << ptr->data->second << std::endl;
                }
                ptr = ptr->next;
            }
            if(!error) printf("congratulations! no error.\n");
            printf("\n");
        }
        
        void traverse() {
            if(empty()) {
                printf("map is empty\n");
            }
            else {
                node *ptr = root;
                traverse(ptr);
            }
        }
    private:
        void traverse(node *ptr) {
            if(ptr == nullptr) return;
            printf("current node: %d %d; ", ptr->data->first, ptr->data->second);
            if(ptr->lson) printf("lson: %d %d; ", ptr->lson->data->first, ptr->lson->data->second);
            else printf("no left son; ");
            if(ptr->rson) printf("rson: %d %d; ", ptr->rson->data->first, ptr->rson->data->second);
            else printf("no right son; ");
            if(ptr->parent) printf("parent: %d %d. \n", ptr->parent->data->first, ptr->parent->data->second);
            else printf("no parent. \n");
            traverse(ptr->lson);
            traverse(ptr->rson);
        }
        /** note this node *&p pass by reference.
         *
         * This function is invoked by insert(const value_type).
         *
         * it doesn't consider empty map situation, which requires caution
         * in prev and next linked list implementation.
         *
         * if new node is inserted when p == nullptr, parent pointer field
         * cannot be handled.
         * we insert to rson when p->rson == nullptr, etc.
         *
         * p will never be nullptr, and pass by reference may not be necessary any more. */
        // first version of insert private function.
        pair<iterator, bool> insert(node *ptr, const value_type &value) {
            pair<iterator, bool> ret; ret.first.headId = head;
            /// duplicated key is found.
            if(equivalence(ptr->data->first, value.first)) {
                ret.first.p = ptr; ret.second = false;
                return ret;
            }
            /**
             * if current ptr has left son, insert recursively and check
             * priority heap property in back track.
             *
             * if current ptr->lson == nullptr, insert directly and handle
             * linked list invariance and parent field. */
            /// value goes to left son.
            if(cmp(value.first, ptr->data->first)) {
                if(ptr->lson != nullptr) {
                    ret = insert(ptr->lson, value);
                    /** if insert successfully, adjust.
                     * don't need to adjust ptr any more, because of parent. */
                    if(ret.second && ptr->lson->priority > ptr->priority)
                        r_rt(ptr);
                    return ret;
                }
                else {
                    /// maintain parent.
                    ptr->lson = new node(&value, rand1());
                    ++elemSz;
                    ptr->lson->parent = ptr;
                    /// maintain linked list.
                    ptr->lson->next = ptr->next;
                    ptr->lson->prev = ptr;
                    ptr->next = ptr->lson;
                    /// handle ret.
                    ret.first.p = ptr->lson;
                    ret.second = true;
                    return ret;
                }
            }
            
            else if(cmp(ptr->data->first, value.first)) {
                if(ptr->rson != nullptr) {
                    ret = insert(ptr->rson, value);
                    /** if insert successfully, adjust.
                     * don't need to adjust ptr any more, because of parent. */
                    if(ret.second && ptr->rson->priority > ptr->priority)
                        l_rt(ptr);
                    return ret;
                }
                else {
                    /// maintain parent.
                    ptr->rson = new node(&value, rand1());
                    ++elemSz;
                    ptr->rson->parent = ptr;
                    /// maintain linked list.
                    ptr->rson->next = ptr->next;
                    ptr->rson->prev = ptr;
                    ptr->next = ptr->rson;
                    /// handle ret.
                    ret.first.p = ptr->rson;
                    ret.second = true;
                    return ret;
                }
            }
        }
        
        /** return a node for value to be inserted.
         * if map is empty, return nullptr.
         * if duplicated key, return the node with duplicated key.
         * else I can directly insert onto that node. */
        node *find_insert(const value_type &value) {
            if(root == nullptr) return nullptr;
            
            node *iptr = root;
            while(true) {
                if(equivalence(iptr->data->first, value.first)) {
                    return iptr;
                }
                else if(cmp(value.first, iptr->data->first)) {
                    if(iptr->lson == nullptr) return iptr;
                    else iptr = iptr->lson;
                }
                else {
                    if(iptr->rson == nullptr) return iptr;
                    else iptr = iptr->rson;
                }
            }
        }
        
        /** return a node pointer with the same key equal to parameter key.
         * return nullptr if not exist. */
        node *find_erase(const Key &key) const {
            node *ptr = root;
            while(ptr != nullptr && !equivalence(ptr->data->first, key)) {
                if(cmp(key, ptr->data->first)) ptr = ptr->lson;
                else ptr = ptr->rson;
            }
            return ptr;
        }
        
        /** do rotation and handle parent field, return pointer to new_top.
         * attention: root node has no parent.
         * parent field is horrible! */
        node *r_rt(node *p) {
            /*
             printf("left rotate: top node from %d %d to %d %d\n",
             p->data->first, p->data->second,
             p->lson->data->first, p->lson->data->second);
             */
            node *nt = p->lson; // nt: new_top.
            /// root node parent special case.
            if(p->parent) {
                if(p == p->parent->lson) p->parent->lson = nt;
                else p->parent->rson = nt;
            }
            
            nt->parent = p->parent; p->parent = nt;
            p->lson = nt->rson;
            if(p->lson) p->lson->parent = p;
            nt->rson = p;
            return nt;
        }
        
        node *l_rt(node *p) {
            /*
             printf("left rotate: top node from %d %d to %d %d\n",
             p->data->first, p->data->second,
             p->rson->data->first, p->rson->data->second);
             */
            node *nt = p->rson;
            
            if(p->parent) {
                if(p == p->parent->lson) p->parent->lson = nt;
                else p->parent->rson = nt;
            }
            
            nt->parent = p->parent; p->parent = nt;
            p->rson = nt->lson;
            if(p->rson) p->rson->parent = p;
            nt->lson = p;
            // traverse();
            return nt;
        }
        
        /** a careless function for clear(). */
        void clear(node *&p) {
            if(p == nullptr) return;
            clear(p->lson);
            clear(p->rson);
            delete p; p = nullptr;
        }
        
        int height(node *p) {
            if(p == nullptr) return 0;
            return std::max(height(p->lson), height(p->rson)) + 1;
        }
    };
}

#endif
