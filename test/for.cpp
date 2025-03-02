
// #include <cstdio>
// #include <functional>
// #include <type_traits>
// #include <utility>



// template <typename... Args>
// struct type_list {
//     static constexpr size_t size = sizeof...(Args);
// };

// namespace detail {

// template <typename>
// struct head;
// template <typename T, typename... Remains>
// struct head<type_list<T, Remains...>> {
//     using type = T;
// };

// template <typename>
// struct tail;
// template <typename T, typename... Remains>
// struct tail<type_list<T, Remains...>> {
//     using type = type_list<Remains...>;
// };

// template <typename, size_t>
// struct nth;

// // end recursive condition
// template <typename T, typename... Remains>
// struct nth<type_list<T, Remains...>, 0> {
//     using type = T;
// };

// template <typename T, typename... Remains, size_t N>
// struct nth<type_list<T, Remains...>, N> {
//     using type = typename nth<type_list<Remains...>, N - 1>::type;
// };

// template <typename, template <typename> typename, size_t N>
// struct count;
// template <typename T, typename... Remains, template <typename> typename Fn>
// struct count<type_list<T, Remains...>, Fn, 0> {
//     static constexpr size_t value = (Fn<T>::value ? 1 : 0);
// };
// template <typename T, typename... Remains, template <typename> typename Fn, size_t N>
// struct count<type_list<T, Remains...>, Fn, N> {
//     static constexpr size_t value =
//         (Fn<T>::value ? 1 : 0) +
//         count<type_list<Remains...>, Fn, N - 1>::value;
// };

// template <typename, template <typename> typename>
// struct map;
// template <typename... Args, template <typename> typename Fn>
// struct map<type_list<Args...>, Fn> {
//     using type = type_list<typename Fn<Args>::type...>;
// };


// template <typename, typename>
// struct cons;
// template <typename T, typename... Args>
// struct cons<T, type_list<Args...>> {
//     using type = type_list<T, Args...>;
// };

// template <typename, typename>
// struct concat;
// template <typename... Args1, typename... Args2>
// struct concat<type_list<Args1...>, type_list<Args2...>> {
//     using type = type_list<Args1..., Args2...>;
// };

// template <typename>
// struct init;
// template <typename T>
// struct init<type_list<T>> {
//     using type = type_list<>;
// };
// template <typename T, typename... Remains>
// struct init<type_list<T, Remains...>> {
//     using type = typename cons<T, typename init<type_list<Remains...>>::type>::type;
// };

// template <typename, template <typename> typename>
// struct filter;
// template <template <typename> typename Fn>
// struct filter<type_list<>, Fn> {
//     using type = type_list<>;
// };
// template <typename T, typename... Remains, template <typename> typename Fn>
// struct filter<type_list<T, Remains...>, Fn> {
//     using type = std::conditional_t<
//         Fn<T>::value,
//         typename cons<T, typename filter<type_list<Remains...>, Fn>::type>::type, // add
//         typename filter<type_list<Remains...>, Fn>::type>;                        // continue to remains
// };

// } // namespace detail


// // template <typename, template <typename> typename>


// template <typename TypeList>
// using head_t = typename detail::head<TypeList>::type;
// template <typename TypeList>
// using tail_t = typename detail::tail<TypeList>::type;
// template <typename TypeList, size_t N>
// using nth_t = typename detail::nth<TypeList, N>::type;

// template <typename TypeList, template <typename> typename Fn>
// constexpr int count = detail::count<TypeList, Fn, TypeList::size - 1>::value;

// template <typename T, typename... Args>
// using cons = typename detail::cons<T, Args...>::type;

// template <typename TypeList1, typename TypeList2>
// using concat = typename detail::concat<TypeList1, TypeList2>::type;

// template <typename TypeList, template <typename> typename Fn>
// using filter = typename detail::filter<TypeList, Fn>::type;



// template <class Type>
// void f(Type dummy)
// {
//     if (std::is_same_v<float, decltype(dummy)>) {
//         printf("www");
//     }
// }



// // template <typename T, typename... Remains>
// // void for_each(std::function<T &&> &f)
// // {
// //     f();
// //     for_each<type_list<Remains...>>(f);
// // };



// // template <typename TypeList>
// // void for_each()
// // {
// //     printf("for_each: %s\n", typeid(T).name());
// //     if (sizeof...(Remains) > 0) {
// //         for_each<type_list<Remains...>>();
// //     }
// // };

// template <int StartIndex, int UpperBound, class Fn, class... Arg>
// void static_for(Fn func, Arg... args)
// {
//     if constexpr (StartIndex < UpperBound) {

//         constexpr int index = StartIndex;
//         func(index, std::forward<Arg>(args)...);
//         static_for<StartIndex + 1, UpperBound>(func, std::forward<Arg>(args)...);
//     }
// }



// using list = type_list<int, double, float>;


// template <class TypeList, class Fn, class... Arg>
// void foreach_types(Fn func, Arg... args)
// {

//     if constexpr (TypeList::size > 0)
//     {
//         using CurType = head_t<TypeList>;
//         func(std::declval<CurType>(), std::forward<Arg>(args)...);
//         foreach_types<tail_t<TypeList>>(func, std::forward<Arg>(args)...);
//     }
// }

// // template <class TypeList, template <typename> typename Fn, class... Arg>
// // void foreach_types_v1(Fn func, Arg... args)
// // {
// //     if constexpr (TypeList::size > 0)
// //     {
// //         using CurType = head_t<TypeList>;
// //         func<CurType>(std::forward<Arg>(args)...);
// //         foreach_types_v1<tail_t<TypeList>>(func, std::forward<Arg>(args)...);
// //     }
// // }

// // template <typename Type>
// // void ff()
// // {
// //     std::declval<Type>();
// }

int main()
{
    // using component_types = type_list<int, double, float>;
    // foreach_types<component_types>([](auto v) {
    //     printf("%s\n", typeid(v).name());
    // });
    // foreach_types_v1<component_types>(ff);
}
