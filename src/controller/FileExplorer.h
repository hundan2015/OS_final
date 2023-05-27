//
// Created by symbolic on 23-5-27.
//

#ifndef OS_FINAL_FILEEXPLORER_H
#define OS_FINAL_FILEEXPLORER_H
#include "HardDiskManager.h"
#include <map>
#include <memory>
using std::cout;
using std::endl;
namespace file_system {
    class FileExplorer {
        HardDiskManager &hard_disk_mgr_;
        // file_first_index_handle -> fcb_ptr
        std::map<short, FCB *> active_fcb_map_;
        FCB *current_dir;
        FCB *last_dir_fcb = nullptr;
        std::shared_ptr<Folder> last_dir;

    public:
        FileExplorer(HardDiskManager &hard_disk_mgr) : hard_disk_mgr_(hard_disk_mgr) {
            //Contain a copy of root fcb.
            current_dir = hard_disk_mgr_.GetRootFCB();
        }

        void OpenFolder(FCB *folder_fcb, std::string folder_name) {
            std::unique_ptr<File> current_dir_file(hard_disk_mgr_.ReadFile(folder_fcb, true));
            Folder current_folder = (current_dir_file.get());
            auto iter = current_folder.fcb_map.find(folder_name);
            if (iter != current_folder.fcb_map.end()) {
                if (current_dir->parent_index_handle != -1)
                    delete current_dir;
                current_dir = new FCB(iter->second);
                cout << "Change to folder " << current_dir->name << " success." << endl;
            } else {
                cout << "No this folder!" << endl;
            }
        }
        void OpenFolderInCurrentFolder(std::string folder_name) {
            OpenFolder(current_dir, folder_name);
        }
        void CreateFile(FCB *folder_fcb, std::string name) {
            if (name.size() >= sizeof(FCB::name) - 1) {
                cout << "Name " << name << " is too long!" << endl;
                return;
            }
            auto file = hard_disk_mgr_.CreateFile(name.c_str());
            file->parent_index_handle = folder_fcb->index_handle;

            std::unique_ptr<File> current_dir_file(hard_disk_mgr_.ReadFile(folder_fcb));
            Folder current_folder_temp(current_dir_file.get());
            if (current_folder_temp.fcb_map.find(name) == current_folder_temp.fcb_map.end()) {
                current_folder_temp.InsertFile(*file);
                hard_disk_mgr_.WriteFile(folder_fcb,
                                         current_folder_temp.GetFileData().get(),
                                         current_folder_temp.GetFileSize());
                //Update the last open dir.
                if (folder_fcb->parent_index_handle != -1) {
                    FCB tempFCB;
                    tempFCB.index_handle = folder_fcb->parent_index_handle;
                    std::unique_ptr<File> last_file_ptr;
                    std::shared_ptr<Folder> last_folder;

                    last_file_ptr = std::unique_ptr<File>(hard_disk_mgr_.ReadFile(&tempFCB, true));
                    last_folder = std::make_shared<Folder>(last_file_ptr.get());
                    for (auto &i: last_folder->fcb_map) {
                        if (i.second.index_handle == folder_fcb->index_handle) {
                            i.second.size = folder_fcb->size;
                            break;
                        }
                    }
                    last_folder->fcb_map.erase("");
                    tempFCB.size = last_folder->GetFileSize();
                    hard_disk_mgr_.WriteFile(&tempFCB, last_folder->GetFileData().get(), last_folder->GetFileSize());
                }
            } else {
                cout << "Already have this file." << endl;
            }
        }

        void CreateFileInCurrentFolder(std::string name) {
            CreateFile(current_dir, name);
        }

        void CreateFolder(FCB *parent_fcb, std::string name) {
            if (name.size() >= sizeof(FCB::name) - 1) {
                cout << "Name " << name << " is too long!" << endl;
                return;
            }
            FCB *folder_fcb = hard_disk_mgr_.CreateFolder(name.c_str(), parent_fcb);
            folder_fcb->parent_index_handle = parent_fcb->index_handle;

            std::unique_ptr<File> current_dir_file(hard_disk_mgr_.ReadFile(parent_fcb));
            Folder current_folder(current_dir_file.get());

            if (current_folder.fcb_map.find(name) == current_folder.fcb_map.end()) {
                current_folder.InsertFile(*folder_fcb);
                hard_disk_mgr_.WriteFile(parent_fcb, current_folder.GetFileData().get(), current_folder.GetFileSize());
                if (parent_fcb->parent_index_handle != -1) {
                    FCB tempFCB;
                    tempFCB.index_handle = parent_fcb->index_handle;
                    std::unique_ptr<File> last_file_ptr;
                    std::shared_ptr<Folder> last_folder;

                    last_file_ptr = std::unique_ptr<File>(hard_disk_mgr_.ReadFile(&tempFCB, true));
                    last_folder = std::make_shared<Folder>(last_file_ptr.get());
                    for (auto &i: last_folder->fcb_map) {
                        if (i.second.index_handle == parent_fcb->index_handle) {
                            i.second.size = parent_fcb->size;
                            break;
                        }
                    }
                    hard_disk_mgr_.WriteFile(&tempFCB, last_folder->GetFileData().get(), last_folder->GetFileSize());
                }
            } else {
                cout << "Already have this file." << endl;
            }
        }
        void CreateFolderInCurrentFolder(std::string name) {
            CreateFolder(current_dir, name);
        }
        void ShowCurrentFolderFile() {
            std::unique_ptr<File> current_dir_file(hard_disk_mgr_.ReadFile(current_dir, true));
            Folder current_folder(current_dir_file.get());
            cout << endl;
            for (auto &i: current_folder.fcb_map) {
                cout << i.first << endl;
            }
        }

        void OpenFileInCurrentFolder(std::string name) {
            OpenFileInFolder(current_dir, name);
        }
        void OpenFileInFolder(FCB *folder_fcb, std::string name) {
            std::unique_ptr<File> current_dir_file(hard_disk_mgr_.ReadFile(folder_fcb, true));
            Folder current_folder = (current_dir_file.get());
            auto iter = current_folder.fcb_map.find(name);
            if (iter != current_folder.fcb_map.end()) {
                OpenFile(&iter->second);
            } else {
                cout << "No this file!" << endl;
            }
        }
        void OpenFile(FCB *file_fcb) {
            File *file = hard_disk_mgr_.ReadFile(file_fcb);
            cout << "Data:" << endl;
            std::unique_ptr<char[]> res(new char[file->GetFileSize()]);
            memcpy(res.get(), file->GetFileData().get(), file->GetFileSize());
            //data.push_back('\0');
            cout << res.get() << endl;
        }
        void WriteFileInCurrentFolder(std::string name, std::string data) {
            WriteFileInFolder(current_dir, name, data);
        }
        void WriteFileInFolder(FCB *folder_fcb, std::string name, std::string data) {
            std::unique_ptr<File> current_dir_file(hard_disk_mgr_.ReadFile(folder_fcb, true));
            Folder current_folder(current_dir_file.get());
            auto iter = current_folder.fcb_map.find(name);
            if (iter != current_folder.fcb_map.end()) {
                WriteFile(&iter->second, data);
            } else {
                cout << "No this file!" << endl;
            }
            hard_disk_mgr_.WriteFile(folder_fcb,
                                     current_folder.GetFileData().get(),
                                     current_folder.GetFileSize());
        }
        void WriteFile(FCB *fcb, std::string data) {
            auto str = data.data();
            hard_disk_mgr_.WriteFile(fcb, str, data.size());
        }
    };

}// namespace file_system


#endif//OS_FINAL_FILEEXPLORER_H
