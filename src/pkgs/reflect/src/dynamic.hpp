#pragma once

#include <string>


#include <assert.h>
#include <functional>
#include <iostream>
#include <set>
#include <typeinfo>


#include "Base.h"
#include "dynamic_any.hpp"
#include "invokeable.hpp"
#include "static.hpp"



TOPLEVEL_NAMESPACE_BEGIN

class Type;
class Numeric;
class Enum;
class Class;
class DynamicAny;

template <typename T>
class Factory;


extern std::set<const Type *> &getGlobalTypes();


template <typename T>
const Type *getType();

template <typename T>
auto &registerType();


#pragma region Type



struct Type {

    enum EKind
    {
        Void,
        Numeric,
        Enum,
        Class,
        Function,

    };

    std::string name;
    EKind       kind;

    Type(std::string_view name, EKind kind) : name(name), kind(kind) {}
    virtual ~Type() = default;

    std::string_view getName() const { return name; }
    EKind            getKind() const { return kind; }

    const class Numeric *asNumeric() const;

    static std::string_view kindToString(EKind kind)
    {
        switch (kind) {
        case EKind::Void:
            return "Void";
        case EKind::Numeric:
            return "Numeric";
        case EKind::Enum:
            return "Enum";
        case EKind::Class:
            return "Class";
        case EKind::Function:
            return "Function";
        default:
            return "Unknown";
        }
    }
};



namespace Test {

#if WITH_TEST
extern void typeTest();
#endif

} // namespace Test


#pragma endregion

#pragma region Void

struct Void : Type {

    Void() : Type("Void", Type::EKind::Void) {}
    static Void create()
    {
        return Void();
    }
};

class VoidFactory final
{
    Void typeInfo;

  public:
    static VoidFactory &instance()
    {
        static VoidFactory instance;
        std::once_flag     flag;
        std::call_once(
            flag,
            [](auto &instance) {
                getGlobalTypes().insert(&instance.typeInfo);
            },
            instance);
        return instance;
    }

    const Void &getTypeInfo() const { return typeInfo; }
};
#pragma endregion

#pragma region Numeric
struct Numeric : Type {

    enum class EKind
    {
        Unknown,
        Int8,
        Int16,
        Int32,
        Int64,
        Float,
        Double,
        Bool,
        Void,
    };

    union
    {
        int8_t  int8Value;
        int16_t int16Value;
        int32_t int32Value;
        int64_t int64Value;
        float   floatValue;
        double  doubleValue;
#if __ENABLE_LONG_DOUBLE
        long double longDoubleValue;
#endif
        bool boolValue;
    };

    EKind kind;
    bool  bSigned;

    Numeric(EKind kind, bool bSigned)
        : Type(getName(kind), Type::EKind::Numeric)
    {
        kind    = kind;
        bSigned = bSigned;
    }

    template <typename T>
    static Numeric create()
    {
        return Numeric{DetectKind<T>(), std::is_signed_v<T>};
    }

    static const char *getName(EKind kind)
    {
        switch (kind) {
        case EKind::Int8:
            return "Int8";
        case EKind::Int16:
            return "Int16";
        case EKind::Int32:
            return "Int32";
        case EKind::Int64:
            return "Int64";
        case EKind::Float:
            return "Float";
        case EKind::Double:
            return "Double";
        case EKind::Bool:
            return "Bool";
        case EKind::Unknown:
            return "Unknown";
        case EKind::Void:
            return "Void";
            break;
        }

        assert(false);
        return "";
    }

    template <typename T>
    bool setValue(T value)
    {
        EKind          kind    = DetectKind<T>();
        constexpr bool bSigned = std::is_signed_v<T>;
        this->bSigned          = bSigned;
        switch (kind) {
        case EKind::Unknown:
            return false;
        case EKind::Int8:
            int8Value = value;
            break;
        case EKind::Int16:
            int16Value = value;
            break;
        case EKind::Int32:
            int32Value = value;
            break;
        case EKind::Int64:
            int64Value = value;
            break;
        case EKind::Float:
            floatValue = value;
        case EKind::Double:
            doubleValue = value;
        case EKind::Bool:
            boolValue = value;
            break;
        }
    }

    template <typename T>
    static EKind DetectKind()
    {
        using t1 = std::remove_reference_t<T>;



        if constexpr (false) {
            void *v;
            using tv = decltype(v);

            auto a = std::is_integral_v<tv>;
            auto b = std::is_floating_point_v<tv>;
            auto c = std::is_pointer_v<tv>;
            auto d = std::is_null_pointer_v<tv>;
        }


        if constexpr (std::is_integral_v<t1>)
        {
            if constexpr (std::is_same_v<t1, bool>) {
                return EKind::Bool;
            }

            if constexpr (sizeof(t1) == 1) {
                return EKind::Int8;
            }
            else if constexpr (sizeof(t1) == 2) {
                return EKind::Int16;
            }
            else if constexpr (sizeof(t1) == 4) {
                return EKind::Int32;
            }
            else if constexpr (sizeof(t1) == 8) {
                return EKind::Int64;
            }
        }
        else if constexpr (std::is_floating_point_v<t1>)
        {
            if constexpr (std::is_same_v<t1, float>) {
                return EKind::Float;
            }
            else if constexpr (std::is_same_v<t1, double>) {
                return EKind::Double;
            }
#if __ENABLE_LONG_DOUBLE__
            else if constexpr (std::is_same_v<t1, long double>) {
                return EKind::Double;
            }
#endif
        }
        else if constexpr (std::is_void_v<t1>) {
            return EKind::Void;
        }



        // what type is T?
        fprintf(stderr, "Unknown type: %s\n", typeid(T).name());
        assert(false);
        return EKind::Unknown;
    }
};


template <typename T>
class NumericFactory final
{
    Numeric typeInfo;

  public:
    static NumericFactory &instance()
    {
        static NumericFactory instance = []() {
            NumericFactory instance{Numeric::create<T>()};
            getGlobalTypes().insert(&instance.getTypeInfo());
            return instance;
        }();
        return instance;
    }

    NumericFactory(Numeric &&info) : typeInfo(std::move(info))
    {
    }
    const Numeric &getTypeInfo() const { return typeInfo; }

    // void Unregister()
    // {
    //      TODO
    //     info = Numeric();
    // }
};

#pragma endregion



#pragma region Class


struct ClassMember {
};


#pragma region ClassFunction



struct ClassFunction : public Type {
    using Any = DynamicAny;

    std::string               name;
    const Type               *returnType;
    std::vector<const Type *> parameterTypes;
    // void(*ptr);

    Invokable *invoke_ptr;


    ClassFunction(const std::string &name)
        : Type(name, Type::EKind::Function)
    {
    }

    Any call(Any &instance, const std::vector<Any> &args)
    {
        return invoke_ptr->invoke(instance, args);
    }


    template <typename Func>
    static ClassFunction create(const std::string &name, Func func)
    {
        using func_t      = decltype(func);
        using traits      = function_traits<func_t>;
        using owner       = typename traits::owner_class;
        using return_type = typename traits::return_type;
        using parameters  = typename traits::parameters;

        if constexpr (false) {
            struct A {
                void a() { printf("baga\n"); }
            };

            auto cf = ClassFunction::create<decltype(&A::a)>("a");
        }

        // void(*ptr) = new (typename traits::ptr_type)(T);

        auto ret           = ClassFunction(name);
        ret.returnType     = getType<return_type>(),
        ret.parameterTypes = convertTypeListToVector<parameters>(std::make_index_sequence<traits::parameters::size>()),
        ret.invoke_ptr     = new InvokableImpl<func_t>(func);
        return ret;
    }

  private:
    template <typename Params, size_t... Idx>
    static std::vector<const Type *> convertTypeListToVector(std::index_sequence<Idx...>)
    {
        return {getType<nth_t<Params, Idx>>()...};
    }
};


namespace Test {
#if WITH_TEST
inline void classFunctionTest()
{
    struct A {
        void a() { printf("============ %s calling\n", __FUNCTION_SIG); }

        void printInt(int n)
        {
            printf("=============%s %d\n", __FUNCTION_SIG, n);
        }
    };

    auto pA = &A::a;

    using traits = function_traits<decltype(pA)>;
    using r      = traits::return_type;
    using b      = traits::parameters;
    auto n       = traits::parameters::size;



    A          aInstance;
    DynamicAny aInstanceConstRef = DynamicAny::make_const_ref(aInstance);

    ClassFunction func = ClassFunction::create("a", &A::a);
    func.call(aInstanceConstRef, {});



    ClassFunction func2 = ClassFunction::create("printInt", &A::printInt);
    func2.call(aInstanceConstRef, {DynamicAny::make_copy(42)});
    // this for store ptr in the class?
    // auto ptr = std::make_unique<ClassFunction>(std::move(func));
}

#endif
} // namespace Test


#pragma endregion


#pragma region ClassVariable

struct ClassVariable {
    using Any = DynamicAny;

    // Class      *ownerClass;
    std::string name;
    const Type *type;

    template <typename OwnerClass, typename... Args>
    Any call(const std::vector<Any> &args);

    template <typename T>
    static ClassVariable create(const std::string &name)
    {
        // using traits = variable_traits<T>;
        return ClassVariable{
            // .ownerClass = ownerClass,
            // .name = name,
            // .type = traits::type,
        };
    }
};

#pragma endregion


class Class : public Type
{
    using Any = DynamicAny;

  public:
  private:
    std::vector<ClassMember> functions;
    std::vector<ClassMember> variables;

  public:
    Class() : Type("Unknown-Class", Type::EKind::Class) {}
    Class(const std::string &name) : Type(name, Type::EKind::Class) {}


    void addFunction(ClassFunction &&member)
    {
        UNIMPLEMENTED();
        // functions.emplace_back(std::move(member));
    }
    void addVariable(ClassVariable &&member)
    {
        UNIMPLEMENTED();
        // variables.emplace_back(std::move(member));
    }


    template <typename OwnerClass, typename RetType, size_t... Idx, typename... Args>
    static Any callMemberFunctionImpl(RetType (OwnerClass::*ptr)(Args...), const std::vector<Any> &args, std::index_sequence<Idx...>)
    {
        OwnerClass *instance = (OwnerClass *)args[0].getPayload();
        auto        ret      = instance->*ptr(unwrap<Args>(args[Idx + 1])...);
        return make_copy(ret);
    }
};

namespace Test {
#if WITH_TEST
void classTest();
#endif

} // namespace Test


template <typename T>
class ClassFactory final
{
    Class typeInfo;

  public:
    static ClassFactory &instance()
    {
        static ClassFactory<T> instance;
        static std::once_flag  flag;
        std::call_once(
            flag,
            [](ClassFactory<T> &instance) {
                debug(), "Class Factory instance created : [", typeid(T).name(), "]";
                getGlobalTypes().insert(&instance.getTypeInfo());
            },
            instance);
        return instance;
    }

    ClassFactory() = default;

    ClassFactory(Class &&info) : typeInfo(std::move(info))
    {
    }

    ClassFactory &registerName(std::string_view name)
    {
        typeInfo.name = name;
        return *this;
    }

    template <typename MemberVariable>
    ClassFactory &addVariable(std::string_view name)
    {
        // typeInfo.addVariable(ClassVariable::create<MemberVariable>(name));
        return *this;
    }

    template <typename FunctionType>
    ClassFactory &addFunction(std::string_view name)
    {
        auto func = ClassFunction::create<FunctionType>(std::string(name));
        typeInfo.addFunction(std::move(func));
        return *this;
    }


    const Class &getTypeInfo() const { return typeInfo; }

    // void Unregister()
    // {
    //      TODO
    //     info = Numeric();
    // }
};

#pragma endregion



struct TrivialFactory {
    static TrivialFactory &instance()
    {
        static TrivialFactory instace;
        return instace;
    }
};



template <typename T>
class Factory final
{
  public:
    static auto &getFactory()
    {

        using t1 = std::remove_reference_t<T>;

        // std::is_member_function_pointer<>
        if constexpr (std::is_function_v<t1>) {
            UNIMPLEMENTED();
            assert(false);
            return TrivialFactory::instance();
        }
        else if constexpr (std::is_arithmetic_v<t1>) {
            return NumericFactory<T>::instance();
        }
        else if constexpr (std::is_void_v<t1>) {
            return VoidFactory::instance();
        }
        // else if constexpr (std::is_enum_v<t1>) {
        //     return EnumFactory<T>::Instance();
        // }
        else if constexpr (std::is_class_v<t1>) {
            return ClassFactory<T>::instance();
        }
        else {
            debug(), "Getting factory of ", typeid(T).name();
            assert(false);
            return TrivialFactory::instance();
        }
    }
};



#pragma
// -------------------------inline tempalte functions-------------------------


template <typename T>
const Type *getType()
{
    return &Factory<T>::getFactory().getTypeInfo();
}

template <typename T>
auto &registerType()
{
    return Factory<T>::getFactory();
}

template <typename OwnerClass, typename... Args>
inline DynamicAny ClassVariable::call(const std::vector<Any> &args)
{
    assert(args.size() == 1 && args[0].getTypeInfo() == getType<Class>());
    return Class::callMemberFunctionImpl(args, std::make_index_sequence<sizeof...(Args)>());
}

namespace Test {

#if WITH_TEST

#endif
} // namespace Test



TOPLEVEL_NAMESPACE_END
