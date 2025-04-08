#include "ben-strasser/fast-cpp-csv-parser/csv.h"
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <format>
using namespace std;

int main() {
    string rc_path = getenv("NEKORC_PATH");
    filesystem::path csv_file = filesystem::path(rc_path) / "sample" / "csv" / "myneko.csv";

    io::CSVReader<2> csv_in{csv_file.string()};
    csv_in.read_header(io::ignore_extra_column, "age", "name");
    string name;
    int age;
    int row = 1;
    while(csv_in.read_row(age, name)) {
        cout << format("{}:{{age:{},name:{}}}", row++, age, name) << endl;
    }
    
}