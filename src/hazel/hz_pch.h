#pragma once

#ifdef _WIN32
    #include <windows.h>
#endif

#ifdef __linux
    #include <unistd.h>
#endif

#include <any>
#include <chrono>
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
#include <ranges>
#include <stack>
#include <stdexcept>
#include <string>
#include <ranges>
#include <type_traits>

#include <fmt/format.h>

#include <hazel/core/log.h>
#include <hazel/debug/instrumentor.h>

#include "hazel/utils/path.h"
