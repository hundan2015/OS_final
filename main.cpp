#include "src/controller/FileExplorer.h"
#include "src/controller/HardDiskManager.h"
#include <iostream>
using namespace file_system;
int main() {
    HardDiskManager hardDiskManager;
    FileExplorer fileExplorer(hardDiskManager);
    fileExplorer.ShowCurrentFolderFile();
    fileExplorer.CreateFolderInCurrentFolder("shabi");
    fileExplorer.CreateFileInCurrentFolder("Ubuntu");
    fileExplorer.ShowCurrentFolderFile();

    fileExplorer.OpenFolderInCurrentFolder("shabi");
    fileExplorer.CreateFileInCurrentFolder("Suse");
    fileExplorer.ShowCurrentFolderFile();

    fileExplorer.OpenFolderInCurrentFolder("..");
    fileExplorer.ShowCurrentFolderFile();
    fileExplorer.OpenFolderInCurrentFolder("shabi");
    fileExplorer.ShowCurrentFolderFile();
    fileExplorer.WriteFileInCurrentFolder("Suse", "niahflasflasdjflkasf");
    fileExplorer.OpenFileInCurrentFolder("Suse");
    return 0;
}
