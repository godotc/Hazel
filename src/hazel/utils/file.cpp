#include "file.h"
#include <filesystem>

namespace utils {

std::optional<std::vector<char>> File::read_all(const std::string &filepath)
{
    static constexpr size_t FILE_MAX_SIZE = 1024 * 1024 * 128; // 128 MB

    // Open the file
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return {};
    }

    // Get the size of the file
    std::streamsize file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    if (file_size > FILE_MAX_SIZE) {
        std::cerr << "File is too large: " << filepath << std::endl;
        return {};
    }


    // Allocate memory to store the file content
    std::vector<char> buffer(file_size);
    // Read the file content into the allocated memory
    if (!file.read(buffer.data(), file_size)) {
        std::cerr << "Failed to read file: " << filepath << std::endl;
        return {};
    }

    // Close the file
    file.close();


    return std::move(buffer);
}

bool File::is_image(const std::filesystem::path &filepath)
{
    auto ext = filepath.filename().extension();
    return ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp";
}


} // namespace utils