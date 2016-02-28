#include "myutil.h"
#include <iostream>

int main() {
    char buf[] = "1\n12\n123\n1234\n12345\n123456\n12345\n1234\n123\n12\n1\n0";
    char* pbuf = buf;
    auto vec = split_string(pbuf, '\n', 5);
    std::cout << vec << std::endl;
    vec = split_string(pbuf, '\n', 6);
    std::cout << vec << std::endl;
    vec = split_string(pbuf, '\n', 7);
    std::cout << vec << std::endl;
    return 0;

}
