
#include "file_dist.h"
#include <iostream>

int main(int argc, char* argv[]){
    if(argc < 3){
        std::cout << "Please give at least 2 arguments" << std::endl;
        return -1;
    }
    std::cout << "Got mountpoint " << argv[1] << std::endl;
    file_dist fd(argv[1]);
    for(int i = 2; i < argc; i++){
        fd.mount_and_add_device(std::filesystem::directory_entry(argv[i]),file_dist::ltfs);
    }
    //std::filesystem::directory_entry("/dev/sdb1");
    
    return 0;
}