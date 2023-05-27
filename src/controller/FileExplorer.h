//
// Created by symbolic on 23-5-27.
//

#ifndef OS_FINAL_FILEEXPLORER_H
#define OS_FINAL_FILEEXPLORER_H
#include "HardDiskManager.h"
#include <map>
namespace file_system {
    class FileExplorer {
        HardDiskManager &hard_disk_mgr_;

    public:
        FileExplorer(HardDiskManager &hard_disk_mgr) : hard_disk_mgr_(hard_disk_mgr) {}
    };
}// namespace file_system


#endif//OS_FINAL_FILEEXPLORER_H
