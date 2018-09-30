#ifndef __tqcsort_h__
#define __tqcsort_h__

#include "multisys.h"
#include <set>

namespace tlib {
    class sort_pool;

    class sort_leaf {
    public:
        sort_leaf() : _last(nullptr), _next(nullptr), _load(0) {}
        virtual ~sort_leaf() {}

    private:
        friend class sort_pool;
        sort_leaf * _last;
        sort_leaf * _next;

        s32 _load;
    };

    class sort_pool {
    public:
        virtual ~sort_pool() {}

        inline void leaf_swap(sort_leaf * a, sort_leaf * b) {
            (nullptr != a->_last) ? a->_last->_next = b : a = a;
            (nullptr != a->_next) ? a->_next->_last = b : a = a;

            (nullptr != b->_last) ? b->_last->_next = a : b = b;
            (nullptr != b->_next) ? b->_next->_last = a : b = b;

            sort_leaf * swap = a->_last;
            a->_last = b->_last;
            b->_last = swap;

            swap = a->_next;
            a->_next = b->_next;
            b->_next = swap;
        }

        void push_front(sort_leaf * leaf) {
            if (_set.find(leaf) != _set.end()) {
                tassert(false, "wtf");
                return;
            }

            if (nullptr == _head) {
                tassert(_head == _end, "wtf");
                _head = leaf;
                _end = leaf;
            } else {
                _head->_last = leaf;
                leaf->_next = _head;
                _head = leaf;
            }

            while (leaf->_next != nullptr) {
                if (leaf->_next->_load < leaf->_load) {
                    leaf_swap(leaf, leaf->_next);
                } else {
                    break;
                }
            }

            putright_head_end();
            _set.insert(leaf);
        }

        void push_back(sort_leaf * leaf) {
            if (_set.find(leaf) != _set.end()) {
                tassert(false, "wtf");
                return;
            }

            if (nullptr == _end) {
                tassert(_head == _end, "wtf");
                _head = leaf;
                _end = leaf;
            } else {
                _end->_next = leaf;
                leaf->_last = _end;
                _end = leaf;
            }

            while (leaf->_last != nullptr) {
                if (leaf->_last->_load > leaf->_load) {
                    leaf_swap(leaf, leaf->_last);
                } else {
                    break;
                }
            }

            putright_head_end();
            _set.insert(leaf);
        }

        inline void set_load(sort_leaf * leaf, s32 load) {
            if (_set.find(leaf) == _set.end()) {
                tassert(false, "wtf");
                return;
            }

            leaf->_load = load;

            while (leaf->_next != nullptr) {
                if (leaf->_next->_load < leaf->_load) {
                    leaf_swap(leaf, leaf->_next);
                }
                else {
                    break;
                }
            }

            while (leaf->_last != nullptr) {
                if (leaf->_last->_load > leaf->_load) {
                    leaf_swap(leaf, leaf->_last);
                }
                else {
                    break;
                }
            }
            putright_head_end();
        }

        inline void remove(sort_leaf * leaf) {
            auto itor = _set.find(leaf);
            if (itor == _set.end()) {
                tassert(false, "wtf");
                return;
            }
            
            leaf->_next ? leaf->_next->_last = leaf->_last : leaf = leaf;
            leaf->_last ? leaf->_last->_next = leaf->_next : leaf = leaf;

            leaf == _head ? _head = leaf->_next : leaf = leaf;
            leaf == _end ? _end = leaf->_last : leaf = leaf;

            leaf->_next = nullptr;
            leaf->_last = nullptr;

            _set.erase(itor);
        }

        template<typename t>
        t * get_head() { return (t *)_head; }

        template<typename t>
        t * get_end() { return (t *)_end; }

    private:
        inline void putright_head_end() {
            while (_head->_last != nullptr) {
                _head = _head->_last;
            }

            while (_end->_next != nullptr) {
                _end = _end->_next;
            }
        }

    private:
        sort_leaf * _head;
        sort_leaf * _end;

        std::set<sort_leaf *> _set;
    };
}

#endif //__tqcsort_h__
