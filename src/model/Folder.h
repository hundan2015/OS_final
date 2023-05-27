//
// Created by Symbolic on 2023/5/24.
//

#ifndef OS_FINAL_FOLDER_H
#define OS_FINAL_FOLDER_H
#include "FCB.h"
#include <vector>
namespace file_system {
    class IFile {
        virtual std::unique_ptr<char[]> GetFileData() = 0;
        virtual unsigned int GetFileSize() = 0;
    };
    class File : public IFile {
    public:
        std::unique_ptr<char[]> data;
        unsigned int data_size;
        std::unique_ptr<char[]> GetFileData() override {
            std::unique_ptr<char[]> res_data(new char[data_size]);
            memcpy(res_data.get(), data.get(), data_size);
            return res_data;
        }
        unsigned int GetFileSize() override {
            return data_size;
        }
    };
    class Folder : public IFile {
        std::vector<FCB> fcb_vec;

    public:
        Folder(char *data, int block_count) {
            const unsigned int fcb_count = block_count * block_size / sizeof(FCB);
            for (int i = 0; i < fcb_count; ++i) {
                memcpy(&fcb_vec[i], data, sizeof(FCB));
            }
        }
        Folder() {
            fcb_vec = std::vector<FCB>(block_size / sizeof(FCB));
        }
        std::unique_ptr<char[]> GetFileData() override {
            const unsigned int size = fcb_vec.size();
            const unsigned int data_size = size * sizeof(FCB);
            std::unique_ptr<char[]> data(new char[data_size]);
            for (int i = 0; i < size; ++i) {
                memcpy(data.get() + sizeof(FCB) * i, &fcb_vec[i], sizeof(FCB));
            }
            /*// Fill the rest make it to a full block.
            const unsigned int shit = size * sizeof(FCB) % block_size;
            auto temp = FCB();
            for (int i = 0; i < shit; ++i) {
                memcpy(data.get(), &temp, sizeof(FCB));
            }*/
            return data;
        }
        unsigned int GetFileSize() override {
            const unsigned int size = fcb_vec.size();
            const unsigned int data_size = size * sizeof(FCB);
            return data_size;
        }
    };
}// namespace file_system


#endif//OS_FINAL_FOLDER_H
