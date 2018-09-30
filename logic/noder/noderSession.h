#ifndef __noderSession_h__
#define __noderSession_h__

#include "header.h"

class noderSession : public iNoderSession, public tlib::sort_leaf {
public:
    virtual ~noderSession() {}

    noderSession() {}
    noderSession(const std::string & name, const s32 area, const s32 id, const std::string & ip, const s32 port)
        : _name(name), _area(area), _id(id), _ip(ip), _port(port) {}

    virtual void onConnected(api::iCore * core);
    virtual void onDisconnect(api::iCore * core);
    virtual void onConnectFailed(api::iCore * core);

    static void onReport(api::iCore * core, iSession * session, s64 context, const oReport & body);
    static void onLoadReport(api::iCore * core, iSession * session, s64 context, const oLoadReport & body);
    static void onRelayClientMessageToNoder(api::iCore * core, iSession * session, s64 context, const oRelayClientMessageToNoder & body);

    virtual const std::string & getname() { return _name; }
    virtual const s32 getarea() { return _area; }
    virtual const s32 getid() { return _id; }
    virtual const std::string & getip() { return _ip; }
    virtual const s32 getport() { return _port; }

    void setname(const std::string & name) { _name = name; }
    void setid(const s32 id) { _id = id; }
    void setip(const std::string & ip) { _ip = ip; }
    void setport(const s32 port) { _port = port; }

private:
    std::string _name;
    s32 _area;
    s32 _id;
    std::string _ip;
    s32 _port;
};

#endif //__noderSession_h__
