#ifndef __iPublic_h__
#define __iPublic_h__

#include "define.h"

#define getUnitProtoData(unit, data) { \
    const std::vector<dc::layout> & layout = unit->getLayout(); \
    oIdentity * oid = (data).mutable_id(); \
    set_oIdentity(oid, unit->getid()); \
    for (s32 i = 0; i < layout.size(); i++) { \
        if (layout[i]._visual && layout[i]._share) { \
            oAttribute attr; \
            switch (layout[i]._type) { \
            case dc::type_s64: { \
                attr.set_type(eType::__int); \
                attr.set_name(layout[i]._name); \
                attr.mutable_value()->set_value_int(unit->getAttrInt64(layout[i])); \
                break; \
            } \
            case dc::type_string: { \
                attr.set_type(eType::__string); \
                attr.set_name(layout[i]._name); \
                attr.mutable_value()->set_value_string(unit->getAttrString(layout[i])); \
                break; \
            } \
            case dc::type_float: { \
                attr.set_type(eType::__float); \
                attr.set_name(layout[i]._name); \
                attr.mutable_value()->set_value_float(unit->getAttrFloat(layout[i])); \
                break; \
            } \
            } \
            (*(data).mutable_attres())[layout[i]._name] = attr; \
        } \
    } \
}

#endif //__iPublic_h__
