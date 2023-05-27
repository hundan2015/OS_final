//
// Created by Symbolic on 2023/5/24.
//

#ifndef OS_FINAL_HARDDISKMANAGER_H
#define OS_FINAL_HARDDISKMANAGER_H
#include "../model/Block.h"
#include "../model/FCB.h"
#include "../model/Folder.h"
#include <memory>
#include <stack>
namespace file_system {
    const int hard_disk_size = 512;
    class HardDiskManager {
        std::array<Block, hard_disk_size> hard_disk;
        SuperBlock super_block;
        void InitSuperBlock() {
            super_block.target_block = &hard_disk[0];
            memcpy(hard_disk[0].data, super_block.GetBlockData().get(), block_size);
            for (short i = 1; i < hard_disk_size; ++i) {
                ReturnBlock(i);
            }
        }
        /**
         * The method get an empty block.
         * @return The handle of an empty block.
         */
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
        /**
         * The method using to return a block which no longer need.
         * @param block_num The handle of target block.
         */
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
        /**
         * Get a handle of new file index.
         * @return The handle of file index.
         */
        short CreateFileIndex() {
            short index = GetBlock();
            memcpy(hard_disk[index].data, FileIndex().GetBlockData().get(), block_size);
            return index;
        }
        void ReturnFileIndexCascade(short file_index_handle) {
            std::stack<short> file_index_handle_stack;
            FileIndex root;
            do {
                file_index_handle_stack.push(root.next_index_handle);
                root = FileIndex(&hard_disk[file_index_handle]);
            } while (root.next_index_handle != -1);

            while (!file_index_handle_stack.empty()) {
                ReturnBlock(file_index_handle_stack.top());
                file_index_handle_stack.pop();
            }
        }

    public:
        HardDiskManager() {
            Format();
        }
        /**
         * The basic format function.
         */
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
        /**
         * Write data to the target file.
         * @param file_index Target file index.
         * @param data The data need to write.
         * @param size The size the data.
         */
        void WriteFile(FileIndex *file_index, char *data, unsigned int size) {
            // Read the file index
            for (auto &i: file_index->index) {
                if (i == -1) {// If the data is larger than the previous file, get a new block.
                    i = GetBlock();
                }
                if (size > block_size) {//Judge the rest data whether larger than the block size.
                    size -= block_size;
                    memcpy(hard_disk[i].data, data, block_size);
                    data += block_size;
                } else {
                    memcpy(hard_disk[i].data, data, size);
                    break;
                }
            }

            if (size <= 0) {
                // Maybe the original data is larger than now.
                if (file_index->next_index_handle != -1) {
                    ReturnFileIndexCascade(file_index->next_index_handle);
                    file_index->next_index_handle = -1;
                }
                return;
            }

            short next_file_index_handle;
            if (file_index->next_index_handle != -1) {
                // If code runs to here, means it need a new file index.
                next_file_index_handle = CreateFileIndex();
            } else {
                next_file_index_handle = file_index->next_index_handle;
            }
            auto next_file_index = FileIndex(&hard_disk[next_file_index_handle]);
            WriteFile(&next_file_index, data, size);
        }
        /**
         * Read the data from target file index.
         * @param file_index Target file index.
         * @param size The site of the data's size. You can use it after reading.
         * @return
         */
        char *ReadFile(FileIndex *file_index, unsigned int &size) {
            std::string data_string;
            FileIndex *current_file_index = file_index;
            std::vector<FileIndex *> file_index_ptr;
            do {
                for (const auto &i: current_file_index->index) {
                    if (i != -1) {
                        std::string block_data_str(hard_disk[i].data, hard_disk[i].data + block_size);
                        data_string += block_data_str;
                    } else {
                        break;
                    }
                }
                if (current_file_index->next_index_handle != -1) {
                    auto *next_file_index = new FileIndex(&hard_disk[current_file_index->next_index_handle]);
                    file_index_ptr.emplace_back(next_file_index);
                    current_file_index = next_file_index;
                }
            } while (current_file_index->next_index_handle != -1);
            char *data_ptr = new char[data_string.size()];
            size = data_string.size();
            memcpy(data_ptr, data_string.c_str(), data_string.size());
            return data_ptr;
        }
        /**
         * Create file
         * @return
         */
        FCB *CreateFile(char *name) {
            FCB *result = new FCB();
            memcpy(result->name, name, sizeof(result->name));
            result->index_handle = CreateFileIndex();
            return nullptr;
        }
        File *ReadFile(FCB *fcb) {
            auto result = new File();
            unsigned int size = 0;
            FileIndex fcb_file_index(&hard_disk[fcb->index_handle]);
            result->data = std::unique_ptr<char[]>(ReadFile(&fcb_file_index, size));
            result->data_size = size;
            return result;
        }
    };
}// namespace file_system


#endif//OS_FINAL_HARDDISKMANAGER_H
