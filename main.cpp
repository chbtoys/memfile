// Compile: clear && clang++ -std=c++20 main.cpp -o memfile
#include "memfile.hpp"
#include <iostream>
#include <cstring>

int main() {
    using namespace memfile;

    // Adding files
    MemFile::selectFile("example.bin", MemFile::Mode::Write);
    MemFile::selectFile("data.bin", MemFile::Mode::Append);

    // Writing to a file
    MemFile& file1 = MemFile::getFile("example.bin");
    const char* binaryData = "\x01\x02\x03\x04\x05";
    file1.write(binaryData, 5);
    file1.save();

    // Appending to the same file
    MemFile::selectFile("example.bin", MemFile::Mode::Append);
    MemFile& file2 = MemFile::getFile("example.bin");
    const char* moreData = "\x06\x07\x08\x09\x0A";
    file2.write(moreData, 5);
    file2.save();

    // Reading from a file
    MemFile::selectFile("example.bin", MemFile::Mode::Read);
    MemFile& file3 = MemFile::getFile("example.bin");
    char buffer[20];
    size_t bytesRead = file3.read(buffer, sizeof(buffer));
    for (size_t i = 0; i < bytesRead; ++i) {
        std::cout << std::hex << static_cast<int>(buffer[i]) << " ";
    }
    std::cout << std::endl;

    // Listing files with sizes
    MemFile::listFiles();

    // Copy example.bin to data.bin
    MemFile::selectFile("example.bin", MemFile::Mode::Read);
    MemFile& file4 = MemFile::getFile("example.bin");
    size_t fileSize = file4.getFileSize();
    MemFile::selectFile("data.bin", MemFile::Mode::Write);
    MemFile& file5 = MemFile::getFile("data.bin");
    file5.write(buffer, fileSize);

    // Deleting a file
    MemFile::removeFile("example.bin");

    // Listing files again to confirm deletion
    std::cout << std::endl;
    MemFile::listFiles();

    // Directory handling examples
    std::string dirPath = "example_dir";

    // Create a directory
    if (MemFile::createDirectory(dirPath)) {
        std::cout << "Directory created successfully: " << dirPath << std::endl;
    }

    // List directory contents
    std::cout << "Contents of the directory (" << dirPath << "):" << std::endl;
    MemFile::listDirectory(dirPath);

    // Create a file in the new directory
    std::string filePathInDir = dirPath + "/new_file.bin";
    MemFile::selectFile(filePathInDir, MemFile::Mode::Write);
    MemFile& fileInDir = MemFile::getFile(filePathInDir);
    const char* dirFileData = "\x0B\x0C\x0D\x0E\x0F";
    fileInDir.write(dirFileData, 5);
    fileInDir.save();

    // List directory contents again to show the new file
    std::cout << "Contents of the directory (" << dirPath << ") after adding new_file.bin:" << std::endl;
    MemFile::listDirectory(dirPath);

    // Remove the new file
    MemFile::removeFile(filePathInDir);
    std::cout << "Removed file: " << filePathInDir << std::endl;

    // List directory contents again to confirm removal
    std::cout << "Contents of the directory (" << dirPath << ") after removing new_file.bin:" << std::endl;
    MemFile::listDirectory(dirPath);

    // Remove the directory
    MemFile::removeDirectory(dirPath);
    std::cout << "Removed directory: " << dirPath << std::endl;

    return 0;
}
