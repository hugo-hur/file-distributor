#pragma once

#include <filesystem>
#include <map>
#include <vector>

class file_dist{
    private:
    std::filesystem::path mountpoint;
    std::map<std::string,std::filesystem::directory_entry> real_mountpoints;

    std::vector<std::filesystem::path> filelists;
    std::vector<std::filesystem::path> medialists;

    public:
    enum target_fs_type{
        ltfs,
        other
    };
    //file_dist(std::filesystem::path mountpoint, std::vector<std::filesystem::directory_entry> targets);
    file_dist(std::filesystem::path mountpoint);
    ~file_dist();
    void mount_and_add_device(std::filesystem::directory_entry path, target_fs_type type = other);
};