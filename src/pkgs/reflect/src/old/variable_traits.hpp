#pragma once


#include <type_traits>
namespace detail {


template <typename T>
struct variable_type {
    using type = T;
};
template <typename Class, typename T>
struct variable_type<T Class::*> {
    using type = T;
};

} // namespace detail


template <typename T>
using variable_type_t = typename detail::variable_type<T>::type;


namespace internal {

template <typename T>
struct basic_variable_traits {
    using type                      = variable_type_t<T>;
    static constexpr bool is_member = std::is_member_pointer_v<T>;
};

} // namespace internal


template <typename T>
struct variable_traits;

template <typename T>
struct variable_traits<T *> : internal::basic_variable_traits<T> {
    using pointer_type = T *;
};

template <typename Class, typename T>
struct variable_traits<T Class::*> : internal::basic_variable_traits<T Class::*> {
    using pointer_type             = T Class::*;
    using static_class             = Class;
    static constexpr bool is_const = false;
};

template <typename Class, typename T>
struct variable_traits<const T Class::*> : internal::basic_variable_traits<const T Class::*> {
    using pointer_type             = T Class::*;
    using static_class             = Class;
    static constexpr bool is_const = true;
};

int b()
{
    struct a {
        int a;
    };


    using b = variable_traits<decltype(&a::a)>::pointer_type;
    using c = variable_traits<decltype(&a::a)>::static_class;



    return 0;
}
