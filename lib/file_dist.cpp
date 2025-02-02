#include "file_dist.h"
#include <random>
#include <iostream>
#include <fstream>
#include <uuid/uuid.h>

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
            WEXITSTATUS(pclose(pipe));
        }
    }
}

void file_dist::mount_and_add_device(std::filesystem::directory_entry path, target_fs_type type)
{

    auto temp_mount_dir_path = create_temporary_directory();
    std::string mountstr;
    if(type == other){
        mountstr = "mount " +
        path.path().string() + 
        ' ' + 
        temp_mount_dir_path.string();
    }
    else if(type == ltfs){
        mountstr = "ltfs -o devname=" +
        path.path().string() +
        ' ' + 
        temp_mount_dir_path.string();
    }
    std::cout << "Mounting device with command: " << mountstr << std::endl;
    FILE* pipe = popen(mountstr.c_str(), "r");
    if (pipe == NULL)
    {
        throw new std::runtime_error(std::string("Could not open pipe to mount filesystem on device ") + path.path().string());
    }
    std::string output;
    char c;
    while(fread(&c, 1,1,pipe) == 1){
        output += c;
    }

    int mount_exit_code = WEXITSTATUS(pclose(pipe));

    std::cout << "MOUNT OUTPUT*******************************" << std::endl;
    std::cout << output << std::endl;
    std::cout << "MOUNT retcode: " << mount_exit_code << std::endl;

    std::cout << "Device mounted to: " << temp_mount_dir_path.string() << std::endl;
    //Read the special files
    auto filelistpath = temp_mount_dir_path / ".filelist";
    std::cout << "Filelistpath: " << filelistpath.string() << std::endl;
    auto medialistpath = temp_mount_dir_path / ".medialist";
    std::cout << "Medialistpath: " << medialistpath.string() << std::endl;

    std::string id;

    
    if(std::filesystem::exists(medialistpath)){
        try{
            std::stringstream buffer;
            buffer << std::ifstream(medialistpath).rdbuf();
            getline(buffer, id,';');//Get the first (this media id)

            //std::string medialist = buffer.str();
            std::cout << "Reading medialist from media: " << id << std::endl;
            std::string media_id;
            while(getline(buffer,media_id,';')){
                std::cout << "Found media: " << media_id << std::endl;
            }

        }
        catch(const std::ifstream::failure& e){
            throw;
        }
    }
    else{
        std::cout << "Medialist did not exist on the media! Creating new uuid for media" << std::endl;

        uuid_t uuid_out;
        uuid_generate_random(uuid_out);
        char uuid_text[37];
        uuid_unparse_lower(uuid_out, uuid_text);//Saves as null terminated string

        id = uuid_text;

        std::cout << "Creating new medialist at: " << medialistpath.string() << std::endl;
        //Write own id as first
        std::ofstream out(medialistpath);
        out << id + ";";
        out.close();
    }

    std::cout << "Media id: " << id << " mounted to: " << temp_mount_dir_path.string() << std::endl;

    std::cout << "Reading filelist from media: " << id << std::endl;
    if(std::filesystem::exists(filelistpath)){
        try{
            std::stringstream buffer;
        buffer << std::ifstream(filelistpath).rdbuf();
            std::string media_id,filename,filetype;
            while(getline(buffer,media_id,';')){
                getline(buffer,filename,';');
                getline(buffer,filetype,';');
                
                std::cout << "Found file: " << filename << " at media: " << media_id << " type: " << filetype << std::endl;
            }
        }
        catch(const std::ifstream::failure& e){
            throw;
        }
    }
    else{
        std::cout << "Filelist did not exist on the media! Creating one from own contents." << std::endl;
        std::ofstream out(filelistpath);
        for(const auto& p: std::filesystem::recursive_directory_iterator(temp_mount_dir_path)) {

            std::cout << "Checking: " << p.path().string() << std::endl;
            auto fspath = std::filesystem::relative(p.path(),temp_mount_dir_path);
            auto spath = fspath.string();

            out << id << ';' << spath << ';';
            if (std::filesystem::is_directory(p)) {
                std::cout << "Found directory at: " << spath << std::endl;
                out << "d;";
            }
            else{
                std::cout << "Found file at: " << spath << std::endl;
                out << "f;";
            }
            
        }
        std::cout << "Closing filelist" << std::endl;
        out.close();
    }
    //save the newly mounted path
    real_mountpoints.emplace(id, temp_mount_dir_path);
    std::cout << "Closing filelist" << std::endl;
}
