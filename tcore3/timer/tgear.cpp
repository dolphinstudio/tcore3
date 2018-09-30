#include "tgear.h"
#include "tlist.h"
#include "timermgr.h"
#include "tbase.h"

namespace tcore {
    tgear::tgear(int maxMoveDst, tgear* nextGear)
        : _timerVec(nullptr)
        , _nextGear(nextGear)
        , _curMoveDst(0)
        , _maxMoveDst(maxMoveDst) {

        _timerVec = NEW tlist[_maxMoveDst];
    }

    tgear::~tgear() {
        if (_timerVec) {
            DEL[] _timerVec;
            _timerVec = nullptr;
        }
    }

    tlist * tgear::getTimerList(s32 index) {
        tassert(index < _maxMoveDst, "index out of range");

        return _timerVec + index;
    }

    void tgear::checkHighGear() {
        if (_curMoveDst >= _maxMoveDst)
            _curMoveDst = 0;

        if (_curMoveDst == 0) {
            if (_nextGear){
                _nextGear->updateToLowGear();
            }
        }
    }

    void tgear::update() {
        if (_curMoveDst >= _maxMoveDst)
            _curMoveDst = 0;

        tlist* currentList = &_timerVec[_curMoveDst];
        if (!currentList) {
            _curMoveDst++;
            return;
        }

        while (!currentList->empty()) {
            tbase * base = currentList->popFront();
            if (!base)
                continue;

            timermgr::getInstance()->moveToRunning(base);
        }

        ++_curMoveDst;
        if (_curMoveDst == _maxMoveDst)
            _curMoveDst = 0;
    }

    void tgear::updateToLowGear() {
        if (_curMoveDst >= _maxMoveDst)
            _curMoveDst = 0;

        if (_curMoveDst == 0) {
            if (_nextGear)
                _nextGear->updateToLowGear();
        }

        tlist* currentList = &_timerVec[_curMoveDst];
        if (!currentList) {
            ++_curMoveDst;
            return;
        }

        while (!currentList->empty()) {
            tbase * base = currentList->popFront();
            if (!base)
                continue;

            timermgr::getInstance()->schedule(base);
        }

        ++_curMoveDst;
    }
}