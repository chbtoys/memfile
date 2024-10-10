#ifndef MEMFILE_H
#define MEMFILE_H

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <iostream>
#include <filesystem>
#include <cstdio>
#include <cstdlib>

namespace memfile {

class MemFile {
public:
    enum class Mode { Read, Write, Append };

    // Default constructor
    MemFile() = default;

    MemFile(const std::string& path, Mode mode) : path_(resolveEnvVars(path)), mode_(mode) {
        if (mode == Mode::Read || mode == Mode::Append) {
            std::ifstream file(path_, std::ios::binary);
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

    void save(std::string newPath) {
        std::filesystem::path memFilePath(path_);
        std::string filename = memFilePath.filename().string();
        std::string fullPath;
        if (newPath == ".") {
            fullPath = newPath + "/" + filename;  // If newPath is ".", add a slash before filename
        } else if (newPath.back() != '/') {
            fullPath = newPath + "/" + filename;  // Add a slash if newPath doesn't end with one
        } else {
            fullPath = newPath + filename;        // Just concatenate if slash is already present
        }

        std::ofstream file(fullPath, std::ios::binary);
        if (file.is_open()) {
            file.write(content_.data(), content_.size());
        }
    }

    void load(const std::string& fullPath, const std::string& newPath) {
        std::ifstream file(fullPath, std::ios::binary);
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();  // Read the file content into the buffer
            content_ = buffer.str(); // Store the buffer content into the 'content_' string
            position_ = 0;           // Reset the position to the start of the file
            path_=newPath;
        } else {
            throw std::runtime_error("Could not open file: " + path_);
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
        files_[resolveEnvVars(path)] = MemFile(path, mode);
    }

    static MemFile& getFile(const std::string& path) {
        return files_.at(resolveEnvVars(path));
    }

    static void removeFile(const std::string& path) {
        auto it = files_.find(resolveEnvVars(path));
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

    // Environment variable handling
    static void setEnv(const std::string& var, const std::string& value) {
        env_[var] = value;
    }

    static std::string getEnv(const std::string& var) {
        if (env_.find(var) != env_.end()) {
            return env_.at(var);
        }
        // Fallback to system environment variable
        const char* sysEnv = std::getenv(var.c_str());
        return sysEnv ? std::string(sysEnv) : "";
    }

    // Resolves environment variables in file paths (e.g., ${HOME}/file.txt)
    static std::string resolveEnvVars(const std::string& path) {
        std::string resolvedPath = path;
        size_t start = resolvedPath.find("${");
        while (start != std::string::npos) {
            size_t end = resolvedPath.find("}", start);
            if (end != std::string::npos) {
                std::string varName = resolvedPath.substr(start + 2, end - start - 2);
                std::string varValue = getEnv(varName);
                resolvedPath.replace(start, end - start + 1, varValue);
                start = resolvedPath.find("${", start + varValue.length());
            } else {
                break;
            }
        }
        return resolvedPath;
    }

private:
    std::string path_;
    std::string content_;
    size_t position_ = 0;
    Mode mode_;
    static std::map<std::string, MemFile> files_;
    static std::map<std::string, std::string> env_;  // Custom environment variables
};

// Initialize static members
std::map<std::string, MemFile> MemFile::files_;
std::map<std::string, std::string> MemFile::env_;

} // namespace memfile

#endif // MEMFILE_H
