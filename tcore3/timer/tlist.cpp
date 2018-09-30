#include "tlist.h"
#include "tbase.h"

namespace tcore {
    void tlist::pushBack(tbase * p) {
        p->setList(this);
        p->setNext(nullptr);
        p->setPrev(_tail);
        if (_tail)
            _tail->setNext(p);
        else
            _head = p;

        _tail = p;
    }

    tbase* tlist::popFront() {
        tbase* p = _head;
        if (_head) {
            _head = _head->getNext();
            if (!_head)
                _tail = nullptr;
            else
                _head->setPrev(nullptr);

            p->setNext(nullptr);
            p->setPrev(nullptr);
            p->setList(nullptr);
        }
        return p;
    }

    void tlist::remove(tbase* p) {
        if (p->getPrev())
            p->getPrev()->setNext(p->getNext());

        if (p->getNext())
            p->getNext()->setPrev(p->getPrev());

        if (p == _head)
            _head = _head->getNext();

        if (p == _tail)
            _tail = _tail->getPrev();

        p->setNext(nullptr);
        p->setPrev(nullptr);
        p->setList(nullptr);
    }
}
