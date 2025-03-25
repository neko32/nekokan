#include <iostream>
#include <filesystem>
#include <string>
#include <cstdlib>
#include <optional>
#include <cassert>
#include <vector>
#include <algorithm>
using namespace std;

optional<filesystem::path> get_path(const string& env_name) {
    char *vals = getenv(env_name.c_str());
    if(vals == NULL) {
        return nullopt;
    } else {
        return filesystem::path {vals};
    }
}

int main() {
    vector<optional<filesystem::path>> paths {};
    paths.push_back(get_path("HOME_TMP_DIR"));
    paths.push_back(get_path("TANUPROJ_HOME"));
    paths.push_back(get_path("NEKOKAN_INDB_PATH"));
    paths.push_back(get_path("NEKOKAN_BIN_DIR"));
    paths.push_back(get_path("NEKOKAN_LIB_DIR"));
    paths.push_back(get_path("NEKOKAN_CONF_DIR"));

    assert(all_of(paths.begin(), paths.end(), [](optional<filesystem::path>& p){return p.has_value();}));
    
    for(optional<filesystem::path>& f:paths) {
        filesystem::path& path = f.value();
        if(filesystem::create_directories(path)) {
            cout << path.string() << " created." << endl;
        } else {
            cout << path.string() << " already exists. Skipped." << endl;
        }
    }

    cout << "All done." << endl;
}