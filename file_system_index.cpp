#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <stdio.h>

using namespace std;

const int BLOCK_SIZE = 16; // Size of disk blocks in bytes

// Data structure for a disk block
struct DiskBlock {
    int index;
    char data[BLOCK_SIZE]; 
    
    DiskBlock(int i) {
        index = i;
    }
};


// Data structure for a file
struct File {
    string name;
    int size;
    int firstBlockIndex;
    int noOfDiskBlocks;
    
    File(string nm) {
        name = nm;
        size = 0;
        noOfDiskBlocks = 1;
    }
};


class FileSystem_LL {
    public:

    int noOfBlocks;
    int noOfFreeBlocks;
    int noOfFiles;
    
    vector<DiskBlock*> DISK;
    vector<DiskBlock*> freeBlocks;
    map<string, File*> filesTable;

    FileSystem_LL(int nb) {
        noOfFiles = 0;
        noOfBlocks = nb;
        // creating free blocks list
        for(int i = 0; i < noOfBlocks; i++) {
            DiskBlock* db = new DiskBlock(i);
            DISK.push_back(db);
            freeBlocks.push_back(db);
        }
        noOfFreeBlocks = noOfBlocks;
        printf("Free Blocks are created!\n");
    }

    int createFile(string name) {
        // checking if file already exists!
        if(filesTable.find(name) != filesTable.end()) {
            printf("File %s already exists!\n", name.c_str());
            return -1;
        }

        if(noOfFreeBlocks == 0) {
            printf("No blocks available to create file %s!\n", name.c_str());
            return -1;
        }
        // creating file
        File * file = new File(name);
        DiskBlock* db = freeBlocks.back();
        
        freeBlocks.pop_back();
        file->firstBlockIndex = db->index; 
        // cout << "index for file : " << name << " = " << file->firstBlockIndex << endl;
        noOfFreeBlocks--;
        // adding file to filesystem table
        filesTable[name] = file;
        noOfFiles++;
        return 1;
    }

    int writeFile(string name, string data) {
        if(filesTable.find(name) == filesTable.end()) {
            printf("Cannot write to %s, File does not exists!\n", name.c_str());
            return -1;
        }

        int noOfBlocksRequired = data.size()/BLOCK_SIZE; 
        data.size()%BLOCK_SIZE == 0 ? 0 : noOfBlocksRequired++; // taking the ceiling

        noOfBlocksRequired++; // ectra block for index block

        File* file = filesTable[name];
        // cout << "writing for file : " << name << " = " << file->firstBlockIndex << endl;
        DiskBlock* indexDB = DISK[file->firstBlockIndex];
        // free some blocks if file size is reduced
        if(file->noOfDiskBlocks > noOfBlocksRequired) {
            int noOfBlocksExtra = file->noOfDiskBlocks - noOfBlocksRequired;
            // freeing the last extra blocks id node
            for(int i = 0; i < noOfBlocksExtra; i++) {
                freeBlocks.push_back(DISK[indexDB->data[file->noOfDiskBlocks - i - 2]]);
            }
            file->noOfDiskBlocks -= noOfBlocksExtra;
            noOfFreeBlocks += noOfBlocksExtra;
        }
        // getting free blocks for file as size is increased
        else if(file->noOfDiskBlocks < noOfBlocksRequired) {
            int noOfBlocksNeeded = noOfBlocksRequired - file->noOfDiskBlocks;
            if(noOfBlocksNeeded > noOfFreeBlocks) {
                // printf("Not enough space(blocks) for file!\n");
            }
            for(int i = 0; i < noOfBlocksNeeded; i++) {
                DiskBlock* db = freeBlocks.back();
                freeBlocks.pop_back();
                // cout << file->noOfDiskBlocks + i - 1 << ". block alloted = " << db->index << endl;
                indexDB->data[file->noOfDiskBlocks + i - 1] = db->index;
            }
            noOfFreeBlocks -= noOfBlocksNeeded;
            file->noOfDiskBlocks += noOfBlocksNeeded;
        }

        // write data to file.
        /*code*/
        // cout << "data : " << data << endl;
        // cout << "No of blocks after adding " << file->noOfDiskBlocks << endl;
        for(int i = 0; i < file->noOfDiskBlocks-1; i++) {
            int bl_ind = indexDB->data[i];
            // cout << "writing on block " << bl_ind << endl;
            for(int j = 0; j < BLOCK_SIZE && i*BLOCK_SIZE + j < data.size(); j++) {
                DISK[bl_ind]->data[j] = data[i*BLOCK_SIZE+j];
                // cout <<  ".";
            }
            // cout << endl;
        }
        file->size = data.size();
        return 1;
    }

    int deleteFile(string name) {
        if(filesTable.find(name) == filesTable.end()) {
            printf("Cannot Delete %s, File does not exists!\n", name.c_str());
            return -1;
        }

        
        File* file = filesTable[name];
        DiskBlock* indexDB = DISK[file->firstBlockIndex];
        // freeing the last extra blocks id node
        for(int i = 0; i < file->noOfDiskBlocks-1; i++) {
            freeBlocks.push_back(DISK[indexDB->data[file->noOfDiskBlocks - i - 1]]);
        }
        
        filesTable.erase(name);
        noOfFiles--;
        noOfFreeBlocks += file->noOfDiskBlocks;
        file->noOfDiskBlocks = 1;
        delete file;
        freeBlocks.push_back(DISK[file->firstBlockIndex]);
        return 1;
    }

    string readFile(string name) {
        if(filesTable.find(name) == filesTable.end()) {
            printf("Cannot read, File does not exists!\n");
            return "";
        }

        File* file = filesTable[name];
        DiskBlock* indexDB = DISK[file->firstBlockIndex];
        // if(indexDB != NULL) cout << "block id : " << indexDB->index << endl;
        string data = "";
        // while(curr != NULL) {
        //     string s(curr->data);
        //     data += s;
        //     curr = curr->nextBlock;
        // }
        // cout << "Hello agaib" << endl;
        // cout << "no of disk blocks : " << file->noOfDiskBlocks << endl;
        for(int j = 0; j < file->noOfDiskBlocks-1; j++) {
            // cout << "Block accessed! " << (int)indexDB->data[j] << "." <<  endl;
            DiskBlock* db = DISK[indexDB->data[j]]; 
            // cout << "Thisi s> " << db->data << endl;
            string s(db->data);
            data.append(s);
            // for(int i = 0; i < BLOCK_SIZE && data.size() < file->size; i++) {
                
            //     data.push_back(db->data[j*BLOCK_SIZE + i]);
            //     // cout << curr->data[i] << endl;
            // }
        }
        return data;
    } 

    void displayFileSystemInfo() {
        // if(filesTable.find(name) == filesTable.end()) {
        //     printf("File does not exists!\n");
        //     return;
        // }
        // File* file = filesTable[name];
        cout << "============================    File System  ===========================================================" << endl << endl;
        // cout << "FileName     : " << file->name << endl;
        cout << "Total No of Files       : " << noOfFiles << endl;
        cout << "Total No of Blocks      : " << noOfBlocks << endl;
        cout << "Total No of Free Blocks : " << noOfFreeBlocks << endl << endl;

        printf(" --------------------------------------------------------------------------------------------------\n");
        printf("|         Name         | Size(B) | NoOfBlocks | Index Block | Alloted Blocks |      contents       |\n");
        printf("|--------------------------------------------------------------------------------------------------|\n");
        for(auto e : filesTable) {
            int a[3] = {-1, -1, -1};
            for(int i = 0; i < e.second->noOfDiskBlocks-1 && i < 3; i++) {
                a[i] = DISK[e.second->firstBlockIndex]->data[i];
            }
            printf("| %20.20s | %7d | %10d | %11d | %2d, %2d, %2d,... | %16.16s... |\n", e.first.c_str(), e.second->size, e.second->noOfDiskBlocks, e.second->firstBlockIndex, a[0], a[1], a[2], readFile(e.first).c_str());
            // cout << e.first << " " << e.second->size << " " << e.second->fileBlocks.size << endl;
        }
        printf(" --------------------------------------------------------------------------------------------------\n\n");
        cout << "========================================================================================================" << endl << endl;


    
    }

};


int main(int argc, char const *argv[])
{
    /* code */
    FileSystem_LL fs(10);
    
    string name = "abc.txt";
    string data = "This is my newly created Text file";
    fs.createFile(name);
    fs.createFile(name);
    fs.writeFile(name, data);
    cout << fs.readFile(name) << endl;
    fs.createFile("Second.cpp");
    fs.writeFile("Second.cpp", "#include<stdio.h>\n int main() {\n\tprintf(\"Hello World!\");\n\treturn 0;\n}");
    fs.createFile("Third.cpp");
    
    string d = fs.readFile("Second.cpp");
    cout << "Data in file " << "Second.cpp" << " is : \n" << d << endl;
    // cout << "hello" << endl;
    fs.displayFileSystemInfo();    
    fs.deleteFile(name);
    fs.deleteFile(name);

    fs.displayFileSystemInfo();
    return 0;
}
