#include <iostream>
#include <sqlite3.h>
#include <string>
#include <format>
#include <filesystem>
using namespace std;

int get_deps_from_db(void *param, int col_cnt, char** data, char** col_name) {
    const string deps = data[0] == nullptr ? "": data[0];
    *((string*)param) = deps;
    return 0;
}

void get_dependency(const string& mod_name, string* dep) {
        sqlite3 *db = nullptr;
        char *err_msg = nullptr;
        string indb_path_str {getenv("NEKOKAN_INDB_PATH")};
        filesystem::path indb_path {indb_path_str};
        int ret = sqlite3_open((indb_path / "nekokan" / "db" / "catalog.sqlite3").c_str(), &db);
        if(ret != SQLITE_OK) {
            throw runtime_error("catalog db open failed");
        }
        string sql = "select dependencies from Catalog";

        ret = sqlite3_exec(db, sql.c_str(), get_deps_from_db, (void*)(dep), &err_msg);
        sqlite3_close(db);
    }

int main(int argc, char **argv) {
    if(argc != 1) {
        cout << "depgen [app/lib_name]" << endl;
        exit(1);
    }

    const string mod_name {argv[1]};
    cout << format("retriving dependencies for {}.. ", mod_name) << endl;

    string deps{};

    get_dependency(mod_name, &deps);

    cout << format("dependencies from catalog: {}", deps) << endl;
}

/*
g++-13 -std=c++20 -Wall -Wextra -g -pthread  -Iinclude -I/opt/libnekokan/lib/include -I/opt/libnekokan/lib/include/nekokan -I/opt/libnekokan/lib/include/nekokan/base64_sample_app -I/opt/libnekokan/lib/include/nlohmann -I/opt/libnekokan/lib/include/nlohmann/json -I/opt/libnekokan/lib/include/base64_sample -I/opt/libnekokan/lib/include/nekokan_music -I/opt/libnekokan/lib/include/cpptanu_cfg -I/opt/libnekokan/lib/include/base64 -I/opt/libnekokan/lib/include/tobiaslocker -I/opt/libnekokan/lib/include/tobiaslocker/base64 -I/opt/libnekokan/lib/include/base64_sample_app -o output/base64_sample_bin src/base64_sample.o -lpqxx -lpq -lcpprest -lpthread -lssl -lcrypto -lcppunit -L/opt/libnekokan/lib -L/opt/libnekokan/lib/include -L/opt/libnekokan/lib/include/nekokan -L/opt/libnekokan/lib/include/nekokan/base64_sample_app -L/opt/libnekokan/lib/include/nlohmann -L/opt/libnekokan/lib/include/nlohmann/json -L/opt/libnekokan/lib/include/base64_sample -L/opt/libnekokan/lib/include/nekokan_music -L/opt/libnekokan/lib/include/cpptanu_cfg -L/opt/libnekokan/lib/include/base64 -L/opt/libnekokan/lib/include/tobiaslocker -L/opt/libnekokan/lib/include/tobiaslocker/base64 -L/opt/libnekokan/lib/include/base64_sample_app
*/