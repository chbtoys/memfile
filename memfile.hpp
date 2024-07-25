#ifndef MEMFILE_H
#define MEMFILE_H

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <iostream>
#include <cstdio> // for std::remove
#include <filesystem> // for directory operations

namespace memfile {

class MemFile {
public:
    enum class Mode { Read, Write, Append };

    // Default constructor
    MemFile() = default;

    MemFile(const std::string& path, Mode mode) : path_(path), mode_(mode) {
        if (mode == Mode::Read || mode == Mode::Append) {
            std::ifstream file(path, std::ios::binary);
            if (file.is_open()) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                content_ = buffer.str();
                position_ = (mode == Mode::Append) ? content_.size() : 0;
            }
        } else if (mode == Mode::Write) {
            content_.clear();
            position_ = 0;
        }
    }

    size_t read(char* buffer, size_t size) {
        if (mode_ != Mode::Read && mode_ != Mode::Append) {
            return 0;
        }
        size_t readSize = std::min(size, content_.size() - position_);
        std::memcpy(buffer, content_.data() + position_, readSize);
        position_ += readSize;
        return readSize;
    }

    size_t write(const char* buffer, size_t size) {
        if (mode_ != Mode::Write && mode_ != Mode::Append) {
            return 0;
        }
        if (position_ + size > content_.size()) {
            content_.resize(position_ + size);
        }
        std::memcpy(content_.data() + position_, buffer, size);
        position_ += size;
        return size;
    }

    void seek(size_t pos) {
        position_ = pos;
    }

    size_t tell() const {
        return position_;
    }

    void save() {
        std::ofstream file(path_, std::ios::binary);
        if (file.is_open()) {
            file.write(content_.data(), content_.size());
        }
    }

    std::string getPath() const {
        return path_;
    }

    size_t getFileSize() const {
        return content_.size();
    }

    static void listFiles() {
        for (const auto& [path, file] : files_) {
            std::cout << path << " (" << std::dec << file.getFileSize() << " bytes)" << std::endl;
        }
    }

    static void selectFile(const std::string& path, Mode mode) {
        files_[path] = MemFile(path, mode);
    }

    static MemFile& getFile(const std::string& path) {
        return files_.at(path);
    }

    static void removeFile(const std::string& path) {
        auto it = files_.find(path);
        if (it != files_.end()) {
            files_.erase(it);
            std::remove(path.c_str()); // Delete the file from the filesystem
        }
    }

    // Directory handling methods
    static bool createDirectory(const std::string& path) {
        return std::filesystem::create_directories(path);
    }

    static void listDirectory(const std::string& path) {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            std::cout << entry.path().string() << std::endl;
        }
    }

    static void removeDirectory(const std::string& path) {
        std::filesystem::remove_all(path);
    }

private:
    std::string path_;
    std::string content_;
    size_t position_ = 0;
    Mode mode_;
    static std::map<std::string, MemFile> files_;
};

std::map<std::string, MemFile> MemFile::files_;

} // namespace memfile

#endif // MEMFILE_H
