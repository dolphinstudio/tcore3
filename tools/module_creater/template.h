#ifndef __template_h__
#define __template_h__

#include "multisys.h"

static const char * interface_content = 
"/* \n\
* File:   i{interface_name}.h\n\
* Author : {auther}\n\
*\n\
* Created on {datetime}\n\
*/\n\
\n\
#ifndef __i{interface_name}_h__\n\
#define __i{interface_name}_h__\n\
\n\
#include \"api.h\"\n\
\n\
class i{interface_name} : public iModule {\n\
public:\n\
    virtual ~i{interface_name}() {}\n\
};\n\
\n\
#endif //__i{interface_name}_h__\n\
";


static const char * header_content =
"/* \n\
* File:   header.h\n\
* Author : {auther}\n\
*\n\
* Created on {datetime}\n\
*/\n\
\n\
#ifndef __header_h__\n\
#define __header_h__\n\
\n\
#include \"tools.h\"\n\
#include \"i{interface_name}.h\"\n\
\n\
using namespace tcore;\n\
\n\
extern api::iCore * g_core;\n\
\n\
#endif //__header_h__\n\
";

static const char * h_content =
"/* \n\
* File:   {module_name}.h\n\
* Author : {auther}\n\
*\n\
* Created on {datetime}\n\
*/\n\
\n\
#ifndef __{module_name}_h__\n\
#define __{module_name}_h__\n\
\n\
#include \"header.h\"\n\
\n\
class {module_name} : public i{interface_name} {\n\
public:\n\
    virtual ~{module_name}() {}\n\
\n\
    virtual bool initialize(api::iCore * core);\n\
    virtual bool launch(api::iCore * core);\n\
    virtual bool destroy(api::iCore * core);\n\
};\n\
\n\
#endif //__{module_name}_h__";

static const char * cpp_content =
"/* \n\
* File:   {module_name}.cpp\n\
* Author : {auther}\n\
*\n\
* Created on {datetime}\n\
*/\n\
\n\
#include \"{module_name}.h\"\n\
\n\
api::iCore * g_core = nullptr;\n\
\n\
bool {module_name}::initialize(api::iCore * core) {\n\
    g_core = core;\n\
    return true;\n\
}\n\
\n\
bool {module_name}::launch(api::iCore * core) {\n\
    return true;\n\
}\n\
\n\
bool {module_name}::destroy(api::iCore * core) {\n\
    return true;\n\
}";

static const char * main_content =
"/* \n\
* File:   main.cpp\n\
* Author : {auther}\n\
*\n\
* Created on {datetime}\n\
*/\n\
\n\
#include \"{module_name}.h\"\n\
\n\
get_dll_instance;\n\
create_module({module_name});\n\
";

static const char * cmake_content =
"#cmake file for project core\n\
#author : {auther}\n\
#created : {datetime}\n\
\n\
set({module_name}\n\
    \"${PROJECT_SOURCE_DIR}/logic/{module_name}/{module_name}.h\"\n\
    \"${PROJECT_SOURCE_DIR}/logic/{module_name}/{module_name}.cpp\"\n\
    \"${PROJECT_SOURCE_DIR}/logic/{module_name}/main.cpp\"\n\
)\n\
\n\
source_group(api FILES ${api})\n\
source_group(common FILES ${common})\n\
source_group(interface FILES ${interface})\n\
source_group(public FILES ${public})\n\
source_group(\\\\ FILES ${{module_name}})\n\
\n\
add_library({module_name} SHARED\n\
    ${{module_name}}\n\
    ${api}\n\
    ${public}\n\
    ${interface}\n\
    ${common}\n\
)\n\
\n\
set_property(TARGET {module_name} PROPERTY FOLDER \"{group}\")\n\
";
#endif //__template_h__
