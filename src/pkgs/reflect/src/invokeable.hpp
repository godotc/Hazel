#pragma once

#include "base.h"
#include "dynamic_any.hpp"
#include "static.hpp"

TOPLEVEL_NAMESPACE_BEGIN



struct Invokable {
    virtual DynamicAny invoke(DynamicAny instance, const std::vector<DynamicAny> &args) = 0;
    // virtual DynamicAny invoke(const std::vector<DynamicAny> &args)                      = 0;
    virtual ~Invokable() = default;
};

template <typename FuncPtr>
struct InvokableImpl : Invokable {

    FuncPtr func_ptr;

    explicit InvokableImpl(FuncPtr ptr) : func_ptr(ptr) {}

    DynamicAny invoke(DynamicAny instance, const std::vector<DynamicAny> &args) override
    {
        using traits      = function_traits<FuncPtr>;
        using class_type  = typename traits::owner_class;
        using return_type = typename traits::return_type;
        using parameters  = typename traits::parameters;


        // 检查参数数量是否匹配
        if (args.size() != parameters::size) {
            throw std::runtime_error("参数数量不匹配");
        }

        class_type *obj = instance.as<class_type>();
        return invokeClasFunctionImpl<class_type, return_type, parameters>(obj, args, std::make_index_sequence<parameters::size>{});
    }



  private:
    template <typename Class, typename Ret, typename Parameters, size_t... I>
    DynamicAny invokeClasFunctionImpl(Class *obj, const std::vector<DynamicAny> &args, std::index_sequence<I...>)
    {
        // clang-format off
        if constexpr (std::is_void_v<Ret>) {
            (obj->*func_ptr)(
                // if  (int) , and arg[1] was a DynamicAny::make_copy(42)
                (
                    *(args[I].as<nth_t<Parameters, I>>()) // as<T> will return T*
                )...
            );
            return DynamicAny(); // 返回空Any表示void
        }
        else {
            auto ret = (obj->*func_ptr)(args[I].as<nth_t<Parameters, I> *>()...);
            return DynamicAny::make_steal(ret);
        }
        // clang-format on
    }
};

TOPLEVEL_NAMESPACE_END