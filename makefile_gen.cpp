#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>
#include <format>
#include <filesystem>
#include <unordered_map>
using namespace std;

static const string PLACEHOLDER_PACKAGE_NAME = "__NEKOKAN_PACKAGE_NAME__";
static const string PLACEHOLDER_BIN_NAME = "__BIN_NAME__";
static const string PLACEHOLDER_BIN_TYPE = "__BIN_TYPE__";
static const string PLACEHOLDER_NEKOKAN_DEST = "__NEKOKAN_DEST__";
static const string PLACEHOLDER_EXTRA_COMPILER_FLAG = "__EXTRA_COMPILER_FLAG__";
static const string PLACEHOLDER_LIB_PREFIX = "__LIB_PREFIX__";
static const string PLACEHOLDER_LIB_POSTFIX = "__LIB_POSTFIX__";

enum class BinType {
    A,
    SO,
    EXE
};

unordered_map<string, BinType> bin_type_map {
    {"a", BinType::A},
    {"so", BinType::SO},
    {"exe", BinType::EXE},
};

void set_bintype(string& s, const string& bin_type) {
    auto it = s.find(PLACEHOLDER_BIN_TYPE);
    if(it != s.npos) {
        s.replace(it, PLACEHOLDER_BIN_TYPE.length(), bin_type);
    }
}

void set_extra_compiler_flag(string& s, BinType bin_type) {
    string extra_compiler_flag {};
    switch(bin_type) {
    case BinType::SO:
        extra_compiler_flag = "-shared -fPIC";
    }
    size_t loc = 0;
    while((loc = s.find(PLACEHOLDER_EXTRA_COMPILER_FLAG)) != s.npos) {
        s.replace(loc, PLACEHOLDER_EXTRA_COMPILER_FLAG.length(), extra_compiler_flag);
    }
}

void set_lib_prefix(string &s, BinType bin_type) {
    string lib_prefix{};
    switch(bin_type) {
    case BinType::A:
        lib_prefix = "lib";
        break;
    case BinType::SO:
        lib_prefix = "lib";
        break;
    case BinType::EXE:
        break;
    }
    size_t loc = 0;
    while((loc = s.find(PLACEHOLDER_LIB_PREFIX)) != s.npos) {
        s.replace(loc, PLACEHOLDER_LIB_PREFIX.length(), lib_prefix);
    }
}

void set_lib_postfix(string &s, BinType bin_type) {
    string lib_postfix{};
    switch(bin_type) {
    case BinType::A:
        lib_postfix = ".a";
        break;
    case BinType::SO:
        lib_postfix = ".so";
        break;
    case BinType::EXE:
        break;
    }
    size_t loc = 0;
    while((loc = s.find(PLACEHOLDER_LIB_POSTFIX)) != s.npos) {
        s.replace(loc, PLACEHOLDER_LIB_POSTFIX.length(), lib_postfix);
    }
}

void set_package_bin_name(string &s, const string& package_name, const string& bin_name) {
    size_t loc = 0;
    while((loc = s.find(PLACEHOLDER_PACKAGE_NAME)) != s.npos) {
        s.replace(loc, PLACEHOLDER_PACKAGE_NAME.length(), package_name);
    }
    while((loc = s.find(PLACEHOLDER_BIN_NAME)) != s.npos) {
        s.replace(loc, PLACEHOLDER_BIN_NAME.length(), bin_name);
    }
}

int main(int argc, char **argv) {

    string bin_type_str {};
    string package_name {"NA"};
    string bin_name {"NA"};

    if(argc < 2) {
        cerr << "makefile_gen [a|so|exe] [package name(optional)] [bin name(optional)]" << endl;
        return 1;
    } else if(argc == 2) {
        bin_type_str = string{argv[1]};
    } else if(argc == 3) {
        bin_type_str = string{argv[1]};
        package_name = string{argv[2]};
    } else if(argc >= 4) {
        bin_type_str = string{argv[1]};
        package_name = string{argv[2]};
        bin_name = string{argv[3]};
    }
    transform(bin_type_str.begin(), bin_type_str.end(), bin_type_str.begin(), ::tolower);
    if(bin_type_map.find(bin_type_str) == bin_type_map.end()) {
        cerr << format("bin type {} not found", bin_type_str) << endl;
        return 1;
    }
    cout << "Bin type is " << bin_type_str << endl;
    BinType bin_type = bin_type_map[bin_type_str];

    filesystem::path rc_path {"resource"};
    ifstream ifs;
    ifs.open((rc_path / "Makefile_template").string());
    if(!ifs.is_open()) {
        cerr << "failed to open resource/Makefile_template" << endl;
        return 1;
    }
    string t;
    stringstream ss;
    while(getline(ifs, t)) {
        ss << t << endl;    
    }
    ifs.close();
    string makefile = ss.str();

    set_bintype(makefile, bin_type_str);
    set_extra_compiler_flag(makefile, bin_type);
    set_lib_prefix(makefile, bin_type);
    set_lib_postfix(makefile, bin_type);
    set_package_bin_name(makefile, package_name, bin_name);
    cout << makefile << endl;

    ofstream ofs;
    ofs.open("Makefile");
    if(!ofs.is_open()) {
        cerr << "failed to open FD for Makefile" << endl;
        return 1;
    }
    ofs << makefile << endl;
    ofs.close();

    cout << "done." << endl;

    return 0;
}