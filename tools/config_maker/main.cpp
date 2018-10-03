#include "tools.h"
#include "header.h"
#include "xmlmaker.h"

struct memberinfo {
    const std::string _type;
    const std::string _name;
    const std::string _desc;
    memberinfo(std::string type, std::string name, std::string desc) : _type(type), _name(name), _desc(desc) {}
};

struct structinfo {
    std::string _name;
    std::vector<memberinfo> _mbinfo;
};

std::map<std::string, structinfo> struct_info_map;

bool readStructs(const std::string & path, const std::string & struct_name, std::string & value, std::string & macro_extern, std::string & function, std::string & iconfig, std::string & oconfig) {

    std::vector<std::string> column_names;
    std::vector<std::string> column_types;
    std::vector<std::string> column_struct;
    std::vector<std::string> column_descs;


    Excel::Book book(path);
    Excel::Sheet * sheet = book.sheet(0);
    if (sheet->rowsCount() < 2 || sheet->columnsCount() < 3) {
        return false;
    }

    s32 index = 0;
    while (index < sheet->rowsCount()) {
        structinfo info;
        info._name = tools::toMultiString(sheet->cell(index++, struct_name_column).getString().c_str());
        value << "struct " << info._name << " {\n";
        const s32 count = sheet->cell(index++, struct_count_column).getDouble();
        for (s32 j = 0; j < count; j++) {
            const std::string type = tools::toMultiString(sheet->cell(index, member_type_column).getString().c_str());
            const std::string name = tools::toMultiString(sheet->cell(index, member_name_column).getString().c_str());
            const std::string desc = tools::toMultiString(sheet->cell(index++, member_desc_column).getString().c_str());
            value << "/*" << desc << "*/\n" << "    " << type << " _" << name << ";\n";
            info._mbinfo.push_back(memberinfo(type, name, desc));
        }
        value << "};\n";
        index++;
        struct_info_map.insert(std::make_pair(info._name, info));
    }

    return true;
}

bool readExcel(const std::string & path, const std::string & struct_name, std::string & value, std::string & macro_extern, std::string & function, std::string & iconfig, std::string & oconfig) {

    std::vector<std::string> column_names;
    std::vector<std::string> column_types;
    std::vector<std::string> column_struct;
    std::vector<std::string> column_descs;

    try {
        Excel::Book book(path);
        Excel::Sheet * sheet = book.sheet(0);
        if (sheet->rowsCount() < 3 || sheet->columnsCount() < 2) {
            return false;
        }

        for (s32 i = 0; i < sheet->columnsCount(); i++) {
            std::string name = tools::toMultiString(sheet->cell(config_name_row, i).getString().c_str());
            column_names.push_back(name);
        }

        for (s32 i = 0; i < sheet->columnsCount(); i++) {
            std::string desc = tools::toMultiString(sheet->cell(config_struct_row, i).getString().c_str());
            column_struct.push_back(desc);
        }

        for (s32 i = 0; i < sheet->columnsCount(); i++) {
            std::string desc = tools::toMultiString(sheet->cell(config_desc_row, i).getString().c_str());
            column_descs.push_back(desc);
        }

        for (s32 i = 0; i < sheet->columnsCount(); i++) {
            std::string type = tools::toMultiString(sheet->cell(config_type_row, i).getString().c_str());
            if (type != "int" && type != "string" && type != "float" && type != "bool" && type != "map" && type != "vector") {
                tassert(false, "file %s column %d type %s error", path.c_str(), i, type.c_str());
            }

            if (type == "map" || type == "vector") {
                type << "<" << column_struct[i] << ">";
            }

            column_types.push_back(type);
        }

        value << "struct o_config_" << struct_name << " {\n";
        for (s32 i = 0; i < sheet->columnsCount(); i++) {
            value << "/*" << column_descs[i] << "*/\n    " << column_types[i] << " _" << column_names[i] << ";\n";
        }
        value << "};\n";
        
        value << "extern std::map<int, o_config_" << struct_name << "> g_config_" << struct_name << "_map;\n";
//         value << "inline const o_config_" << struct_name << " * get_" << struct_name << "_config(int id) { \n";
//         value << "    auto i = g_config_" << struct_name << "_map.find(id);\n";
//         value << "    if (i == g_config_" << struct_name << "_map.end()) { \n";
//         value << "        return nullptr; \n";
//         value << "    }\n";
//         value << "    return &(i->second); \n";
//         value << "}\n";

        macro_extern << "    std::map<int, o_config_" << struct_name << "> g_config_" << struct_name << "_map; \\\n";
        function << "    init_config_" << struct_name << "(path);\n";
        iconfig << "    virtual const std::map<int, o_config_" << struct_name << "> & get_" << struct_name << "_config() = 0;\n";
        oconfig << "    virtual const std::map<int, o_config_" << struct_name << "> & get_" << struct_name << "_config() { return g_config_" << struct_name << "_map; } \\\n";

        value << "inline void init_config_" << struct_name << "(const std::string & path) {\n";
        value << "    std::string xmlpath;\n    xmlpath << path << \"/\" << \"" << struct_name << ".xml\";\n";
        value << "    TiXmlDocument config;\n";
        value << "    if (!config.LoadFile(xmlpath.c_str())) {\n";
        value << "        tassert(false, \"load config %s error\", xmlpath.c_str());\n";
        value << "        return;\n";
        value << "    }\n\n";

        value << "    TiXmlElement * root = config.RootElement();\n";
        value << "    TiXmlElement * " << struct_name << " = root->FirstChildElement(\"" << struct_name << "\");\n";
        value << "    while(" << struct_name << ") {\n";
        value << "        o_config_" << struct_name << " o" << struct_name <<";\n";
        for (s32 i = 0; i < column_names.size(); i++) {
            if (column_types[i] == "int") {
                value << "        o" << struct_name << "._" << column_names[i] << " = tools::stringAsInt(" << struct_name << "->Attribute(\"" << column_names[i] << "\"));\n";
            }
            else if (column_types[i] == "string") {
                value << "        o" << struct_name << "._" << column_names[i] << " = " << struct_name << "->Attribute(\"" << column_names[i] << "\");\n";
            }
            else if (column_types[i] == "float") {
                value << "        o" << struct_name << "._" << column_names[i] << " = tools::stringAsFloat(" << struct_name << "->Attribute(\"" << column_names[i] << "\"));\n";
            }
            else if (column_types[i].substr(0, 3) == "map") {
                value << "        {\n";
                osplitres res;
                tools::split(column_struct[i], ",", res);
                tassert(res.size() == 2, "wtf");

                std::string keytype = res[0];
                std::string valuename = res[1];
                value << "            std::string str = " << struct_name << "->Attribute(\"" << column_names[i] << "\");\n";
                value << "            oSplitRes arrays;\n";
                value << "            tools::Split(str, \";\", arrays);\n";
                value << "            for (s32 i = 0; i < arrays.size(); i++) {\n";
                value << "                oSplitRes key_value;\n";
                value << "                tools::Split(arrays[i], \":\", key_value);\n";

                if (keytype == "int") {
                    value << "                " << keytype << " key = tools::stringAsInt(key_value[0].c_str());\n";
                } else if (keytype == "float") {
                    value << "                " << keytype << " key = tools::stringAsFloat(key_value[0].c_str());\n";
                } else if (keytype == "string") {
                    value << "                " << keytype << " key = key_value[0];\n";
                }

                value << "                oSplitRes values;\n";
                value << "                tools::Split(key_value[1], \",\", values);\n";

                const structinfo info = struct_info_map[valuename];
                value << "                " << valuename << " o" << valuename << ";\n";
                for (s32 index = 0; index < info._mbinfo.size(); index++) {
                    if (info._mbinfo[index]._type == "int") {
                        value << "                o" << valuename << "._" << info._mbinfo[index]._name << " = tools::stringAsInt(values[" << index << "].c_str());\n";
                    } else if (info._mbinfo[index]._type == "float") {
                        value << "                o" << valuename << "._" << info._mbinfo[index]._name << " = tools::stringAsFloat(values[" << index << "].c_str());\n";
                    } else if (info._mbinfo[index]._type == "string") {
                        value << "                o" << valuename << "._" << info._mbinfo[index]._name << " = values[" << index << "];\n";
                    }
                }

                value << "                o" << struct_name << "._" << column_names[i] << ".insert(make_pair(key, o" << valuename << "));\n";
                value << "            }\n";
                value << "        }\n";
            }
            else if (column_types[i].substr(0, 6) == "vector") {
                structinfo info = struct_info_map[column_struct[i]];
                value << "        {\n";
                value << "            std::string str = " << struct_name << "->Attribute(\"" << column_names[i] << "\");\n";
                value << "            oSplitRes arrays;\n";
                value << "            tools::Split(str, \";\", arrays);\n";

                std::string structname = column_struct[i];
                const s32 count = struct_info_map[structname]._mbinfo.size();
                value << "            for (s32 i = 0; i < arrays.size(); i++) {\n";
                if (column_struct[i] == "int") {
                    value << "                int value = tools::stringAsInt(arrays[i].c_str());\n";
                } else if (column_struct[i] == "float") {
                    value << "                float value = tools::stringAsFloat(arrays[i].c_str());\n";
                } else if (column_struct[i] == "string") {
                    value << "                std::string value = arrays[i].c_str();\n";
                }
                else {
                    value << "                oSplitRes values;\n";
                    value << "                tools::Split(arrays[i], \",\", values);\n";
                    value << "                " << column_struct[i] << " value;\n";
                    for (s32 j = 0; j < count; j++) {
                        if (struct_info_map[structname]._mbinfo[j]._type == "int") {
                            value << "                value._" << struct_info_map[structname]._mbinfo[j]._name << " = tools::stringAsInt(values[" << j << "].c_str());\n";
                        }
                        else if (struct_info_map[structname]._mbinfo[j]._type == "float") {
                            value << "                value._" << struct_info_map[structname]._mbinfo[j]._name << " = tools::stringAsFloat(values[" << j << "].c_str());\n";
                        }
                        else if (struct_info_map[structname]._mbinfo[j]._type == "string") {
                            value << "                value._" << struct_info_map[structname]._mbinfo[j]._name << " = values[" << j << "].c_str();\n";
                        }
                    }
                }

                value << "                o" << struct_name << "._" << column_names[i] << ".push_back(value);\n";
                value << "            }\n";
                value << "        }\n";
            }
        }

        value << "        " << struct_name << " = " << struct_name << "->NextSiblingElement(\"" << struct_name << "\");\n";
        value << "        g_config_" << struct_name << "_map.insert(std::make_pair(o" << struct_name << "._" << column_names[0] << ", o" << struct_name << "));\n";
        value << "    }\n";
        value << "}\n";

        printf("%s\n", value.c_str());
        return true;
    }
    catch (const Excel::Exception & x) {
        return false;
    }
    catch (const CompoundFile::Exception & x) {
        return false;
    }
    catch (const std::exception &) {
        return false;
    }

    return true;
}


bool readGlobalExcel(const std::string & path, const std::string & struct_name, std::string & value, std::string & macro_extern, std::string & function, std::string & iconfig, std::string & oconfig) {

    std::vector<std::string> column_names;
    std::vector<std::string> column_types;
    std::vector<std::string> column_descs;
    std::vector<std::string> column_datas;

    try {
        Excel::Book book(path);
        Excel::Sheet * sheet = book.sheet(0);

        printf("sheet row %d : clm %d\n", sheet->rowsCount(), sheet->columnsCount());

        if (sheet->rowsCount() != 5) {
            return false;
        }

        value << "struct o_global_config {\n";
        for (s32 i = 0; i < sheet->columnsCount(); i++) {
            std::string name = tools::toMultiString(sheet->cell(config_name_row, i).getString().c_str());
            column_names.push_back(name);


            std::string type = tools::toMultiString(sheet->cell(config_type_row, i).getString().c_str());
            if (type != "int" && type != "string" && type != "float" && type != "bool") {
                tassert(false, "file %s column %d type %s error", path.c_str(), i, type.c_str());
            }
            column_types.push_back(type);


            std::string desc = tools::toMultiString(sheet->cell(config_desc_row, i).getString().c_str());
            column_descs.push_back(desc);


            std::string data = tools::toMultiString(sheet->cell(config_data_begin, i).getString().c_str());
            column_datas.push_back(data);

            value << "/*" << column_descs[i] << "*/\n    " << column_types[i] << " _" << column_names[i] << ";\n";
        }

        value << "};\n";

        value << "extern o_global_config g_global_config;\n";
        macro_extern << "    o_global_config g_global_config; \\\n";
 
        function << "    init_global_config(path);\n";
        iconfig << "    virtual const o_global_config & get_global_config() = 0;\n";
        oconfig << "    virtual const o_global_config & get_global_config() { return g_global_config; } \\\n";
 
        value << "inline void init_global_config(const std::string & path) {\n";
        value << "    std::string xmlpath;\n    xmlpath << path << \"/\" << \"global.xml\";\n";
        value << "    TiXmlDocument config;\n";
        value << "    if (!config.LoadFile(xmlpath.c_str())) {\n";
        value << "        tassert(false, \"load config %s error\", xmlpath.c_str());\n";
        value << "        return;\n";
        value << "    }\n\n";

        value << "    TiXmlElement * root = config.RootElement();\n";
        value << "    TiXmlElement * global = root->FirstChildElement(\"global\");\n";
        value << "    if (global) {\n";
        for (s32 i = 0; i < column_names.size(); i++) {
            if (column_types[i] == "int") {
                value << "        g_global_config._" << column_names[i] << " = tools::stringAsInt(" << struct_name << "->Attribute(\"" << column_names[i] << "\"));\n";
            }
            else if (column_types[i] == "string") {
                value << "        g_global_config._" << column_names[i] << " = " << struct_name << "->Attribute(\"" << column_names[i] << "\");\n";
            }
            else if (column_types[i] == "float") {
                value << "        g_global_config._" << column_names[i] << " = tools::stringAsFloat(" << struct_name << "->Attribute(\"" << column_names[i] << "\"));\n";
            }
        }

        value << "    }\n";
        value << "}\n";
 
        return true;
    }
    catch (const Excel::Exception & x) {
        return false;
    }
    catch (const CompoundFile::Exception & x) {
        return false;
    }
    catch (const std::exception &) {
        return false;
    }

}

int main() {
    std::string path = tools::file::getApppath();
    tools::file::opaths oPaths;
    tools::file::onames oNames;
    s32 nCount = 0;

    std::string content;
    std::string macro_extern;
    std::string function;
    std::string iconfig;
    std::string oconfig;

    content << "#ifndef __auto_config_h__ \n";
    content << "#define __auto_config_h__ \n";
    content << "// tools from sisyphus \n";
    content << "#include <map>\n" << "#include <string>\n\n" << "#include \"tools.h\"\n"<< "#include \"tinyxml/tinyxml.h\"\n\n" << "using namespace tools;\nusing namespace std;\n\n";

    macro_extern << "#define config_init_macro ";
    function << "inline void initconfig(const std::string & path) {\n";
    iconfig << "class iConfig {\n";
    iconfig << "public:\n";
    iconfig << "    virtual ~iConfig() {}\n";

    oconfig << "#define define_oconfig class OConfig : public iConfig { \\\n";
    oconfig << "public: \\\n";
    oconfig << "    virtual ~OConfig() {} \\\n";

    std::string structsPath = tools::file::getApppath();
    structsPath << "/" << "structs.xls";

    if (!readStructs(structsPath, "structs", content, macro_extern, function, iconfig, oconfig)) {
        tassert(false, "read excel %s error", "structs");
        return -1;
    }

    if (tools::file::getfiles(path.c_str(), ".xls", oPaths, oNames, nCount)) {
        for (s32 i = 0; i < oPaths.size(); i++) {
            if (oNames[i] == "global") {
                if (!readGlobalExcel(oPaths[i], oNames[i], content, macro_extern, function, iconfig, oconfig)) {
                    tassert(false, "read excel %s error", oNames[i].c_str());
                    return -1;
                }
            } else if (oNames[i] == "structs") {
                
            } else {
                if (!readExcel(oPaths[i], oNames[i], content, macro_extern, function, iconfig, oconfig)) {
                    tassert(false, "read excel %s error", oNames[i].c_str());
                    return -1;
                }
            }


            std::string xmlpath;
            xmlpath << tools::file::getApppath() << "/";
            if (!createxml(oPaths[i], oNames[i], xmlpath)) {
                tassert(false, "read excel %s error", oNames[i].c_str());
                return -1;
            }
            content << "\n";
        }
    }

    content << "#endif //__auto_config_h__";

    function << "}\n";
    iconfig << "};\n";
    oconfig << "};\n";

    std::string config_path;
    config_path << tools::file::getApppath() << "/auto_config.h";
    tlib::cfile file(config_path.c_str(), true);
    file << content.c_str() << "\n";
    file << macro_extern.c_str() << "\n\n";
    file << function.c_str() << "\n\n";
    file << iconfig.c_str() << "\n\n";
    file << oconfig.c_str() << "\n\n";
    file.save();
    file.close();

    return 0;
}