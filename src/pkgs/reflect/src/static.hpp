
#pragma once

#include "base.h"

TOPLEVEL_NAMESPACE_BEGIN


#pragma region TypeList

template <typename... Types>
struct type_list {
    static constexpr size_t size = sizeof...(Types);
};
// 类型列表定义


// 核心解包工具（前向声明）
template <template <typename...> class Target, typename TypeList>
struct unpack_apply;

// 特化版本实现解包
template <template <typename...> class Target, typename... Ts>
struct unpack_apply<Target, type_list<Ts...>> {
    using type = Target<Ts...>; // 关键展开点
};

// // 简化别名
// template <template <typename...> class Target, typename TypeList>
// using unpack_apply_t = typename unpack_apply<Target, TypeList>::type;

// // 定义解包操作符
// template <typename TypeList, template <typename...> class Target>
// constexpr auto operator|(TypeList, Target) -> unpack_apply_t<Target::template apply, TypeList> {};

// 定义目标模板接收器
template <template <typename...> class Target>
struct apply_adapter {
    template <typename... Ts>
    using apply = Target<Ts...>;
};



namespace detail {
template <typename, size_t>
struct nth;

// end recursive condition
template <typename T, typename... Remains>
struct nth<type_list<T, Remains...>, 0> {
    using type = T;
};

template <typename T, typename... Remains, size_t N>
struct nth<type_list<T, Remains...>, N> {
    using type = typename nth<type_list<Remains...>, N - 1>::type;
};
} // namespace detail

template <typename TypeList, size_t N>
using nth_t = detail::nth<TypeList, N>::type;

// using t1=  type_list<T1,T2,T3>
//  auto template_func<...>
// how to make template_func<t1> work? to became template_func<T1,T2,T3>

#pragma endregion

#pragma region Function


template <typename FuncSig>
struct function_traits;

template <typename Ret, typename... Params>
struct function_traits<Ret (*)(Params...)> {
    using ptr_type    = Ret (*)(Params...);
    using return_type = Ret;
    using parameters  = type_list<Params...>;
};

// no arg
template <typename Ret>
struct function_traits<Ret (*)()> {
    using ptr_type    = Ret (*)();
    using return_type = Ret;
    using parameters  = type_list<>;
};

template <typename OwnerClass, typename Ret, typename... Params>
struct function_traits<Ret (OwnerClass::*)(Params...)> {
    using ptr_type    = Ret (OwnerClass::*)(Params...);
    using owner_class = OwnerClass;
    using return_type = Ret;
    using parameters  = type_list<Params...>;
};


namespace detail {
template <typename Ret, typename... Args>
auto function_pointer_type(Ret (*)(Args...)) -> Ret (*)(Args...);
template <typename Ret, typename Class, typename... Args>
auto function_pointer_type(Ret (Class::*)(Args...)) -> Ret (Class::*)(Args...);
template <typename Ret, typename Class, typename... Args>
auto function_pointer_type(Ret (Class::*)(Args...) const) -> Ret (Class::*)(Args...) const;
} // namespace detail

// as a variable
template <auto FunctionPtr>
using function_pointer_type_t = decltype(detail::function_pointer_type(FunctionPtr));

template <auto FunctionVal>
using function_traits_t = function_traits<function_pointer_type_t<FunctionVal>>;



#pragma endregion


#pragma region Variable

template <typename>
struct variable_traits;

template <typename OwnerClass, typename T>
struct variable_traits<T OwnerClass::*> {
    using owner_class = OwnerClass;
    using type        = T;
};

template <typename OwnerClass, typename T>
struct variable_traits<const T OwnerClass::*> {
    using owner_class = OwnerClass;
    using type        = const T;
};

#pragma endregion


TOPLEVEL_NAMESPACE_END