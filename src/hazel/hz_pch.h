#pragma once

#ifdef _WIN32
    #include <windows.h>
#endif

#ifdef __linux
    #include <unistd.h>
#endif

#include <any>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <ostream>
#include <queue>
#include <stack>
#include <stdexcept>
#include <string>
#include <type_traits>

#include <fmt/format.h>
