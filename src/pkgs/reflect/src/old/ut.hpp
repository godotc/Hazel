#pragma once

#include <chrono>
#include <concepts>
#include <cstddef>
#include <functional>
#include <iostream>
#include <optional>
#include <ostream>
#include <system_error>
#include <type_traits>
#include <utility>
#include <variant>


template <int Cur, std::size_t UpperBound, typename Fn, typename... Ts>
inline auto static_for(Fn func, Ts... ts)
{
    if constexpr (Cur < UpperBound) {
        func(std::integral_constant<int, Cur>{}, std::forward<Ts>(ts)...);
        static_for<Cur + 1, UpperBound>(func, std::forward<Ts>(ts)...);
    }
}

/*
template <class... Ts>
inline std::ostream &operator<<(std::ostream &out, std::variant<Ts...> v)
{

    bool ok = false;
    static_for<0, sizeof...(Ts)>([&ok, &v, &out](auto i) {
        if (ok) {
            return;
        }
        using t = std::variant_alternative_t<i.value, std::variant<Ts...>>;
        if (const t *val = std::get_if<t>(&v)) {
            // if constexpr (std::holds_alternative<i.value>(v)) {
            out << *val;
            ok = true;
        }
    });

    if (!ok) {
        out << "Invlaid variant...., index: ", v.index();
    }

    return out;
}

*/
