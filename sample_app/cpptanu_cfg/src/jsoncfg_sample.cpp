#include "cpptanu_cfg/cfg_read.h"
#include <iostream>
#include <string>
using namespace std;

int main() {
    
    tanu::cfg::JSONConfig json_cfg {"sample", "jsoncfg"};
    json_cfg.load("sample.json");
    
    cout << json_cfg.dump_cfg().value() << endl;

    cout << json_cfg.get_as_str("detail/lang") << endl;

}