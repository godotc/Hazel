#pragma once

#include <functional>
#include <vector>


template <typename Signature>
class Delegate;

template <typename ReturnType, typename... Args>
class Delegate<ReturnType(Args...)>
{
  public:
    // Function type alias
    using FunctionType = std::function<ReturnType(Args...)>;

    // Constructor to set the delegate function
    Delegate(){};
    Delegate(const FunctionType &function) : m_Function(function), bBound(true) {}

    void Set(const FunctionType &function)
    {
        m_Function = function;
        bBound     = true;
    }


    // Execute the delegate with arguments
    ReturnType operator()(Args... args) const
    {
        return m_Function(args...);
    }

    ReturnType ExecuteIfBound(Args... args) const
    {
        if (bBound) {
            return m_Function(args...);
        }
    }


  private:
    FunctionType m_Function;
    bool         bBound = false;
};



template <typename... Args>
class MulticastDelegate
{
  public:
    // Function type alias
    using FunctionType = std::function<void(Args...)>;

    // Constructor to set the delegate function
    MulticastDelegate(){};

    void Add(const FunctionType &function)
    {
        m_Functions.push_back(function);
    }

    void Brocast(Args... args) const
    {
        for (const auto &func : m_Functions) {
            func(args...);
        }
    }

  private:
    std::vector<FunctionType> m_Functions;
};