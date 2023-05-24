//
// Created by Symbolic on 2023/5/24.
//

#ifndef OS_FINAL_BLOCK_H
#define OS_FINAL_BLOCK_H
#define BLOCK short;
namespace file_system {
    const int block_size = 1024;
    class Block {
    public:
        char data[block_size];
    };

}// namespace file_system

#endif//OS_FINAL_BLOCK_H
