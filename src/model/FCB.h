//
// Created by Symbolic on 2023/5/24.
//

#ifndef OS_FINAL_FCB_H
#define OS_FINAL_FCB_H
#include "Block.h"
#include "array"
#include "iostream"
namespace file_system {
    const int index_size = block_size / sizeof(short) - 1;
    enum FileType { NORMAL,
                    FOLDER,
                    SPECIAL };
    class FCB {
        char name[14];
        short index_handle = -1;//-1~32767
        int permission = -1;
        FileType file_type = NORMAL;
        char pattern[8];
    };
    //TODO: Maybe need an active fcb.


    class SuperBlock {
    public:
        Block *target_block = nullptr;
        short counter = 1;
        short next_super_block = -1;
        static const int valid_block_size = block_size / sizeof(short) - 2;
        std::array<short, valid_block_size> valid_block = {};
        SuperBlock(Block *block) {
            target_block = block;
            memcpy(&counter, block->data, sizeof(short));
            memcpy(&next_super_block, block->data + sizeof(short), sizeof(short));
            memcpy(valid_block.data(), block->data + 2 * sizeof(short), sizeof(short) * valid_block_size);
        }
        std::unique_ptr<char[]> GetBlockData() {
            std::unique_ptr<char[]> data(new char[block_size]);
            memcpy(data.get(), &counter, sizeof(short));
            memcpy(data.get() + sizeof(short), &next_super_block, sizeof(short));
            memcpy(data.get() + 2 * sizeof(short), valid_block.data(), sizeof(short) * valid_block_size);
            return data;
        }
        SuperBlock() = default;
        /**
         * Using a method called RAII. When the super block need to
         * be destroyed, it would auto change the original block data.
         */
        ~SuperBlock() {
            std::cout << "Super block dead." << std::endl;
            if (target_block)
                memcpy(target_block->data, GetBlockData().get(), block_size);
        }
    };

    class FileIndex {// Should be as same as block's size
    public:
        Block *target_block = nullptr;
        std::array<short, index_size> index = {};
        short next_index_handle = -1;

        FileIndex() {
            index.fill(-1);
        }
        FileIndex(Block *block) {
            target_block = block;
            memcpy(index.data(), block->data, sizeof(short) * index_size);
            memcpy(&next_index_handle, block->data + sizeof(short) * index_size, sizeof(short));
        }
        /**
         * Get a unique ptr of FileIndex data.
         * The life cycle of the data should be managed by the caller,
         * which means the data should only be copied but not be assigned.
         * @return The data ptr.
         */
        std::unique_ptr<char[]> GetBlockData() {
            std::unique_ptr<char[]> data(new char[block_size]);
            memcpy(data.get(), index.data(), sizeof(short) * index_size);
            memcpy(data.get() + sizeof(short) * index_size, &next_index_handle, sizeof(short));
            return data;
        }
        ~FileIndex() {
            if (target_block && GetBlockData())
                memcpy(target_block->data, GetBlockData().get(), block_size);
        }
    };

}// namespace file_system

#endif//OS_FINAL_FCB_H
