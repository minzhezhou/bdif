#include "record_block.h"
#include <iostream>

int main() {
    std::FILE* f = std::fopen("test.in", "r");
    std::vector<char> buf(100000); // char is trivally copyable
    int read_cnt = std::fread(&buf[0], sizeof buf[0], buf.size(), f);
    char* pbuf = &buf[0];

    buf[read_cnt] = 0;
    recblock blk;
    int cnt = blk.fill(pbuf);
    std::vector<string> valid_res;
    std::vector<string> invalid_res;
    int cntsum = cnt;
    while (cnt) {
        blk.split(valid_res, invalid_res);      
        char* orgp = pbuf;
        cnt = blk.fill(pbuf);
        cntsum += cnt;
    }
    std::cout << "cntsum" << cntsum << "\n";
    return 0;

}
