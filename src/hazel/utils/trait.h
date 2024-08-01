#pragma once



namespace utils::trait {


struct disable_copy {
    disable_copy(const disable_copy &)            = delete;
    disable_copy &operator=(const disable_copy &) = delete;
};



}; // namespace utils::trait