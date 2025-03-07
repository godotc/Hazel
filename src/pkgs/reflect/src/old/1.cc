
#include <cstddef>
#include <tuple>
#include <type_traits>
bool foo(int) { return false; }

struct Person {
    int  age;
    bool IsTransaxle();
    bool IsFemale();
};


//-- Begin function
template <class T>
struct function_traits;

template <class Ret, class... Arg>
struct function_traits<Ret (*)(Arg...)> {
    using return_type = Ret;
    using args        = std::tuple<Arg...>;
};

template <class Ret, class Class, class... Arg>
struct function_traits<Ret (Class::*)(Arg...)> {
    using return_type              = Ret;
    using args                     = std::tuple<Arg...>;
    using class_type               = Class;
    static constexpr bool is_class = true;
};
//-- End Function

template <class TupleTypes, size_t Index>
struct get_arg {
    using type = std::remove_reference_t<decltype(std::get<Index>(std::declval<TupleTypes>()))>;
};
template <class TupleTypes, size_t Index>
using get_arg_t = get_arg<TupleTypes, Index>::type;




template <class T>
struct variable_traits;

template <class Type, class Class>
struct variable_traits<Type(Class::*)> {
    using type         = Type;
    using static_class = Class;
};



template <class T>
struct TypeInfo;

// clang-format off
#define BEGIN_CLASS(T)      \
  template <> struct TypeInfo<T> \
  { using type = T;   \

#define FUNCTIONS(...) using functions = std::tuple<__VA_ARGS__>;
#define FUNCTION(X) function_traits<decltype((X))>
#define END_CLASS() };



BEGIN_CLASS(Person)
  FUNCTIONS(
      FUNCTION(&Person::IsFemale),
      FUNCTION(&Person::IsTransaxle)
  )
END_CLASS()
// clang-format off

int main()
{

    auto ptr            = &foo;
    using function_info = function_traits<decltype(ptr)>;
    using ret           = function_info::return_type;
    using args          = function_info::args;

    using first_arg     = get_arg_t<args, 0>;
    static_assert(std::is_same_v<first_arg, int>);


    //  show_impl<param>{};
    //
    using info                        = function_traits<decltype(&Person::IsFemale)>;
    bool (Person::*is_transaxle)(void) = &Person::IsTransaxle;
    int(Person::*the_age)             = &Person::age;

    // variable
    using var_info = variable_traits<decltype(&Person::age)>;
    using type     = var_info::type;
}
