#pragma once

#include <cstddef>
#include <fstream>
#include <iostream>
#include <optional>
#include <string_view>
#include <vector>



namespace utils {

struct File {

    /**
     * Reads the entire contents of a file into a vector of bytes.
     * @param filepath The path to the file to be read.
     * @return An optional vector of bytes containing the contents of the file.
     *         If the file cannot be opened or read, the optional is empty.
     * @throws None
     */
    static std::optional<std::vector<char>> read_all(const std::string &filepath);

    static bool is_image(const std::filesystem::path &filepath);
};
} // namespace utils