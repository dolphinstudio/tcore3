#include "header.h"
#include "template.h"
#include "cfile.h"

input_args_map g_input_args;
std::string g_out_path;

static char toCapital(const char & letter) {
    if (letter >= 'a'&& letter <= 'z') {
        return letter - 32;
    }
    return letter;
}

int main(int argc, const char ** args, const char ** env) {
    parse(argc, args);

    if (getarg("del")) {
        string module_name = getarg("del");
        string interface_name;
        if (module_name.size() > 0) {
            interface_name = module_name;
            *(char *)(interface_name.c_str()) = toCapital(interface_name.c_str()[0]);
        }

        string path = tools::file::getApppath();
        string interface_file = path + "/interface/i" + interface_name + ".h";
        printf("interface : %s\n", interface_file.c_str());

        string module_dir = path + "/" + module_name;
        printf("module_dir : %s\n", module_dir.c_str());

        string module_header = module_dir + "/header.h";
        string module_h = module_dir + "/" + module_name + ".h";
        string module_cpp = module_dir + "/" + module_name + ".cpp";
        string module_main = module_dir + "/main.cpp";
        string module_cmake = module_dir + "/CMakeLists.txt";

        tools::file::delfile(interface_file.c_str());
        tools::file::delfile(module_header.c_str());
        tools::file::delfile(module_h.c_str());
        tools::file::delfile(module_cpp.c_str());
        tools::file::delfile(module_main.c_str());
        tools::file::delfile(module_cmake.c_str());

        tools::file::deldir(module_dir.c_str());

        string cmake_manager_file = path + "/CMakeLists.txt";
        tlib::cfile cmake_manager;
        cmake_manager.open(cmake_manager_file.c_str());
        string data;
        bool res = cmake_manager.readtostring(data);
        cmake_manager.close();
        string temp = "\n    \"${PROJECT_SOURCE_DIR}/logic/interface/i" + interface_name + ".h\"";
        tools::stringReplase(data, temp, "");
        string temp2;
        temp2.append("\n").append("ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/").append(module_name).append(")");
        tools::stringReplase(data, temp2, "");
        tools::stringReplase(data, "\r", "");
        cmake_manager.open(cmake_manager_file.c_str(), true);
        cmake_manager << tlib::cdata((void *)data.c_str(), data.size());
        cmake_manager.save();
        cmake_manager.close();
        return 0;
    }

    string auther = getarg("auther");
    string datetime = tools::time::getCurrentTimeString();

    string module_name = getarg("name");
    string group = getarg("group");
    string interface_name;
    
    if (module_name.size() > 0) {
        interface_name = module_name;
        *(char *)(interface_name.c_str()) = toCapital(interface_name.c_str()[0]);
    }

    string path = tools::file::getApppath();

    string interface_file = path + "/interface/i" + interface_name + ".h";
    printf("interface : %s\n", interface_file.c_str());

    string module_dir = path + "/" + module_name;
    printf("module_dir : %s\n", module_dir.c_str());

    if (tools::file::exists(interface_file) || tools::file::exists(module_dir)) {
        printf("module %s interface file or dir already exists, push any key quit", module_name.c_str());
        getchar();
        exit(1);
    }

    string module_header = module_dir + "/header.h";
    printf("module_header : %s\n", module_header.c_str());

    string module_h = module_dir + "/" + module_name + ".h";
    printf("module_h : %s\n", module_h.c_str());

    string module_cpp = module_dir + "/" + module_name + ".cpp";
    printf("module_cpp : %s\n", module_cpp.c_str());

    string module_main = module_dir + "/main.cpp";
    printf("module_main : %s\n", module_main.c_str());

    string module_cmake = module_dir + "/CMakeLists.txt";
    printf("module_cmake : %s\n", module_cmake.c_str());


    if (tools::file::exists(interface_file)
        || tools::file::exists(module_dir)
        || tools::file::exists(module_header)
        || tools::file::exists(module_h)
        || tools::file::exists(module_cpp)) {
        tassert(false, "module file exists");
        printf("module file exists, please check\n");
        return -1;
    }

    std::string cinterface(interface_content);
    tools::stringReplase(cinterface, "{auther}", auther);
    tools::stringReplase(cinterface, "{datetime}", datetime);
    tools::stringReplase(cinterface, "{interface_name}", interface_name);

    std::string cheader(header_content);
    tools::stringReplase(cheader, "{auther}", auther);
    tools::stringReplase(cheader, "{datetime}", datetime);
    tools::stringReplase(cheader, "{interface_name}", interface_name);

    std::string ch(h_content);
    tools::stringReplase(ch, "{auther}", auther);
    tools::stringReplase(ch, "{datetime}", datetime);
    tools::stringReplase(ch, "{interface_name}", interface_name);
    tools::stringReplase(ch, "{module_name}", module_name);

    std::string ccpp(cpp_content);
    tools::stringReplase(ccpp, "{auther}", auther);
    tools::stringReplase(ccpp, "{datetime}", datetime);
    tools::stringReplase(ccpp, "{module_name}", module_name);

    std::string cmain(main_content);
    tools::stringReplase(cmain, "{auther}", auther);
    tools::stringReplase(cmain, "{datetime}", datetime);
    tools::stringReplase(cmain, "{module_name}", module_name);

    std::string ccmake(cmake_content);
    tools::stringReplase(ccmake, "{auther}", auther);
    tools::stringReplase(ccmake, "{datetime}", datetime);
    tools::stringReplase(ccmake, "{module_name}", module_name);
    tools::stringReplase(ccmake, "{group}", group);


    tlib::cfile file_interface(interface_file.c_str(), true);
    file_interface << cinterface.c_str();
    file_interface.save();
    file_interface.close();

    tools::file::mkdir(module_dir.c_str());

    tlib::cfile file_header(module_header.c_str(), true);
    file_header << cheader.c_str();
    file_header.save();
    file_header.close();

    tlib::cfile file_h(module_h.c_str(), true);
    file_h << ch.c_str();
    file_h.save();
    file_h.close();

    tlib::cfile file_cpp(module_cpp.c_str(), true);
    file_cpp << ccpp.c_str();
    file_cpp.save();
    file_cpp.close();

    tlib::cfile file_main(module_main.c_str(), true);
    file_main << cmain.c_str();
    file_main.save();
    file_main.close();

    tlib::cfile file_cmake(module_cmake.c_str(), true);
    file_cmake << ccmake.c_str();
    file_cmake.save();
    file_cmake.close();

    string cmake_manager_file = path + "/CMakeLists.txt";
    tlib::cfile cmake_manager;
    cmake_manager.open(cmake_manager_file.c_str());
    string data;
    bool res = cmake_manager.readtostring(data);
    cmake_manager.close();
    string temp = "${interface}\n    \"${PROJECT_SOURCE_DIR}/logic/interface/i" + interface_name + ".h\"";
    tools::stringReplase(data, "${interface}", temp);
    data.append("\n").append("ADD_SUBDIRECTORY(${PROJECT_SOURCE_DIR}/logic/").append(module_name).append(")");
    tools::stringReplase(data, "\r", "");
    cmake_manager.open(cmake_manager_file.c_str(), true);
    cmake_manager << tlib::cdata((void *)data.c_str(), data.size());

    return 0;
}
