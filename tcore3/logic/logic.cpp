#include "logic.h"
#include "core.h"
#include "tools.h"
#include "tinyxml/tinyxml.h"

#include <string>
#include <map>
#include <list>
typedef std::list<iModule *> MODULE_LIST;
typedef std::map<std::string, iModule *> MODULE_MAP;
static MODULE_LIST static_module_list;
static MODULE_MAP static_module_map;

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif //linux

namespace tcore {
    logic * logic::getInstance() {
        static logic * p = nullptr;
        if (nullptr == p) {
            p = NEW logic;
            p->launch();
        }

        return p;
    }

    bool logic::launch() {
        core * core = core::getInstance();

        std::string configpath;
        configpath.append( tools::file::getApppath()).append("/server_config/").append(core->getCorename()).append("/core/module.xml");

        TiXmlDocument config;
        if (!config.LoadFile(configpath.c_str())) {
            tassert(false, "where is %s", configpath.c_str());
            return false;
        }

        TiXmlElement * root = config.RootElement();
        tassert(root, "wtf");
        const char * path = root->Attribute("path");
        if (nullptr == path) {
            path = "";
        }

        TiXmlElement * module = root->FirstChildElement("module");
        while (module) {
            std::string modulepath;
#ifdef WIN32
            modulepath.append(tools::file::getApppath()).append(path).append("/").append(module->Attribute("name")).append(".dll");
            HINSTANCE hinst = ::LoadLibrary(modulepath.c_str());
            if (!hinst) {
                printf("load %s error %s\n", modulepath.c_str(), strerror(::GetLastError()));
            }
            getModule fun = (getModule)::GetProcAddress(hinst, "getModules");
#else
            modulepath.append(tools::file::getApppath()).append(path).append("/lib").append(module->Attribute("name")).append(".so");
            void * handle = dlopen(modulepath.c_str(), RTLD_LAZY);
            if (!handle) {
                printf("load %s error %s\n", modulepath.c_str(), dlerror());
            }
            getModule fun = (getModule)dlsym(handle, "getModules");
#endif //WIN32
            tassert(fun, "get function:GetLogicModule error");
            iModule * logic = fun();
            tassert(logic, "can't get module from lib%s.so", path);

            while (logic) {
                MODULE_MAP::iterator itor = static_module_map.find(logic->getName());
                if (itor != static_module_map.end()) {
                    tassert(false, "module %s is exists", logic->getName());
                    continue;
                }

                static_module_map.insert(std::make_pair(logic->getName(), logic));
                static_module_list.push_back(logic);
                logic->initialize(core::getInstance());
                logic = logic->getNext();
            }


            module = module->NextSiblingElement("module");
        }

        MODULE_LIST::iterator itor = static_module_list.begin();
        MODULE_LIST::iterator iend = static_module_list.end();

        while (itor != iend) {
            (*itor)->launch(core);

            itor++;
        }

        return true;
    }

    void logic::shutdown() {
        MODULE_LIST::iterator itor = static_module_list.begin();
        MODULE_LIST::iterator iend = static_module_list.end();

        while (itor != iend) {
            (*itor)->destroy(core::getInstance());
            itor++;
        }

        while (itor != iend) {
            DEL *itor;
            itor++;
        }
    }

    iModule * logic::findModule(const char * name) {
        MODULE_MAP::iterator itor = static_module_map.find(name);
        if (itor == static_module_map.end() || nullptr == itor->second) {
            return nullptr;
        }

        return itor->second;
    }

    void logic::loadModule(const char * path) {
        std::string modulepath;
        modulepath.append(tools::file::getApppath()).append("/").append(path);
#ifdef WIN32
        HINSTANCE hinst = ::LoadLibrary(modulepath.c_str());
        if (!hinst) {
            printf("load %s error %s\n", modulepath.c_str(), strerror(::GetLastError()));
        }
        getModule fun = (getModule)::GetProcAddress(hinst, "getModules");
#else
        void * handle = dlopen(modulepath.c_str(), RTLD_LAZY);
        if (!handle) {
            printf("load %s error %s\n", modulepath.c_str(), dlerror());
        }
        getModule fun = (getModule)dlsym(handle, "getModules");
#endif //WIN32            
        tassert(fun, "get function:GetLogicModule error");
        iModule * logic = fun();
        tassert(logic, "can't get module from lib%s.so", path);

        while (logic) {
            MODULE_MAP::iterator itor = static_module_map.find(logic->getName());
            if (itor != static_module_map.end()) {
                tassert(false, "module %s is exists", logic->getName());
                continue;
            }

            static_module_map.insert(std::make_pair(logic->getName(), logic));
            static_module_list.push_back(logic);
            logic->initialize(core::getInstance());
            logic = logic->getNext();
        }
    }
}
