#ifndef __Event_h__
#define __Event_h__

#include "iEvent.h"
#include <list>
#include <unordered_map>

class event : public iEvent {
    typedef std::list<iEventer *> eventer_list;
    typedef std::unordered_map<s32, eventer_list> eventer_map;
public:
    virtual bool initialize(api::iCore * core);
    virtual bool launch(api::iCore * core);
    virtual bool destroy(api::iCore * core);

    virtual void trigger(const s32 eventid, const void * context, const s32 size);
    virtual void register_ev(const s32 eventid, iEventer * handle);
private:
    eventer_map _event_pool;
};

#endif //__Event_h__
