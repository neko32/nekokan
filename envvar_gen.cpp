#include <iostream>
#include <fstream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <cstdlib>
#include <array>
#include <filesystem>
#include <pwd.h>
#include <grp.h>
#include <cstring>
using namespace std;

bool user_exist(const string& user_name) {
    const char* c_usr_name = user_name.c_str();
    struct passwd* pw = getpwnam(c_usr_name);
    return pw != nullptr;
}

bool group_exist(const string& group_name) {
    const char* c_group_name = group_name.c_str();
    struct group* grp = getgrnam(c_group_name);
    return grp != nullptr;
}

int main(int argc, char **argv) {

    array<char, 2048> buf;
    string home_dir;
    stringstream ss;

    string nk_admin_user {};
    string nk_admin_group {};

    if(argc == 2) {
        nk_admin_user = string(argv[1]);
        nk_admin_group = string(argv[1]);
    } else if(argc == 3) {
        nk_admin_user = string(argv[1]);
        nk_admin_group = string(argv[2]);
    } else {
        cerr << "evgen [nk admin user name] [nk admin group name]" << endl;
        return 1;
    }

    if(!user_exist(nk_admin_user) || !group_exist(nk_admin_group)) {
        cerr << "specified user or group doesn't exist" << endl;
        return 2;
    }
    cout << "nekokan admin user - " << nk_admin_user << endl;
    cout << "nekokan admin group - " << nk_admin_group << endl;
    
    // Preamble
    ss << "#!/bin/bash" << endl;

    // retrieve HOME_DIR
    unique_ptr<FILE, decltype(&pclose)> pipe(popen("realpath ~", "r"), pclose);
    
    if(!pipe) {
        throw runtime_error("failed to run realpth command to get home dir");
        exit(1);
    }

    while(fgets(buf.data(), buf.size(), pipe.get()) != nullptr) {
        home_dir = buf.data();
    }
    if(!home_dir.empty() && home_dir.back() == '\n') {
        home_dir.pop_back();
    }

    filesystem::path home_path = filesystem::path(home_dir);
    filesystem::path tanuproj_path = home_path / "dev";
    filesystem::path opt_path = filesystem::path("/opt");
    filesystem::path libnekokan_path = opt_path / "libnekokan" / "lib";

    // HOME_TMP_DIR
    ss << "# HOME_TMP_DIR" << endl;
    ss << "export HOME_TMP_DIR=" << (home_path / "tmp").string() << endl;

    ss << "# TANUPROJ_HOME" << endl;
    ss << "export TANUPROJ_HOME=" << tanuproj_path.string() << endl;

    // NEKOKAN_INDB_PATH
    ss << "# NEKOKAN_INDB_PATH" << endl;
    ss << "export NEKOKAN_INDB_PATH=" << (home_path / "nekokan_indb").string() << endl;

    // NEKOKAN_CODE_DIR
    ss << "# NEKOKAN_CODE_DIR" << endl;
    ss << "export NEKOKAN_CODE_DIR=" << (tanuproj_path / "nekokan").string() << endl; 

    // NEKOKAN_BIN_DIR
    ss << "# NEKOKAN_BIN_DIR" << endl;
    ss << "export NEKOKAN_BIN_DIR=" << (opt_path / "nekokan" / "bin").string() << endl;

    // NEKOKAN_CONF_DIR
    ss << "# NEKOKAN_CONF_DIR" << endl;
    ss << "export NEKOKAN_CONF_DIR=" << (opt_path / "nekorc").string() << endl;

    // NEKORC DIR
    ss << "# NEKORC_PATH" << endl;
    ss << "export NEKORC_PATH=" << (opt_path / "nekorc").string() << endl;

    // NEKOKAN_LIB_DIR
    ss << "# NEKOKAN_LIB_DIR" << endl;
    ss << "export NEKOKAN_LIB_DIR=" << libnekokan_path.string() << endl;

    // NEKOKAN_CMD_DIR
    ss << "# NEKOKAN_CMD_DIR" << endl;
    ss << "export NEKOKAN_CMD_DIR=" << (opt_path / "nkcmd").string() << endl;

    // TANULIB_CPP_CODE_DIR
    ss << "# TANULIB_CPP_CODE_DIR" << endl;
    ss << "export TANULIB_CPP_CODE_DIR=" << (tanuproj_path / "tanulib_cpp") << endl;

    // NEKOKAN_ADMIN_USER & NEKOKAN_ADMIN_GROUP
    ss << "# NEKOKAN_ADMIN_USER" << endl;
    ss << "export NEKOKAN_ADMIN_USER=" << nk_admin_user << endl;
    ss << "# NEKOKAN_ADMIN_GROUP" << endl;
    ss << "export NEKOKAN_ADMIN_GROUP=" << nk_admin_group << endl;

    // NEKOKAN_ENV
    ss << "# NEKOKAN_ENV" << endl;
    ss << "export NEKOKAN_ENV=local" << endl;

    // Update LD_LIBRARY_PATH
    ss << "# Prepend LIBNEKOKAN_DIR to LD_LIBRARY_PATH" << endl;
    ss << "export LD_LIBRARY_PATH=" << libnekokan_path.string() << ":${LD_LIBRARY_PATH}" << endl;

    // special command for LD_LIBRARY_PATH update
    ss << "# special command to optimie LD_LIBRARY_PATH for nekokan" << endl;
    ss << "alias setnkld=\"export LD_LIBRARY_PATH=" << libnekokan_path.string() << ":/usr/local/lib\"" << endl;


    // shortcut to nekokan
    ss << "# Shortcuts for nekokan" << endl;
    ss << "alias gonklib=\"cd ${NEKOKAN_LIB_DIR}\"" << endl;
    ss << "alias gonkbin=\"cd ${NEKOKAN_BIN_DIR}\"" << endl;
    ss << "alias gonkconf=\"cd ${NEKOKAN_CONF_DIR}\"" << endl;
    ss << "alias gonkproj=\"source gonkproj.bash\"" << endl;
    ss << "alias gonkrc=\"cd ${NEKORC_PATH}\"" << endl;
    ss << "alias rundbeaver=\"rundbeaver.bash\"" << endl;
    ss << "alias runollama=\"runollama.bash\"" << endl;
    ss << "alias stopollama=\"stopollama.bash\"" << endl;
    ss << "alias runnekomcphost=\"runnekomcphost.bash\"" << endl;
    cout << ss.str() << endl;

    // write the result to ~/.nekokan_include.bash
    filesystem::path inc_file_path = home_path / ".nekokan_include.bash";
    cout << "writing the above result to " << inc_file_path.string() << " ... " << endl;

    if(filesystem::exists(inc_file_path)) {
        if(!filesystem::remove(inc_file_path)) {
            throw runtime_error("failed to remove " + inc_file_path.string());
        }
    }
    ofstream fout;
    fout.open(inc_file_path.string(), ios::out);
    if(!fout.is_open()) {
        throw runtime_error("failed to open " + inc_file_path.string());
    }
    fout << ss.str() << endl;
    fout.close();

    // generate and show a code snippet for .bashrc
    string snippet = R"(
if [ -e ~/.nekokan_include.bash ]; then
  source ~/.nekokan_include.bash
  echo "${LD_LIBRARY_PATH} will be updated with the one optimized for nekokan. Pls revert back if needed"
  setnkld
  echo "new LD_LIBRARY_PATH is ${LD_LIBRARY_PATH}"
  export PATH=${PATH}:${NEKOKAN_CMD_DIR}
  export GO111MODULE=on
  export ANTHROPIC_API_KEY=tobeset
fi
    )";
    cout << "copy the below to your .bashrc or equiv:" << endl;
    cout << snippet << endl;

    cout << "all done." << endl;
}

