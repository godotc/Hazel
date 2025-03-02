//
// Created by nono on 10/17/23.
//

#include "cstdio"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <thread>
#include <vector>


struct ProfileResult {
    std::string Name;
    long long   Start, End;
    uint32_t    ThreadID;
};

struct InstrumentationSession {
    std::string Name;
};

class Instrumentor
{
  private:
    InstrumentationSession *m_CurrentSession;
    std::ofstream           m_OutputStream;
    int                     m_ProfileCount;

  public:
    Instrumentor()
        : m_CurrentSession(nullptr), m_ProfileCount(0)
    {
    }

    void BeginSession(const std::string &name, const std::string &filepath = "results.json")
    {
        m_OutputStream.open(filepath);
        WriteHeader();
        m_CurrentSession = new InstrumentationSession{name};
    }

    void EndSession()
    {
        WriteFooter();
        m_OutputStream.close();
        delete m_CurrentSession;
        m_CurrentSession = nullptr;
        m_ProfileCount   = 0;
    }

    void WriteProfile(const ProfileResult &result)
    {
        if (m_ProfileCount++ > 0)
            m_OutputStream << ",";

        std::string name = result.Name;
        std::replace(name.begin(), name.end(), '"', '\'');

        m_OutputStream << "{";
        m_OutputStream << "\"cat\":\"function\",";
        m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
        m_OutputStream << "\"name\":\"" << name << "\",";
        m_OutputStream << "\"ph\":\"X\",";
        m_OutputStream << "\"pid\":0,";
        m_OutputStream << "\"tid\":" << result.ThreadID << ",";
        m_OutputStream << "\"ts\":" << result.Start;
        m_OutputStream << "}";

        m_OutputStream.flush();
    }

    void WriteHeader()
    {
        m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
        m_OutputStream.flush();
    }

    void WriteFooter()
    {
        m_OutputStream << "]}";
        m_OutputStream.flush();
    }

    static Instrumentor &Get()
    {
        static Instrumentor instance;
        return instance;
    }
};

class InstrumentationTimer
{
  public:
    InstrumentationTimer(const char *name)
        : m_Name(name), m_Stopped(false)
    {
        m_StartTimepoint = std::chrono::high_resolution_clock::now();
    }

    ~InstrumentationTimer()
    {
        if (!m_Stopped)
            Stop();
    }

    void Stop()
    {
        auto endTimepoint = std::chrono::high_resolution_clock::now();

        long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
        long long end   = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

        uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
        Instrumentor::Get().WriteProfile({m_Name, start, end, threadID});

        m_Stopped = true;
    }

  private:
    const char                                                 *m_Name;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
    bool                                                        m_Stopped;
};

#define ENABLE_PROFILING 1


#if _WIN32
    #define THE_PRETTY_FUNCTION __FUNCSIG__
#elif __linux__
    #define THE_PRETTY_FUNCTION __PRETTY_FUNCTION__
#else
    #error Need your implementation
#endif

#if ENABLE_PROFILING
    #define PROFILE_SCOPE(name) InstrumentationTimer timer##__LINE__(name)
    #define PROFILE_FUNCTION() PROFILE_SCOPE(THE_PRETTY_FUNCTION)
#else
    #define PROFILE_SCOPE(name)
#endif

auto f1()
{
    PROFILE_FUNCTION();
    auto v = std::vector<int>(1000, 1);
    for (auto &i : v) {
        std::cout << "Hello #" << sin(i + rand()) << std::endl;
    }
}
auto f2()
{
    PROFILE_FUNCTION();
    auto v = std::vector<int>(1000, 1);
    for (auto i : v) {
        std::cout << "Hello #" << sin(i + rand()) << std::endl;
    }
}

auto BenchMark()
{
    PROFILE_FUNCTION();
    printf("bench marking .... \n");
    f1();
    f2();
}

int main()
{
    return 1;
    Instrumentor::Get().BeginSession("Profile");
    BenchMark();
    Instrumentor::Get().EndSession();


    std::fstream f("results.json");
    char         buf[1024];
    f.read(buf, 1024);
    std::cout << buf << std::endl;
}