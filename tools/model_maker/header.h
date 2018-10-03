#ifndef __header_h__
#define __header_h__

static const char * filehead = "#ifndef __attribute_h__\n\
#define __attribute_h__\n\
#include \"multisys.h\"\n\
#pragma pack(push, 1)\n\
namespace dc {\n\
    enum {\n\
        type_s64,\n\
        type_string,\n\
\n\
        type_can_not_be_key,\n\
        type_float = type_can_not_be_key,\n\
        type_bool,\n\
    };\n\
\n\
    struct layout {\n\
        const std::string _name; \n\
        const s16 _index;\n\
        const s32 _offset;\n\
        const s8 _type;\n\
        const s16 _size;\n\
        const bool _visual; \n\
        const bool _share;\n\
        const bool _save;\n\
        const bool _important;\n\
\n\
        layout(const char * name, s16 index, s32 offset, s8 type, s16 size, bool visual, bool share, bool save, bool important) : _name(name), _index(index), _offset(offset), _type(type), _size(size), _visual(visual), _share(share), _save(save), _important(important) {}\n\
    };\n";

    static const char * fileend = "\n\
}\n\
#pragma pack(pop)\n\
#endif //__attribute_h__\n\
";

#endif //__header_h__
