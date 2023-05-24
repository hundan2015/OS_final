//
// Created by Symbolic on 2023/5/24.
//

#ifndef OS_FINAL_FILEMANAGER_H
#define OS_FINAL_FILEMANAGER_H
#include "../model/Block.h"
#include "../model/FCB.h"
#include "../model/Folder.h"
namespace file_system {
    const int hard_disk_size = 512;
    class FileManager {
        std::array<Block, hard_disk_size> hard_disk;
        SuperBlock super_block;
        void InitSuperBlock() {
            super_block.target_block = &hard_disk[0];
            memcpy(hard_disk[0].data, super_block.GetBlockData().get(), block_size);
            for (short i = 1; i < hard_disk_size; ++i) {
                ReturnBlock(i);
            }
        }
        short GetBlock() {
            //Read Super block.
            //SuperBlock super_block(&hard_disk[0]);
            short result;
            if (super_block.counter == 1) {
                SuperBlock next_super_block(&hard_disk[super_block.next_super_block]);
                result = super_block.next_super_block;
                super_block = next_super_block;
                next_super_block.target_block = nullptr;
            } else {
                result = super_block.valid_block[super_block.counter - 1];
                super_block.counter--;
            }
            return result;
        }
        void ReturnBlock(short block_num) {
            //SuperBlock super_block(&hard_disk[0]);
            if (super_block.counter >= SuperBlock::valid_block_size) {
                memcpy(hard_disk[block_num].data, super_block.GetBlockData().get(), block_size);
                super_block = SuperBlock();
                super_block.next_super_block = block_num;
                super_block.target_block = &hard_disk[0];
            } else {
                super_block.valid_block[super_block.counter] = block_num;
                super_block.counter++;
            }
        }

    public:
        FileManager() {
            Format();
        }
        void Format() {
            hard_disk.fill(Block());
            /**
             * | super block | root_dir_index | root_dir_file_block0 | ....... |
             * |      0      |       1        |          2           | ....... |
             */
            //TODO:Init Super block.
            InitSuperBlock();
            //Init root dir index.
            FileIndex root_file_index;
            memcpy(hard_disk[1].data, root_file_index.GetBlockData().get(), block_size);
            root_file_index.target_block = &hard_disk[1];
            // Init root dir file.
            Folder dir_folder;
            auto dir_folder_data = dir_folder.GetFileData();
            WriteFile(&root_file_index, dir_folder_data.get(), dir_folder.GetFileSize());
            std::cout << "Format finished." << std::endl;
        }
        void WriteFile(FileIndex *file_index, char *data, unsigned int size) {
            for (auto &i: file_index->index) {
                if (i == -1) {
                    i = GetBlock();
                }
                if (size > block_size) {
                    size -= block_size;
                    memcpy(hard_disk[i].data, data, block_size);
                    data += block_size;
                } else {
                    memcpy(hard_disk[i].data, data, size);
                    return;
                }
            }
            auto next_file_index_handle = CreateFileIndex();
            auto next_file_index = FileIndex(&hard_disk[next_file_index_handle]);
            WriteFile(&next_file_index, data, size);
        }
        //TODO: Make read file.
        char *ReadFile(FileIndex *file_index) {
            return nullptr;
        }
        short CreateFileIndex() {
            short index = GetBlock();
            memcpy(hard_disk[index].data, FileIndex().GetBlockData().get(), block_size);
            return index;
        }
    };
}// namespace file_system


#endif//OS_FINAL_FILEMANAGER_H
