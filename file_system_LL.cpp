#include <iostream>
#include <fstream>
#include <map>

#include <stdio.h>

using namespace std;

const int BLOCK_SIZE = 16; // Size of disk blocks in bytes

// Data structure for a disk block
struct DiskBlock {
    char data[BLOCK_SIZE];
    DiskBlock* nextBlock; // Index of the next block in the linked list

    DiskBlock() {
        nextBlock = NULL;
        // data[BLOCK_SIZE] = '\0';
    }
};


struct DiskBlockList {
    DiskBlock* head;
    int size;

    DiskBlockList() {
        head = NULL;
        size = 0;
    }

    void addBlocks(DiskBlock* diskBlock, int noOfBlocks) {
        if(head == NULL) { // First Node added
            head = diskBlock;
        }
        else { // Add block at beginning
            diskBlock->nextBlock = head;
            head = diskBlock;
        }
        size += noOfBlocks;
    }

    DiskBlock* getBlocks(int noOfBlocksRequired) {
        if(noOfBlocksRequired > size) {
            // printf("Not enough blocks for file!\n");
            return NULL;
        }

        DiskBlock* current = head;
        for(int i = 0; i < noOfBlocksRequired-1; i++) {
            current = current->nextBlock;
        }
        DiskBlock* allocatedBlocks = head;
        head = current->nextBlock; // new head 
        current->nextBlock = NULL; // breaking LL of size noOfBlocksRequired 
        size -= noOfBlocksRequired;
        return allocatedBlocks;
    }
};


// Data structure for a file
struct File {
    string name;
    int size;
    DiskBlockList fileBlocks; // Index of the first block in the linked list
    
    File(string nm) {
        name = nm;
        size = 0;
    }
};


class FileSystem_LL {
    public:

    int noOfBlocks;
    int noOfFreeBlocks;
    int noOfFiles;
    
    DiskBlockList freeList;

    map<string, File*> filesTable;

    FileSystem_LL(int nb) {
        noOfFiles = 0;
        noOfBlocks = nb;
        // creating free blocks list
        for(int i = 0; i < noOfBlocks; i++) {
            freeList.addBlocks(new DiskBlock(), 1);
        }
        noOfFreeBlocks = noOfBlocks;
        printf("Free Block List is created!\n");
    }

    int createFile(string name) {
        // checking if file already exists!
        if(filesTable.find(name) != filesTable.end()) {
            printf("File already exists!\n");
            return -1;
        }

        // creating file
        File * file = new File(name);
        // adding file to filesystem table
        filesTable[name] = file;
        noOfFiles++;
        return 1;
    }

    int writeFile(string name, string data) {
        if(filesTable.find(name) == filesTable.end()) {
            printf("Cannot write, File does not exists!\n");
            return -1;
        }

        int noOfBlocksRequired = data.size()/BLOCK_SIZE; 
        data.size()%BLOCK_SIZE == 0 ? 0 : noOfBlocksRequired++;

        File* file = filesTable[name];

        // free some blocks if file size is reduced
        if(file->fileBlocks.size > noOfBlocksRequired) {
            int noOfBlocksExtra = file->fileBlocks.size - noOfBlocksRequired;
            DiskBlock* freed = file->fileBlocks.getBlocks(noOfBlocksExtra);
            freeList.addBlocks(freed, noOfBlocksExtra);
            noOfFreeBlocks += noOfBlocksExtra;
        }
        // getting free blocks for file as size is increased
        else if(file->fileBlocks.size < noOfBlocksRequired) {
            int noOfBlocksNeeded = noOfBlocksRequired - file->fileBlocks.size;
            DiskBlock* blockHead = freeList.getBlocks(noOfBlocksNeeded);
            if(blockHead == NULL) {
                printf("Not enough space(blocks) for file!\n");
            }
            file->fileBlocks.addBlocks(blockHead, noOfBlocksRequired);
            noOfFreeBlocks -= noOfBlocksNeeded;
        }

        // write data to file.
        /*code*/
        DiskBlock* curr = file->fileBlocks.head;
        int blockNo = 0;
        while(curr != NULL) {
            
            for(int i = 0; i < BLOCK_SIZE && blockNo*BLOCK_SIZE + i != data.size(); i++) {
                // if(blockNo*BLOCK_SIZE + i == data.size()) {
                //     curr->data[i] = '\0';
                //     break;
                // }
                curr->data[i] = data[blockNo*BLOCK_SIZE + i];
                // cout << curr->data[i] << endl;
            }
            blockNo++;
            curr = curr->nextBlock;
        }
        file->size = data.size();
        return 1;
    }

    int deleteFile(string name) {
        if(filesTable.find(name) == filesTable.end()) {
            printf("Cannot Delete, File does not exists!\n");
            return -1;
        }

        
        File* file = filesTable[name];

        DiskBlock* freed = file->fileBlocks.getBlocks(file->fileBlocks.size);
        freeList.addBlocks(freed, file->fileBlocks.size);
        filesTable.erase(name);
        noOfFiles--;

        delete file;
        return 1;
    }

    string readFile(string name) {
        if(filesTable.find(name) == filesTable.end()) {
            printf("Cannot read, File does not exists!\n");
            return "";
        }

        File* file = filesTable[name];
        DiskBlock* curr = file->fileBlocks.head;
        string data = "";
        // while(curr != NULL) {
        //     string s(curr->data);
        //     data += s;
        //     curr = curr->nextBlock;
        // }
        
        while(curr != NULL) {
            for(int i = 0; i < BLOCK_SIZE && data.size() < file->size; i++) {
                
                data.push_back(curr->data[i]);
                // cout << curr->data[i] << endl;
            }
            curr = curr->nextBlock;
        }
        return data;
    } 

    void displayFileSystemInfo() {
        // if(filesTable.find(name) == filesTable.end()) {
        //     printf("File does not exists!\n");
        //     return;
        // }
        // File* file = filesTable[name];
        cout << "============================    File System  ============================" << endl << endl;
        // cout << "FileName     : " << file->name << endl;
        cout << "Total No of Files       : " << noOfFiles << endl;
        cout << "Total No of Blocks      : " << noOfBlocks << endl;
        cout << "Total No of Free Blocks : " << noOfFreeBlocks << endl << endl;

        printf(" -------------------------------------------------------------------\n");
        printf("|         Name         | Size(B) | NoOfBlocks |      contents       |\n");
        printf("|-------------------------------------------------------------------|\n");
        for(auto e : filesTable) {
            printf("| %20.20s | %7d | %10d | %16.16s... |\n", e.first.c_str(), e.second->size, e.second->fileBlocks.size, readFile(e.first).c_str());
            // cout << e.first << " " << e.second->size << " " << e.second->fileBlocks.size << endl;
        }
        printf(" -------------------------------------------------------------------\n\n");
        cout << "=========================================================================" << endl << endl;


    
    }

};


int main(int argc, char const *argv[])
{
    /* code */
    FileSystem_LL fs(10);
    
    string name = "abc.txt";
    string data = "This is my newly created Text file";
    fs.createFile(name);
    fs.writeFile(name, data);
    fs.createFile("Second.cpp");
    fs.writeFile("Second.cpp", "#include<stdio.h>\n int main() {\n\tprintf(\"Hello World!\");\n\treturn 0;\n}");
    // string d = fs.readFile("Second.cpp");
    // cout << "Data in file " << "Second.cpp" << " is : \n" << d << endl;
    fs.displayFileSystemInfo();    
    fs.deleteFile(name);
    fs.displayFileSystemInfo();
    return 0;
}
