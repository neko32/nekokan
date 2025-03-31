#include "nlohmann/json/json.hpp"
using json = nlohmann::json;

#include <string>
#include <iostream>
using namespace std;

int main() {
    string jstr = R"(
{
    "tako":1,
    "neko":"meow"
}
)";
    auto js = json::parse(jstr);
    cout << js.dump(4) << endl;

}