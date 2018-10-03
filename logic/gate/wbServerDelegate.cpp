#include "wbServerDelegate.h"
#include "wsDelegate.h"

iWBSessionDelegate * wbServerDelegate::onMallocSessionDelegate() {
    wsDelegate * wsd = NEW wsDelegate;
    wsd->retain();
    return wsd;
}
