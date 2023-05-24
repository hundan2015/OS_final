cmake_minimum_required(VERSION 3.25)
project(OS_final)

set(CMAKE_CXX_STANDARD 17)

add_executable(OS_final main.cpp src/model/FCB.h src/model/Block.h src/controller/FileManager.cpp src/controller/FileManager.h src/model/Folder.h)
