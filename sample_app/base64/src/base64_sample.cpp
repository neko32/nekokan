#include "tobiaslocker/base64/base64.hpp"

#include <iostream>
using namespace std;

int main() {
    auto encoded_str =  base64::to_base64("nekode gozai");
    std::cout << encoded_str << std::endl;
    auto decoded_str = base64::from_base64(encoded_str);
    std::cout << decoded_str << std::endl;
}