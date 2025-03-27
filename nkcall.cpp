#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace nekokan::installer {

    enum class LibType {
        REGULAR_LIB,
        HEADER_ONLY_LIB
    };

    class CatalogItem {
    private:
        int m_id;
        string m_name;
        string m_description;
        string m_location;
        vector<int> m_dependencies; 
    public:
        explicit CatalogItem(
            int id, 
            const string& name,
            const string& descr,
            const string& loc,
            const vector<int> dependencies
        ):m_id(id), m_name(name), m_description(descr), m_location(loc), \
        m_dependencies(dependencies) {}

        int id() const { return m_id; }
        const string& name() const {return m_name;} 
        const string& description() const {return m_description;} 
        const string& location() const {return m_location;} 
        const vector<int>& dependencies() const {return m_dependencies;} 
    };

}

int main(int argc, char** argv) {
    if(argc != 2) {
        cout << "nkcall [app/lib name]" << endl;
        exit(1);
    }
    const string name {argv[1]};
    cout << name << endl;
}