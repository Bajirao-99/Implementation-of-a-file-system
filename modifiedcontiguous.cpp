#include <iostream>
#include <unordered_map>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

const int BLOCK_SIZE = 4096; // block size in bytes
const int NUM_BLOCKS = 512;  // total number of blocks on the disk

struct File
{
    string name;
    int size;
    string initial_block;    // the first block of the contiguous area allocated to the file
    vector<string> overflow_blocks; // the additional blocks allocated to the file
};

class FileSystem
{
private:
    vector<bool> blocks;    // tracks which blocks are free or allocated
    unordered_map<string, File> directory; // directory of files on the disk

public:
    FileSystem()
    {
        // initialize all blocks as free
        blocks.resize(NUM_BLOCKS, false);
    }

    bool createOrModifyFile(const string& name, int size)
    {
        auto start = high_resolution_clock::now(); // start time stamp

        if (size <= 0) {
            cout << "\n\nInvalid file size!\n";
            return false;
        }

        if (size > NUM_BLOCKS * BLOCK_SIZE) {
            cout << "\n\nFile size too large!\n";
            return false;
        }

        if (directory.find(name) != directory.end()) {
            deleteFile(name);
        }

        string initial_block;
        int num_blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;

        for (int i = 0; i < NUM_BLOCKS && initial_block.empty(); i++) {
            if (!blocks[i]) {
                bool contiguous_space_available = true;
                for (int j = i; j < i + num_blocks_needed; j++) {
                    if (j >= NUM_BLOCKS || blocks[j]) {
                        contiguous_space_available = false;
                        break;
                    }
                }
                if (contiguous_space_available) {
                    initial_block = to_string(i);
                    for (int j = i; j < i + num_blocks_needed; j++) {
                        blocks[j] = true;
                    }
                }
            }
        }

        if (initial_block.empty()) {
            cout << "\n\nNot enough space on disk!\n";
            return false;
        }

        vector<string> overflow_blocks;
        int num_overflow_blocks_needed = num_blocks_needed - 1;

        for (int i = 0; i < NUM_BLOCKS && overflow_blocks.size() < num_overflow_blocks_needed; i++) {
            if (!blocks[i]) {
                blocks[i] = true;
                overflow_blocks.push_back(to_string(i));
            }
        }

        directory[name] = {name, size, initial_block, overflow_blocks};

        auto stop = high_resolution_clock::now();                 // stop time stamp
        auto duration = duration_cast<nanoseconds>(stop - start); // calculate duration in nanoseconds
        cout << "\n\nCreated or modified " << name << " in " << duration.count() << " nanoseconds\n";
        return true;
    }

    bool deleteFile(const string& name)
    {
        auto start = high_resolution_clock::now(); // start time stamp

        if (directory.find(name) == directory.end()) {
            cout << "\n\nFile not found!\n";
            return false;
        }

        const auto& file = directory[name];
        blocks[stoi(file.initial_block)] = false;
        for (const auto& block : file.overflow_blocks) {
            blocks[stoi(block)] = false;
        }

        directory.erase(name);

        auto stop = high_resolution_clock::now();                 // stop time stamp
        auto duration = duration_cast<nanoseconds>(stop - start); //
        cout << "\n\nDeleted " << name << " in " << duration.count() << " nanoseconds\n";
        return true;
    }

    bool readFile(const string& name)
    {
        auto start = high_resolution_clock::now(); // start time stamp

        if (directory.find(name) == directory.end()) {
            cout << "\n\nFile not found!\n";
            return false;
        }

        const auto& file = directory[name];

        cout << "\n\nBlocks of file '" << name << "': " << file.initial_block << ",";
        for (const auto& block : file.overflow_blocks) {
            cout << block << ",";
        }
        cout << "\n";

        auto stop = high_resolution_clock::now();                 // stop time stamp
        auto duration = duration_cast<nanoseconds>(stop - start); // calculate duration in nanoseconds
        cout << "Read " << name << " in " << duration.count() << " nanoseconds\n";
        return true;
    }

    void displayDiskUsage()
    {
        int num_free_blocks = 0;
        for (const auto& block : blocks) {
            if (!block) {
                num_free_blocks++;
            }
        }
        int num_allocated_blocks = NUM_BLOCKS - num_free_blocks;
        int disk_usage_percent = (num_allocated_blocks * 100) / NUM_BLOCKS;

        cout << "\n\nDisk Usage: " << num_allocated_blocks << " / " << NUM_BLOCKS << " blocks allocated (" << disk_usage_percent << "%)\n";
    }

    void displayDirectory() const
    {
        cout << "\n\nDirectory:\n";
        for (const auto& entry : directory) {
            const string& name = entry.first;
            const File& file = entry.second;
            cout << "Name: " << name << ", Size: " << file.size << " bytes, Blocks: " << file.initial_block;
            for (const auto& block : file.overflow_blocks) {
                cout << " -> " << block;
            }
            cout << endl;
        }
    }
};

int main()
{
    FileSystem fs;

    fs.createOrModifyFile("file1", 3000);
    fs.createOrModifyFile("file2", 10000);
    fs.createOrModifyFile("file3", 20000);

    fs.displayDiskUsage();
    fs.displayDirectory();

    fs.readFile("file1");
    fs.readFile("file2");
    fs.readFile("file3");

    fs.deleteFile("file1");

    fs.displayDiskUsage();
    fs.displayDirectory();

    return 0;
}