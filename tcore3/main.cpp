#include "tools.h"
#include "logic/logic.h"
#include "logger/logger.h"
#include <fstream>
#include <stdlib.h>
#ifdef WIN32
#include "dumper.h"
class AttachDumper {
public:
    AttachDumper() {
        tlib::dumper::getInstance().setDumpFilename((tools::time::getCurrentTimeString("dump/%4d_%02d_%02d_%02d_%02d_%02d").append(".dmp")).c_str());
    }
};
AttachDumper dumper;

#endif //WIN32

#include "core.h"
using namespace tcore;

int main(int argc, const char ** args, const char ** env) {
    core::getInstance()->parseArgs(argc, args);
    if (core::getInstance()->getArgs("pause")) {
        getchar();
    }
    
    const char * name = core::getInstance()->getArgs("name");
    if (name) {
        core::getInstance()->setCorename(name);
    } else {
        core::getInstance()->setCorename("test");
    }

    logic::getInstance()->launch();
    logger::getInstance();
    while (1) {
        core::getInstance()->loop();
    }

    return 0;
}
