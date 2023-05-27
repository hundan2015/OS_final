//
// Created by symbolic on 23-5-27.
//
#include "../src/controller/HardDiskManager.h"
using namespace file_system;
using namespace std;
int main() {
    auto hard_disk_manager = file_system::HardDiskManager();
    FCB *current_dir_fcb;

    FCB *root_fcb = hard_disk_manager.GetRootFCB();
    FCB *folder_test_fcb = hard_disk_manager.CreateFolder("test", root_fcb);
    /**
     * | 0 - Super block
     * | 1 - root index block
     * | 2 - root file block
     * | 3 - folder index block
     * | 4 - folder file block
     */
    File *folder_file = hard_disk_manager.ReadFile(folder_test_fcb);
    Folder folder = Folder(folder_file);
    for (auto i: folder.fcb_map) {
        cout << i.first << endl;
    }

    File *root_folder_file = hard_disk_manager.ReadFile(root_fcb);
    Folder root_folder = Folder(root_folder_file);
    root_folder.InsertFile(*folder_test_fcb);
    hard_disk_manager.WriteFile(root_fcb, root_folder.GetFileData().get(), root_folder.GetFileSize());

    File *root_folder_file_ano = hard_disk_manager.ReadFile(root_fcb);
    Folder root_folder_ano = Folder(root_folder_file_ano);
    cout << endl;
    for (auto &i: root_folder_ano.fcb_map) {
        cout << i.first << endl;
    }

    current_dir_fcb = root_fcb;
    auto current_dir_file = hard_disk_manager.ReadFile(current_dir_fcb);
    auto current_folder = new Folder(current_dir_file);
    cout << endl;
    for (auto &i: current_folder->fcb_map) {
        cout << i.first << endl;
    }
}