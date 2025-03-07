#pragma once

#include "function_traits.hpp"
#include "variable_traits.hpp"
#include <string_view>
#include <type_traits>



namespace detail {
template <typename Ret, typename... Args>
auto function_pointer_type(Ret (*)(Args...)) -> Ret (*)(Args...);
template <typename Ret, typename Class, typename... Args>
auto function_pointer_type(Ret (Class::*)(Args...)) -> Ret (Class::*)(Args...);
template <typename Ret, typename Class, typename... Args>
auto function_pointer_type(Ret (Class::*)(Args...) const) -> Ret (Class::*)(Args...) const;
} // namespace detail

// as a variable
template <auto FunctionVal>
using function_pointer_type_t = decltype(detail::function_pointer_type(FunctionVal));

template <auto FunctionVal>
using function_traits_t = function_traits<function_pointer_type_t<FunctionVal>>;

namespace detail {

template <typename T>
struct is_function {
    static constexpr bool value =
        std::is_function_v<std::remove_pointer_t<T>> || std::is_member_function_pointer_v<T>;
};
} // namespace detail

template <typename T>
static constexpr bool is_function_v = detail::is_function<T>::value;


namespace internal {


template <typename T, bool IsFunc>
struct base_field_traits;

template <typename T>
struct base_field_traits<T, true> : public function_traits<T> {
    using traits = function_traits<T>;
    constexpr bool is_member() const { return traits::is_member; }
    constexpr bool is_const() const { return traits::is_const; }
    // we partial IsFunc to true
    constexpr bool   is_function() const { return true; }
    constexpr bool   is_variable() const { return false; }
    constexpr size_t param_count() const { return std::tuple_size_v<typename traits::args>; }
};

template <typename T>
struct base_field_traits<T, false> : public variable_traits<T> {
    using traits = variable_traits<T>;
    constexpr bool is_member() const { return traits::is_member; }
    constexpr bool is_const() const { return traits::is_const; }
    // we partial IsFunc to true
    constexpr bool is_function() const { return false; }
    constexpr bool is_variable() const { return true; }
};

} // namespace internal

template <typename T>
struct field_traits;


template <typename T>
struct field_traits : public internal::base_field_traits<T, is_function_v<T>> {

    constexpr field_traits(T &&pointer, std::string_view name)
        : pointer{std::forward<T>(pointer)},
          name{name.substr(name.find_last_of(":") + 1)}
    {
        // name = name.substr(name.find_last_of(":") + 1); // must in contraction brace {} initializes
    }

    T                pointer;
    std::string_view name;
};


template <class T>
struct TypeInfo;

// clang-format off
#define BEGIN_CLASS(X) template <> struct TypeInfo<X> {
#define functions(...) static constexpr auto functions = std::make_tuple(__VA_ARGS__);
#define func(F) field_traits { F, #F }
#define END_CLASS() };
// clang-format on


template <typename T>
constexpr auto reflect_info()
{
    return TypeInfo<T>{};
}



#include <iostream>



struct Person1 final {
    std::string last_name;
    float       height;
    // bool        is_transaxle;
    bool is_female;

    void Sleep() const
    {
        printf("Zzzzz....\n");
    }
    // bool IsTransaxle() const { return is_transaxle; }
    bool IsFemale() const
    {
        printf("???....\n");
        return is_female;
    }
    bool GetMarriedWith(Person1 &other)
    {
        if (other.is_female != is_female) //&& is_transaxle != other.is_transaxle;)
        {
            if (is_female) {
                last_name = "Mrs." + other.last_name;
            }
            else {
                last_name = "Mr." + last_name;
            }
            return true;
        }
        return false;
    }
};



BEGIN_CLASS(Person1)
functions(
    func(&Person1::Sleep),
    func(&Person1::IsFemale),
    func(&Person1::GetMarriedWith));
END_CLASS();



template <size_t... Index, typename Tuple, typename Function>
void VisitTuple(Tuple tuple, Function &&func, std::index_sequence<Index...>)
{
    (func(std::get<Index>(tuple)), ...);
}

template <size_t Index, typename... Args, typename Class>
void VisitTuple(const std::tuple<Args...> &tuple, Class *instance)
{
    using tuple_type = std::tuple<Args...>;

    if constexpr (Index > std::tuple_size_v<tuple_type>) {
        return;
    }

    if constexpr (auto elem = std::get<Index>(tuple);
                  elem.param_count() == 0) {
        if (instance && elem.pointer) {
            (instance->*elem.pointer)();
        }
        VisitTuple<Index + 1>(tuple, instance);
    }
}


inline void srefl_test()
{
    using type1 = function_pointer_type_t<&Person1::GetMarriedWith>;
    static_assert(std::is_same_v<type1, bool (Person1::*)(Person1 &)>);


    constexpr auto info = reflect_info<Person1>();
    std::cout << std::get<0>(info.functions).name << std::endl;

    auto field = field_traits{&Person1::GetMarriedWith, "&Person1::GetMarriedWith"};
    std::cout << field.pointer << std::endl;
    std::cout << field.is_member() << std::endl;
    std::cout << field.is_const() << std::endl;
    std::cout << (field.pointer == &Person1::GetMarriedWith) << std::endl;

    auto field2 = field_traits{&Person1::IsFemale, "&Person1::GetMarriedWith"};
    std::cout << field2.is_const() << std::endl;



    Person1 p;
    VisitTuple<0>(info.functions, &p);
    using tuple_type = decltype(info.functions);

    // TODO: Fix this, code cannot compile
    // VisitTuple(
    //     info.functions, [](auto &&elem) {
    //         std::cout << elem.name << std::endl;
    //     },
    //     std::make_index_sequence<std::tuple_size_v<tuple_type>>());
}
