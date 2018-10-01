#ifndef __webSocketHeader_h__
#define __webSocketHeader_h__

#include "multisys.h"
#include "iWebSocket.h"
#include "tpool.h"
#include "base64.h"
#include "sha1.h"

#ifdef WIN32
#include <Winsock2.h>
#pragma comment( lib, "ws2_32.lib" )
#else
#include <arpa/inet.h>
#endif //WIN32

#include <map>
#include <string>
#include <iostream> 
#include <sstream>

class webSocketSession;

using namespace tcore;
extern api::iCore * g_core;

#endif //__webSocketHeader_h__
