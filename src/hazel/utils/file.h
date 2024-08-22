/**
 * @ Author: godot42
 * @ Create Time: 2024-08-02 10:47:59
 * @ Modified by: @godot42
 * @ Modified time: 2024-08-22 14:09:12
 * @ Description:
 */


#pragma once



#include <cstddef>
#include <filesystem>


namespace utils {

struct File {

    /**
     * Reads the entire contents of a file into a vector of bytes.
     * @param filepath The path to the file to be read.
     * @return An optional vector of bytes containing the contents of the file.
     *         If the file cannot be opened or read, the optional is empty.
     * @throws None
     */
    static std::optional<std::string> read_all(const std::filesystem::path &filepath);

    static bool is_image(const std::filesystem::path &filepath);


    static std::optional<size_t> get_content_hash(const std::filesystem::path &filepath);
    static std::optional<size_t> get_hash(const std::string &text);
};
} // namespace utils