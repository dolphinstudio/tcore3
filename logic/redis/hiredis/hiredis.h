#ifdef WIN32
#include "hiredis/win/hiredis.h"
#pragma comment(lib,"ws2_32.lib")
#include <winsock.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "hiredis/linux/hiredis.h"
#endif //WIN32
