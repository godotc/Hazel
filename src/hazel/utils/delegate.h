//
/*
 * @ Author: godot42
 * @ Modified by: @godot42
 * @ Modified time: 2024-12-28 04:26:58
 * @ Modified time: 2024-12-28 04:27:37
 * @ Description:
 */

#pragma once

#include <functional>
#include <memory>
#include <utility>
#include <vector>

template <typename Signature>
class Delegate;

template <typename ReturnType, typename... Args>
class Delegate<ReturnType(Args...)>
{
  public:
    using FunctionType = std::function<ReturnType(Args...)>;

    Delegate(){};
    Delegate(const FunctionType &function) : m_Function(function), bBound(true) {}

    void Bind(const FunctionType &function)
    {
        m_Function = function;
        bBound     = true;
    }

    void Unbind() { bBound = false; }

    ReturnType ExecuteIfBound(Args ...args) const
    {
        if (bBound) {
            return m_Function(std::forward<Args>(args)...);
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
    using FunctionType = std::function<void(Args...)>;

    MulticastDelegate(){};

    void Add(const FunctionType &function) { m_Functions.push_back(function); }

    template <typename Obj>
    void AddWeak(Obj *obj, void (Obj::*memberFunc)(Args...))
    {
        std::weak_ptr<Obj> weak_obj(obj);
        m_Functions.push_back([weak_obj, memberFunc](Args... args) {
            if (auto objPtr = weak_obj.lock()) {
                (objPtr.get()->*memberFunc)(std::forward<Args>(args)...);
            }
        });
    }


    void Broadcast(Args... args) const
    {
        for (const auto &func : m_Functions) {
            func(std::forward<Args>(args)...);
        }
    }

  private:
    std::vector<FunctionType> m_Functions;
};