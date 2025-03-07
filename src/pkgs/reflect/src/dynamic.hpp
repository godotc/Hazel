#pragma once

#include <string>


#include <assert.h>
#include <functional>
#include <set>
#include <typeinfo>

#include "Base.h"



#include <iostream>



namespace TOPLEVEL_NAMESPACE {

class Type;
class Numeric;
class Enum;
class Class;
class EnhanceAny;

template <typename T>
class Factory;



extern std::set<const Type *> &getGlobalTypes();

template <typename T>
const Type *getType();



struct Type {

    enum EKind
    {
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

    static std::string_view kindToString(EKind kind);
};


namespace Test {

#if WITH_TEST
inline void typeTest()
{
    Type  t("test", Type::EKind::Numeric);
    auto &global_types = getGlobalTypes();
    global_types.insert(&t);
}
#endif

} // namespace Test


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
        Bool
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
            break;
        }
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
            UNIMPLEMENTED();
        case EKind::Double:
        case EKind::Bool:
            break;
        }
    }

    template <typename T>
    static EKind DetectKind()
    {
        using t1 = std::remove_reference_t<T>;

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



class Class : public Type
{
    struct Member {
    };

  private:
    std::vector<Member> functions;
    std::vector<Member> variables;

  public:
    Class() : Type("Unknown-Class", Type::EKind::Class) {}
    Class(const std::string &name) : Type(name, Type::EKind::Class) {}

    template <typename T>
    static Class create()
    {
        return Class{};
    }
};


template <typename T>
class ClassFactory final
{
    Class typeInfo;

  public:
    static ClassFactory &instance()
    {
        static ClassFactory instance = []() {
            ClassFactory instance{Class::create<T>()};
            getGlobalTypes().insert(&instance.getTypeInfo());
            return instance;
        }();
        return instance;
    }

    ClassFactory(Class &&info) : typeInfo(std::move(info))
    {
    }

    ClassFactory &registerName(std::string_view name)
    {
        typeInfo.name = name;
        return *this;
    }

    template <typename VariableType>
    ClassFactory &addVariable(std::string_view name)
    {
        UNIMPLEMENTED();
        return *this;
    }

    template <typename FunctionType>
    ClassFactory &addFunction(std::string_view name)
    {
        UNIMPLEMENTED();
        return *this;
    }


    const Class &getTypeInfo() const { return typeInfo; }

    // void Unregister()
    // {
    //      TODO
    //     info = Numeric();
    // }
};


template <typename T>
class Factory final
{
  public:
    static auto &getFactory()
    {
        using t1 = std::remove_reference_t<T>;

        if constexpr (std::is_fundamental_v<t1>) {
            return NumericFactory<T>::instance();
        }
        // else if constexpr (std::is_enum_v<t1>) {
        //     return EnumFactory<T>::Instance();
        // }
        else if constexpr (std::is_class_v<t1>) {
            return ClassFactory<T>::instance();
        }
        // else {
        //     return TrivialFactory::Instance();
        // }
    }
};


namespace Test {

#if WITH_TEST
inline void test()
{
    const Type *intType = getType<int>();
    intType->asNumeric();
}
#endif
} // namespace Test
}; // namespace TOPLEVEL_NAMESPACE
