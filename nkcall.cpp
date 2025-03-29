#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <sqlite3.h>

using namespace std;

namespace nekokan::installer {

    enum class LibType {
        REGULAR_LIB,
        HEADER_ONLY_LIB
    };

    static unordered_map<string, LibType> libtype_str_map {
        {"regular_library", LibType::REGULAR_LIB},
        {"header_only_library", LibType::HEADER_ONLY_LIB}
    };

    string from_libtype_to_str(LibType libtype) {
        switch(libtype) {
            case LibType::REGULAR_LIB:
                return "regular_library";
            case LibType::HEADER_ONLY_LIB:
                return "header_only_library";
            default:
                throw runtime_error("not supported libtype");
        }
    }

    LibType from_str_to_libtype(const string libtype) {
        string libtype_s {libtype};
        transform(libtype_s.begin(), libtype_s.end(), libtype_s.begin(), ::tolower);   
        try {
            return libtype_str_map[libtype_s];
        } catch(...) {
            throw runtime_error(libtype_s + " not supported");
        }
    }

    class CatalogItem {
    private:
        int m_id;
        string m_name;
        string m_display_name;
        string m_description;
        string m_repo_url;
        string m_code_location;
        LibType m_libtype;
        vector<int> m_dependencies; 
        int m_special_install_instruction_id;
    public:
        explicit CatalogItem(
            int id, 
            const string& name,
            const string& display_name,
            const string& descr,
            const string& repo_url,
            const string& code_loc,
            LibType libtype,
            const vector<int> dependencies,
            int special_install_instruction_id
        ):m_id(id), m_name(name), m_display_name(display_name), m_description(descr), m_repo_url(repo_url), m_code_location(code_loc), \
        m_libtype(libtype), m_dependencies(dependencies), \
        m_special_install_instruction_id(special_install_instruction_id){}

        int id() const { return m_id; }
        const string& name() const {return m_name;} 
        const string& display_name() const {return m_display_name;}
        const string& description() const {return m_description;} 
        const string& repo_url() const {return m_repo_url;}
        const string& code_location() const {return m_code_location;} 
        const LibType libtype() const {return m_libtype;}
        const vector<int>& dependencies() const {return m_dependencies;} 
        const int special_install_instruction_id() const {return m_special_install_instruction_id;}

        const string to_str() const {
            stringstream ss;
            ss << "{";
            ss << "id:" << m_id << "," << "name:" << m_name << "," << "display_name:" << m_display_name << ",";
            ss << "description:" << m_description << "," << "repo_url:" << m_repo_url << ",";
            ss << "code_location:" << m_code_location << "," << "libtype:" << from_libtype_to_str(m_libtype) << ",";
            ss << "dependencies:[";
            stringstream deps;
            for_each(m_dependencies.begin(), m_dependencies.end(), [&deps](int dep){deps << dep << ",";});
            string depss = deps.str();
            if(depss.size() > 0 && depss[depss.size() - 1] == ',') depss.pop_back();
            ss << depss;
            ss << "],special_install_instruction_id:" << m_special_install_instruction_id;
            ss << "}";
            return ss.str();
        }
    };

    int get_catalog_from_db(void *param, int col_cnt, char** data, char** col_name) {
        const int id = atoi(data[0]);
        const string name = data[1];
        const string display_name = data[2];
        const string description = data[3];
        const string repo_url = data[4];
        const string code_location = data[5];
        const LibType libtype = from_str_to_libtype(data[6]);
        // [TODO] parse deps
        const string deps = data[7] == nullptr ? "": data[7];
        const int special_inst_id = data[8] == nullptr ? -1: atoi(data[8]);
        ((vector<CatalogItem>*)param)->push_back(
            CatalogItem {
                id,
                name,
                display_name,
                description,
                repo_url,
                code_location,
                libtype,
                {0, 1, 2},
                special_inst_id
            }
        );
        return 0;
    }

    void get_catalog_data(vector<CatalogItem>& catalog) {
        sqlite3 *db = nullptr;
        char *err_msg = nullptr;
        int ret = sqlite3_open("db/catalog.sqlite3", &db);
        if(ret != SQLITE_OK) {
            throw runtime_error("catalog db open failed");
        }
        string sql = "select id,name,display_name,description,repo_url,code_location,\
        libtype,dependencies,special_install_instruction_id from Catalog";

        ret = sqlite3_exec(db, sql.c_str(), get_catalog_from_db, (void*)(&catalog), &err_msg);

        sqlite3_close(db);
    }

}

int main(int argc, char** argv) {
    if(argc != 2) {
        cout << "nkcall [app/lib name]" << endl;
        exit(1);
    }
    const string name {argv[1]};
    cout << name << endl;

    vector<nekokan::installer::CatalogItem> catalogs;
    nekokan::installer::get_catalog_data(catalogs);

    for(const auto& cat:catalogs) {
        cout << cat.to_str() << endl;
    }
    
}