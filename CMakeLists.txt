cmake_minimum_required(VERSION 3.22)
project(OS_final)

set(CMAKE_CXX_STANDARD 17)

add_executable(OS_final main.cpp src/model/FCB.h src/model/Block.h src/controller/HardDiskManager.cpp src/controller/HardDiskManager.h src/model/Folder.h src/controller/FileExplorer.cpp src/controller/FileExplorer.h)
add_executable(HardDiskTest test/HardDiskTest.cpp)