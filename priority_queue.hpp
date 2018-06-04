#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"
#include "vector.hpp"

namespace sjtu {
    
    /**
     * a container like std::priority_queue which is a heap internal.
     * it should be based on the vector written by yourself.
     *
     * NOTE: THIS IS A MAXIMUM HEAP!
     */
    template<typename T, class Compare = std::less<T>>
    class priority_queue {
        
    private:
        vector<T> heap;
        size_t length;
        Compare comp;
        
        /** Note that for simplicity, we are using storage array index,
         * where left child is 2n+1, right child is 2n+2.
         * But if we do this, their fathers are not hole/2.
         * percolate small element down! THIS IS A MAXIMUM HEAP! */
        void percolateDown(int i){
            int hole = i;
            T tmp = heap[hole];
            
            while(hole * 2 <= length){
                /** Pick out the bigger child.
                 * first is left child, then compare it with the right one. */
                int biggerChild = 2 * hole;
                if(biggerChild < length &&
                   comp(heap[biggerChild], heap[biggerChild + 1]))
                    ++biggerChild;
                
                /** Compare child with hole. If hole is updated, continue,
                 * otherwise break and insert hole. */
                if(comp(tmp, heap[biggerChild])) {
                    heap[hole] = heap[biggerChild];
                    hole = biggerChild;
                } else
                    break;
            }
            
            heap[hole] = tmp;
        }
    public:
        /**
         * TODO constructors
         */
        priority_queue() { length = 0; };
        priority_queue(const priority_queue &other) {
            if(this == &other) return;
            for(int i = 0; i < other.heap.size(); ++i)
                heap.push_back(other.heap[i]);
            length = other.length;
        }
        /**
         * TODO deconstructor
         */
        ~priority_queue() {}
        /**
         * TODO Assignment operator
         */
        priority_queue &operator=(const priority_queue &other) {
            if(this == &other) return *this;
            heap.clear();
            for(int i = 0; i < other.heap.size(); ++i)
                heap.push_back(other.heap[i]);
            length = other.length;
            return *this;
        }
        /**
         * get the top of the queue.
         * @return a reference of the top element.
         * throw container_is_empty if empty() returns true;
         */
        const T & top() const {
            if(empty()) throw container_is_empty();
            
            return heap[1];
        }
        /**
         * TODO
         * push new element to the priority queue.
         */
        void push(const T &e) {
            /** Create a dummy element. */
            if(heap.size() == 0 && length == 0)
                heap.push_back(e);
            
            /** First create element. */
            heap.push_back(e);
            
            int hole = ++length;
            while(hole / 2 > 0) {
                if(comp(heap[hole/2], e)) {
                    heap[hole] = heap[hole/2];
                    hole /= 2;
                } else
                    break;
            }
            heap[hole] = e;
        }
        /**
         * TODO
         * delete the top element.
         * throw container_is_empty if empty() returns true;
         */
        void pop() {
            if(empty()) throw container_is_empty();
            
            T tmp = heap[length--];
            heap.pop_back();
            
            /** If this gives an empty heap, just return.
             * Otherwise we need to maintain the ordering relation
             * in this heap. */
            if(length == 0) return;
            else {
                heap[1] = tmp;
                percolateDown(1);
            }
        }
        /**
         * return the number of the elements.
         */
        size_t size() const {
            return length;
        }
        /**
         * check if the container has at least an element.
         * @return true if it is empty, false if it has at least an element.
         */
        bool empty() const {
            return length == 0;
        }
        /**
         * return a merged priority_queue with at least O(logn) complexity.
         */
        void merge(priority_queue &other) {
            for(int i = 1; i <= other.length; ++i){
                heap.push_back(other.heap[i]);
                ++length;
            }
            other.heap.clear();
            other.length = 0;
            
            /** Build heap. */
            for(int i = length/2; i > 0; --i)
                percolateDown(i);
        }
    };
}

#endif
