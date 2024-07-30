//
// Created by nono on 10/18/23.
//

#ifndef HAZEL_CONFIG_H
#define HAZEL_CONFIG_H

#define ENABLE_PROFILING 0

#define HAZEL_ENABLE_VIEWPORT 1

#include <string>

class DefaultConfig
{
  public:
    void                  Init() {}
    constexpr std::string ConfigName() { return "DefaultConfig"; }
};

#endif // HAZEL_CONFIG_H
