#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <filesystem>
#include <regex>
#include <curl/curl.h>
#include <format>
#include <sqlite3.h>

using namespace std;

namespace tanulib::net {

    size_t rget_callback(void* ptr, size_t size, size_t nb, void* stream) {
        ofstream* out = static_cast<ofstream*>(stream);
        size_t total_size = size * nb;
        out->write(static_cast<const char*>(ptr), total_size);
        return total_size;
    }

    string rget_text(const string& url) {
        CURL* curl;
        CURLcode curl_code;
        ofstream ofs {"/tmp/rget_tmp.tmp", ios::binary};

        curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, rget_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ofs);

        curl_code = curl_easy_perform(curl);
        if(curl_code != CURLE_OK) {
            throw runtime_error("rget by curl failed.");
        }

        curl_easy_cleanup(curl);
        ofs.close();

        ifstream ifs("/tmp/rget_tmp.tmp", ios::binary);
        if(!ifs) {
            throw runtime_error("opening tmp file failed");
        }
        vector<unsigned char> buf {istreambuf_iterator<char>(ifs), istreambuf_iterator<char>()};
        ostringstream oss;
        for(auto byte:buf) {
            oss << static_cast<char>(byte);
        }
        ifs.close();
        /*
        if(!filesystem::remove("/tmp/rget_tmp.tmp")) {
            throw runtime_error("failed to remove /tmp/rget_tmp.tmp");
        }
            */
        return oss.str();
    }

    class URLParser {
    private:
        string m_scheme;
        string m_host;
        int m_port;
        string m_path;
        string m_queries;
        string m_fragments;
        string m_raw_url;
    public:
        URLParser(const string& raw_url) {
            m_raw_url = raw_url;
            regex reg(R"(^(https?)://([^:/?#]+)(?::(\d+))?([^?#]*)(\?[^#]*)?(#.*)?)");
            smatch match_rez;
            if(regex_match(m_raw_url, match_rez, reg)) {
                this->m_scheme = match_rez[1].str();
                this->m_host = match_rez[2].str();
                string port_s = match_rez[3].str();
                this->m_port = port_s.empty() ? -1: stoi(match_rez[3].str());
                this->m_path = match_rez[4].str();
                this->m_queries = match_rez[5].str();
                this->m_fragments = match_rez[6].str();
            } else {
                throw runtime_error("parsing " + raw_url + " failed.");
            }
        }

        const string& scheme() const {return m_scheme;}
        const string& host() const {return m_host;}
        const int port() const{return m_port;}
        const string& path() const{return m_path;}
        const string& queries() const{return m_queries;}
        const string& fragments() const{return m_fragments;}
        const string path_last() const {
            int pos = m_path.find_last_of('/');
            if(pos == m_path.npos) {
                return m_path;
            } else {
                return string{m_path.substr(pos + 1)};
            }
        }
    };
}

namespace nekokan::installer {

    enum class LibType {
        REGULAR_LIB,
        HEADER_ONLY_LIB,
        EXECUTABLE,
        PY_PLAIN_SCRIPT,
        PY_PI_MODULE,
        PY_NEKOMCP_SERVER
    };

    static unordered_map<string, LibType> libtype_str_map {
        {"regular_library", LibType::REGULAR_LIB},
        {"header_only_library", LibType::HEADER_ONLY_LIB},
        {"executable", LibType::EXECUTABLE},
        {"python_plain_script", LibType::PY_PLAIN_SCRIPT},
        {"python_nekomcp_server", LibType::PY_NEKOMCP_SERVER},
        {"python_pi_module", LibType::PY_PI_MODULE}
    };

    string from_libtype_to_str(LibType libtype) {
        switch(libtype) {
            case LibType::REGULAR_LIB:
                return "regular_library";
            case LibType::HEADER_ONLY_LIB:
                return "header_only_library";
            case LibType::EXECUTABLE:
                return "executable";
            case LibType::PY_PLAIN_SCRIPT:
                return "python_plain_script";
            case LibType::PY_PI_MODULE:
                return "python_pi_module";
            case LibType::PY_NEKOMCP_SERVER:
                return "python_nekomcp_server";
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
        explicit CatalogItem(){}
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

    class Installer {
    protected:
        string m_remote_location;
        string m_install_dest;
        LibType m_libtype;
    public:
        Installer(const string& remote_loc, const string& install_dest, LibType libtype): m_remote_location(remote_loc), m_libtype(libtype) {
            char* lib_env_var = getenv("NEKOKAN_LIB_DIR");
            if(lib_env_var == nullptr) throw runtime_error("NEKOKAN_LIB_DIR env variable must be set.");
            string lib_env_s {lib_env_var};
            filesystem::path lib_path {lib_env_s};
            if(libtype == LibType::HEADER_ONLY_LIB) {
                lib_path /= "include";
            }
            lib_path /= install_dest;
            this->m_install_dest = lib_path.string();
        }

        virtual ~Installer(){}

        virtual bool install() = 0;
        virtual bool already_installed() {
            return filesystem::exists(this->m_install_dest);
        }
        const string& remote_location() {return m_remote_location;}
        const string& install_dest() {return m_install_dest;}
        LibType libtype() {return m_libtype;}
    };

    class HeaderOnlyInstaller:public Installer {
    public:
        HeaderOnlyInstaller(const string& remote_loc, const string& install_dest, LibType libtype): Installer(remote_loc, install_dest, libtype){}
        bool install() override {

            tanulib::net::URLParser urlparser {this->remote_location()};
            string file_name = urlparser.path_last();
            filesystem::path inst_path {this->m_install_dest};
            inst_path /= file_name;

            if(!filesystem::exists(inst_path.parent_path())) {
                if(!filesystem::create_directories(inst_path.parent_path())) {
                    throw runtime_error("creading directories (equiv to mkdir -p) for " + inst_path.parent_path().string() + " failed.");
                };
            }

            cout << "fetching library from " << this->m_remote_location << " ... " << endl;
            string fetched_data = tanulib::net::rget_text(this->m_remote_location);

            cout << "installing fetched header file to " << inst_path.string() << " ... " << endl;
            ofstream ofs {inst_path.string()};
            if(!ofs.is_open()) {
                throw runtime_error("failed to open " + inst_path.string());
            }
            ofs << fetched_data << endl;
            ofs.close();
            cout << "done." << endl;

            return true;
        }
    };

    class NekoMCPInstaller: public Installer {
    public:    
        NekoMCPInstaller(const string& remote_loc, const string& install_dest, LibType libtype): Installer(remote_loc, install_dest, libtype){
            char* bin_env_var = getenv("NEKOKAN_BIN_DIR");
            if(bin_env_var == nullptr) throw runtime_error("NEKOKAN_BIN_DIR env variable must be set.");
            string bin_env_s {bin_env_var};
            filesystem::path lib_path {bin_env_s};
            lib_path /= install_dest;
            this->m_install_dest = lib_path.string();
        }

        bool install() override {

            tanulib::net::URLParser urlparser {this->remote_location()};
            string file_name = urlparser.path_last();
            filesystem::path inst_path {this->m_install_dest};
            inst_path /= file_name;

            if(!filesystem::exists(inst_path.parent_path())) {
                if(!filesystem::create_directories(inst_path.parent_path())) {
                    throw runtime_error("creading directories (equiv to mkdir -p) for " + inst_path.parent_path().string() + " failed.");
                };
            }

            cout << "fetching mcp server code from " << this->m_remote_location << " ... " << endl;
            string fetched_data = tanulib::net::rget_text(this->m_remote_location);

            cout << "installing mcp server to " << inst_path.string() << " ... " << endl;
            ofstream ofs {inst_path.string()};
            if(!ofs.is_open()) {
                throw runtime_error("failed to open " + inst_path.string());
            }
            ofs << fetched_data << endl;
            ofs.close();
            cout << "done." << endl;

            return true;
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
        const string deps = data[7] == nullptr ? "": data[7];
        stringstream depss(deps);
        vector<int> dep_ids{};
        string t;
        while(getline(depss, t, ',')) {
            t.push_back(stoi(t));
        }
        const int special_inst_id = data[8] == nullptr ? -1: atoi(data[8]);
        ((unordered_map<string,CatalogItem>*)param)->insert({name, 
            CatalogItem {
                id,
                name,
                display_name,
                description,
                repo_url,
                code_location,
                libtype,
                dep_ids,
                special_inst_id
            }
        });
        return 0;
    }

    void get_catalog_data(unordered_map<string, CatalogItem>& catalog) {
        sqlite3 *db = nullptr;
        char *err_msg = nullptr;
        string indb_path_str {getenv("NEKOKAN_INDB_PATH")};
        filesystem::path indb_path {indb_path_str};
        int ret = sqlite3_open((indb_path / "nekokan" / "db" / "catalog.sqlite3").c_str(), &db);
        if(ret != SQLITE_OK) {
            throw runtime_error("catalog db open failed");
        }
        string sql = "select id,name,display_name,description,repo_url,code_location,\
        libtype,dependencies,special_install_instruction_id from Catalog";

        ret = sqlite3_exec(db, sql.c_str(), get_catalog_from_db, (void*)(&catalog), &err_msg);

        sqlite3_close(db);
    }

}

int cmd_uninstall(
    const string& name,
    unordered_map<string, nekokan::installer::CatalogItem>& catalogs
) {
    const nekokan::installer::CatalogItem& item = catalogs[name];
    const string lib_path_str {getenv("NEKOKAN_LIB_DIR")};
    filesystem::path lib_path {lib_path_str};
    const string bin_path_str {getenv("NEKOKAN_BIN_DIR")};
    filesystem::path bin_path {bin_path_str};

    stringstream ss {name};
    string package_root;
    filesystem::path delete_path;

    switch(item.libtype()) {
    case nekokan::installer::LibType::HEADER_ONLY_LIB:
        getline(ss, package_root, '/'); 
        delete_path = lib_path / "include" / package_root;
        cout << format("deleting {} ...", delete_path.string()) << endl;
        filesystem::remove_all(delete_path);
        cout << format("{}[install path:{}] was removed successfully.", name, delete_path.string()) << endl;
        return 0;
    case nekokan::installer::LibType::EXECUTABLE:
    case nekokan::installer::LibType::PY_NEKOMCP_SERVER:
        // if bin name has multiple directories like x/y, then take x to delete all from upper dir
        getline(ss, package_root, '/');
        delete_path = bin_path / package_root;
        cout << format("deleting {} ...", delete_path.string()) << endl;
        filesystem::remove_all(delete_path);
        cout << format("{}[install path:{}] was removed successfully.", name, delete_path.string()) << endl;
        return 0;
    }
    return 0;
}

int cmd_install(
    const string name,
    unordered_map<string, nekokan::installer::CatalogItem>& catalogs
) {

    const nekokan::installer::CatalogItem& item = catalogs[name];
    cout << name << " found. " << endl;
    cout << item.to_str() << endl << endl;

#ifdef __NK_DEBUG__
    for(const auto& [k, cat]:catalogs) {
        cout << k << ":" << cat.to_str() << endl;
    }
#endif

    nekokan::installer::Installer* installer = nullptr;
    bool already_exists = false;
    int ret_code = 0;
    switch(item.libtype()) {
        case nekokan::installer::LibType::REGULAR_LIB:
            // TODO
            break;
        case nekokan::installer::LibType::HEADER_ONLY_LIB:
            installer = new nekokan::installer::HeaderOnlyInstaller(
                item.code_location(),
                item.name(),
                item.libtype()
            );
            already_exists = installer->already_installed();
            cout << installer->install_dest() << " has already the lib? " << boolalpha << already_exists << endl;
            if(already_exists) {
                cout << "library " << item.display_name() << " has already been installed to " << installer->install_dest() << endl;
            } else {
                try {
                    if(!installer->install()) {
                        cerr << "installer couldn't completed installation." << endl;
                        ret_code = 1;
                    };
                } catch(const runtime_error& e) {
                    cerr << e.what() << endl;
                    ret_code = 1;
                }
            }
            break;
        case nekokan::installer::LibType::PY_NEKOMCP_SERVER:
            installer = new nekokan::installer::NekoMCPInstaller(
                item.code_location(),
                item.name(),
                item.libtype()
            );
            already_exists = installer->already_installed();
            cout << installer->install_dest() << " has already the mcp server? " << boolalpha << already_exists << endl;
            if(already_exists) {
                cout << "MCP Server " << item.display_name() << " has already been installed to " << installer->install_dest() << endl;
            } else {
                try {
                    if(!installer->install()) {
                        cerr << "installer couldn't completed installation." << endl;
                        ret_code = 1;
                    };
                } catch(const runtime_error& e) {
                    cerr << e.what() << endl;
                    ret_code = 1;
                }
            }
            break;
        default:
            cout << "not supported libtype?" << endl;
            ret_code = 2;
            break;
    }

    return ret_code;
}

int main(int argc, char** argv) {
    if(argc < 2) {
        cout << "nkcall [cmd] [optional, app/lib name]" << endl;
        exit(1);
    }

    const string cmd {argv[1]};

    unordered_map<string, nekokan::installer::CatalogItem> catalogs;
    nekokan::installer::get_catalog_data(catalogs);

    if(cmd == "list") {
        int counter = 1;
        for(const auto& [key, item]:catalogs) {
            cout << "[" << counter++ << "]:" << key << " -> " << item.to_str() << endl << endl;
        }
        exit(0);
    }

    if(argc < 3) {
        cerr << format("target is missing for command {}. Exitting..", cmd) << endl;
        exit(1);
    }

    string target {argv[2]};
    cout << format("cmd - {}, target - {}", cmd, target) << endl;

    auto found = catalogs.find(target);
    if(found == catalogs.end()) {
        cout << target << " was not found in Nekokan catalog. exitting.." << endl;
        exit(0);
    }

    if(cmd == "install") {
        return cmd_install(target, catalogs);
    } else if(cmd == "uninstall") {
        return cmd_uninstall(target, catalogs);
    } else {
        cout << cmd << " is not supported." << endl;
        return 1;
    }
    
}