//
// Created by Symbolic on 2023/5/24.
//

#ifndef OS_FINAL_FOLDER_H
#define OS_FINAL_FOLDER_H
#include "FCB.h"
#include <map>
#include <vector>
namespace file_system {
    class IFile {
    public:
        std::unique_ptr<FileIndex> target_file_index;
        virtual std::unique_ptr<char[]> GetFileData() = 0;
        virtual unsigned int GetFileSize() = 0;
    };
    class File : public IFile {
    public:
        std::unique_ptr<char[]> data_;
        unsigned int data_size;
        File(FileIndex *target_file_index_, char *data, unsigned int size) {
            target_file_index = std::unique_ptr<FileIndex>(target_file_index_);
            data_size = size;
            data_ = std::unique_ptr<char[]>(new char[size]);
            memcpy(data_.get(), data, size);
        }
        std::unique_ptr<char[]> GetFileData() override {
            std::unique_ptr<char[]> res_data(new char[data_size]);
            memcpy(res_data.get(), data_.get(), data_size);
            return res_data;
        }
        unsigned int GetFileSize() override {
            return data_size;
        }
    };
    class Folder : public IFile {
        void InitFolder(FCB parent_fcb, FCB this_fcb) {
            fcb_map.insert(std::make_pair("..", parent_fcb));
            fcb_map.insert(std::make_pair(".", this_fcb));
            this_fcb.size = GetFileSize();
        }

    public:
        std::map<std::string, FCB> fcb_map;
        Folder(FileIndex *target_file_index_, char *data, unsigned int size, FCB parent_fcb, FCB this_fcb) {
            target_file_index = std::unique_ptr<FileIndex>(target_file_index_);
            const unsigned int fcb_count = size / sizeof(FCB);
            int counter = 0;
            /*for () {
                memcpy(&fcb_map[i], data + sizeof(FCB) * i, sizeof(FCB));, FCB parent_fcb
            }*/
            for (int i = 0; i < fcb_count; ++i) {
                FCB temp;
                memcpy(&temp, data + counter * sizeof(FCB), sizeof(FCB));
                fcb_map.insert(std::make_pair(std::string(temp.name), temp));
                counter++;
            }
            InitFolder(parent_fcb, this_fcb);
        }
        Folder() {
            //InitFolder();
            //fcb_map = std::vector<FCB>(block_size / sizeof(FCB));
        }
        Folder(FCB parent_fcb, FCB this_fcb) {
            //target_file_index =
            char parent_dir_name[] = "..";
            char this_dir_name[] = ".";
            memcpy(parent_fcb.name, parent_dir_name, sizeof(parent_dir_name));
            memcpy(this_fcb.name, this_dir_name, sizeof(this_dir_name));
            InitFolder(parent_fcb, this_fcb);
        }

        Folder(File *folder_file) {
            target_file_index = std::move(folder_file->target_file_index);
            const unsigned int fcb_count = folder_file->GetFileSize() / sizeof(FCB);
            auto data = folder_file->GetFileData();
            int counter = 0;
            for (int i = 0; i < fcb_count; ++i) {
                FCB temp;
                memcpy(&temp, data.get() + counter * sizeof(FCB), sizeof(FCB));
                fcb_map.insert(std::make_pair(std::string(temp.name), temp));
                counter++;
            }
            fcb_map.erase("");
        }
        Folder(File *folder_file, FCB this_fcb) {
            target_file_index = std::move(folder_file->target_file_index);
            const unsigned int fcb_count = folder_file->GetFileSize() / sizeof(FCB);
            auto data = folder_file->GetFileData();
            /*for (int i = 0; i < fcb_count; ++i) {
                memcpy(&fcb_map[i], data.get() + sizeof(FCB) * i, sizeof(FCB));
            }*/
            int counter = 0;
            for (int i = 0; i < fcb_count; ++i) {
                FCB temp;
                memcpy(&temp, data.get() + counter * sizeof(FCB), sizeof(FCB));
                fcb_map.insert(std::make_pair(std::string(temp.name), temp));
                counter++;
            }
            /*if(fcb_map.find(".")==fcb_map.end()){
                fcb_map.insert(std::make_pair(".", this_fcb));
            }*/
            this_fcb.size = GetFileSize();
            //InitFolder(parent_fcb, this_fcb);
        }
        std::unique_ptr<char[]> GetFileData() override {
            const unsigned int size = fcb_map.size();
            const unsigned int data_size = size * sizeof(FCB);
            std::unique_ptr<char[]> data(new char[data_size]);
            int counter = 0;
            for (auto &i: fcb_map) {
                memcpy(data.get() + counter * sizeof(FCB), &(i.second), sizeof(FCB));
                counter++;
            }
            return data;
        }
        unsigned int GetFileSize() override {
            const unsigned int size = fcb_map.size();
            const unsigned int data_size = size * sizeof(FCB);
            return data_size;
        }
        void InsertFile(FCB fcb) {
            std::string name(fcb.name);
            fcb_map.insert(std::make_pair(name, fcb));
        }
        ~Folder() {
        }
    };
}// namespace file_system


#endif//OS_FINAL_FOLDER_H
