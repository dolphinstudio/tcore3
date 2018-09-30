#ifndef __tlist_h__
#define __tlist_h__

#include "multisys.h"

namespace tcore {
    class tbase;
    class tlist {
    public:
        tlist() : _head(nullptr), _tail(nullptr) {}
        virtual ~tlist() {}

        void pushBack(tbase * p);
        tbase* popFront();
        void remove(tbase* p);

        inline bool empty() { return _head == nullptr; }

    private:
        tlist(tlist&);
        tlist & operator=(tlist &);

        tbase * _head;
        tbase * _tail;
    };
}

#endif //__tlist_h__
