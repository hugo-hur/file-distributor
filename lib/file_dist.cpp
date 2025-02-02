#include "file_dist.h"
#include <random>
#include <iostream>


static std::filesystem::path create_temporary_directory(
      unsigned long long max_tries = 10000) {
    auto tmp_dir = std::filesystem::temp_directory_path();
    unsigned long long i = 0;
    std::random_device dev;
    std::mt19937 prng(dev());
    std::uniform_int_distribution<uint64_t> rand(0);
    std::filesystem::path path;
    while (true) {
        std::stringstream ss;
        ss << std::hex << rand(prng);
        path = tmp_dir / ss.str();
        // true if the directory was created.
        if (std::filesystem::create_directory(path)) {
            break;
        }
        if (i == max_tries) {
            throw std::runtime_error("could not find non-existing directory name for temporary directory");
        }
        i++;
    }
    return path;
}

file_dist::file_dist(std::filesystem::path mountpoint):mountpoint(mountpoint)
{

}

file_dist::~file_dist()
{
    for(auto mount : real_mountpoints){
        
        FILE* pipe = popen((std::string("umount ") + mount.second.path().string()).c_str(), "r");
        if(pipe != NULL){
            pclose(pipe);
        }
    }
}

void file_dist::mount_and_add_device(std::filesystem::directory_entry path, target_fs_type type)
{
    FILE* pipe;

    auto temp_mount_dir_path = create_temporary_directory();

    if(type == other){
        pipe = popen((std::string("mount ") +
        path.path().string() + 
        ' ' + 
        temp_mount_dir_path.string()
        ).c_str(), "r");
    }
    else if(type == ltfs){
        pipe = popen((std::string("ltfs -o devname=") +
        path.path().string() +
        ' ' + 
        temp_mount_dir_path.string()
        ).c_str(), "r");
    }

    if (pipe == NULL)
    {
        throw new std::runtime_error(std::string("Could not open pipe to mount filesystem on device ") + path.path().string());
    }
    std::string output;
    char c;
    while(fread(&c, 1,1,pipe) == 1){
        output += c;
    }
    pclose(pipe);

    std::cout << output << std::endl;
    //this->real_mountpoints();
}
