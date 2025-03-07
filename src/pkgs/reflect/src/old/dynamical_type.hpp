#pragma once

#include "function_traits.hpp"
#include "variable_traits.hpp"
#include <cassert>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>



class Type;
class Numeric;
class Enum;
class Class;
class Any;



inline std::set<const Type *> &GlobalTypes()
{
    static std::set<const Type *> global_types;
    return global_types;
};


template <typename T>
inline const Type *GetType();


class Type
{
  public:
    template <class T>
    friend class EnumFactory;
    template <class T>
    friend class ClassFactory;

    enum class EKind
    {
        Numeric,
        Enum,
        Class,
    };



  private:
    std::string name;
    EKind       kind;
    // TODO: add reference type


  public:
    Type(const std::string &name, EKind kind) : name(name), kind(kind) {}
    virtual ~Type() = default;

    std::string GetName() const { return name; }
    EKind       GetKind() const { return kind; }

    const Numeric *AsNumeric() const;
    const Enum    *AsEnum() const;
    const Class   *AsClass() const;

    static std::string EKind2String(EKind kind)
    {
        switch (kind) {
        case EKind::Numeric:
            return "Numeric";
        case EKind::Enum:
            return "Enum";
        case EKind::Class:
            return "Class";
        }
    }
};


class Any
{
  public:
    template <class T>
    friend class any_operations_traits;

    friend class Numeric;

    enum class EStorageType
    {
        Empty,
        Owned,
        Stolen,
        Ref,
        ConstRef,
    };

    struct Operations {
        std::function<Any(const Any &)> copy;
        std::function<Any(Any &)>       steal;
        std::function<void(Any &)>      release;
    };


  private:
    template <class T>
    struct operation_traits {
        static Any copy(const Any &elem)
        {
            assert(elem.typeinfo == GetType<T>());

            Any ret;
            ret.payload      = new T{*static_cast<T *>(elem.payload)}; // erase type
            ret.typeinfo     = elem.typeinfo;
            ret.storage_type = Any::EStorageType::Owned;
            ret.operations   = elem.operations;
            return ret;
        }

        static Any steal(Any &elem)
        {
            assert(elem.typeinfo == GetType<T>());
            Any ret;

#if __SSO
            if constexpr (sizeof(T) <= sizeof(Any::small_size_object)) {
                // Emplacement New:  call the construct on a memory that already allocated which on stack
                new (ret.small_size_object) T{*static_cast<T *>(other.payload)};
            }
            else {
                ret.payload = new T{*static_cast<T *>(other.payload)};
            }
#else
            ret.payload = new T{std::move(*static_cast<T *>(elem.payload))}; // erase type
#endif
            ret.typeinfo      = elem.typeinfo;
            ret.storage_type  = Any::EStorageType::Owned;
            elem.storage_type = Any::EStorageType::Stolen; // update origin's data state
            ret.operations    = elem.operations;
            return ret;
        }


        static void release(Any &elem)
        {
            assert(elem.typeinfo == GetType<T>());

            delete static_cast<T *>(elem.payload); // TODO: maybe force convert (void*)
            elem.payload      = nullptr;
            elem.storage_type = Any::EStorageType::Empty;
            elem.typeinfo     = nullptr;
        }
    };


  private:
#if __SSO
    union
    {
        void *payload;
        char  small_size_object[1024]; // SSO
    };
#else
    void *payload;
#endif
    Type const  *typeinfo;
    EStorageType storage_type = EStorageType::Empty;
    Operations   operations;

  public:
    template <typename T>
    const Type *GetTypeInfo();
    template <class T>
    static auto make_copy(const T &other);

    template <class T>
    static auto make_steal(T &&other);

    template <class T>
    static auto make_ref(T &other);

    template <class T>
    static auto make_const_ref(const T &other);

    Any() = default;

    // Copy constructor
    Any(const Any &other)
    {
        typeinfo     = other.typeinfo;
        storage_type = other.storage_type;
        operations   = other.operations;
        if (operations.copy) {
            // payload = operations.copy(other).payload;
            auto new_any               = operations.copy(other);
            payload                    = new_any.payload;
            new_any.payload            = nullptr; //
            new_any.operations.release = nullptr;
        }
        else {
            storage_type = EStorageType::Empty;
            typeinfo     = nullptr;
        }
    }

    // Move constructor
    Any(Any &&other) // noexcept
    {
        typeinfo     = std::move(other.typeinfo);
        payload      = std::move(other.payload);
        storage_type = std::move(other.storage_type);
        operations   = std::move(other.operations);
    }

    // Copy assignment
    Any &operator=(const Any &other)
    {
        if (this != &other) {
            typeinfo     = other.typeinfo;
            storage_type = other.storage_type;
            operations   = other.operations;

            if (operations.copy) {
                auto new_any               = operations.copy(other);
                payload                    = new_any.payload;
                new_any.payload            = nullptr;
                new_any.operations.release = nullptr;
            }
            else {
                storage_type = EStorageType::Empty;
                typeinfo     = nullptr;
            }
        }
        return *this;
    }


    // Move assignment
    Any &operator=(Any &&other) // noexcept
    {
        if (this != &other) {
            typeinfo     = std::move(other.typeinfo);
            payload      = std::move(other.payload);
            storage_type = std::move(other.storage_type);
            operations   = std::move(other.operations);
        }
        return *this;
    }


    ~Any()
    {
        if (operations.release && (storage_type == EStorageType::Owned || storage_type == EStorageType::Stolen)) {
#ifndef NODEBUG
            printf("Any::~Any() deallocate payload, storage_type: %s\n", StorageType2String(storage_type).c_str());
#endif
            operations.release(*this);
        }
        else {
            // #ifndef NODEBUG
            //             printf("Any::~Any() %s -> Do not deallocate\n", StorageType2String(storage_type).c_str());
            // #endif
        }
    }

    // temp
    const void  *GetPayload() const { return payload; }
    const Type  *GetTypeInfo() const { return typeinfo; }
    EStorageType GetStorageType() const { return storage_type; }

    template <class T>
    T *as() const
    {
        if (typeinfo == GetType<T>()) {
            return static_cast<T *>(payload);
        }
        return nullptr;
    }


    static std::string StorageType2String(EStorageType type)
    {
        switch (type) {
        case EStorageType::Empty:
            return "Empty";
        case EStorageType::Owned:
            return "Owned";
        case EStorageType::Stolen:
            return "Stolen";
        case EStorageType::Ref:
            return "Ref";
        case EStorageType::ConstRef:
            return "ConstRef";
            break;
        }
    }


  private:
};



// TODO: little object optimized for int short char long ....
template <class T>
inline auto Any::make_copy(const T &other)
{
    Any ret;
    ret.payload      = new T{other};
    ret.typeinfo     = GetType<T>();
    ret.storage_type = EStorageType::Owned;
    if constexpr (std::is_copy_constructible_v<T>) {
        ret.operations.copy = operation_traits<T>::copy;
    }
    if constexpr (std::is_move_constructible_v<T>) {
        ret.operations.steal = operation_traits<T>::steal;
    }
    if constexpr (std::is_destructible_v<T>) {
        ret.operations.release = operation_traits<T>::release;
    }
    return ret;
}

template <class T>
inline auto Any::make_steal(T &&other)
{
    using t1 = std::remove_cv_t< // remove const and volatile
        std::remove_reference_t<T>>;
    Any ret;
    ret.payload      = new t1{std::move(other)};
    ret.typeinfo     = GetType<t1>();
    ret.storage_type = EStorageType::Owned;
    if constexpr (std::is_copy_constructible_v<t1>) {
        ret.operations.copy = operation_traits<t1>::copy;
    }
    if constexpr (std::is_move_constructible_v<t1>) {
        ret.operations.steal = operation_traits<t1>::steal;
    }
    if constexpr (std::is_destructible_v<t1>) {
        ret.operations.release = operation_traits<t1>::release;
    }
    return ret;
}

template <class T>
inline auto Any::make_ref(T &other)
{
    Any ret;
    ret.payload      = &other; // pointer to this deref value
    ret.typeinfo     = GetType<T>();
    ret.storage_type = EStorageType::Ref;
    if constexpr (std::is_copy_constructible_v<T>) {
        ret.operations.copy = operation_traits<T>::copy;
    }
    if constexpr (std::is_move_constructible_v<T>) {
        ret.operations.steal = operation_traits<T>::steal;
    }
    if constexpr (std::is_destructible_v<T>) {
        ret.operations.release = operation_traits<T>::release;
    }
    return ret;
}

template <class T>
inline auto Any::make_const_ref(const T &other)
{
    Any ret;
    // ret.payload      = static_cast<void *>(&other); // also pointer to this deref value
    ret.payload      = (void *)(&other); // also pointer to this deref value
    ret.typeinfo     = GetType<T>();
    ret.storage_type = EStorageType::ConstRef;
    if constexpr (std::is_copy_constructible_v<T>) {
        ret.operations.copy = operation_traits<T>::copy;
    }
    if constexpr (std::is_move_constructible_v<T>) {
        ret.operations.steal = operation_traits<T>::steal;
    }
    if constexpr (std::is_destructible_v<T>) {
        ret.operations.release = operation_traits<T>::release;
    }
    return ret;
}


class Numeric : public Type
{
  public:
    enum class EKind
    {
        Unknown,
        Int8,
        Int16,
        Int32,
        Int64,
        Int128,
        Float,
        Double,
        Bool
    };

  private:
    EKind kind;
    bool  bSigned;

  public:

    Numeric(EKind kind, bool bSigned) : Type(GetName(kind), Type::EKind::Numeric), kind(kind), bSigned(bSigned) {}

    bool IsKind(EKind kind) const { return this->kind == kind; }
    bool IsSigned() const { return bSigned; }

    template <typename T>
    static Numeric Create()
    {
        return Numeric{DetectKind<T>(), std::is_signed_v<T>};
    }

    static std::string GetName(EKind kind)
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
        case EKind::Int128:
            return "Int128";
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

    void SetValue(double value, Any &elem)
    {
        auto typeinfo = elem.GetTypeInfo();
        if (typeinfo->GetKind() == Type::EKind::Numeric) {
            auto numeric = typeinfo->AsNumeric();
            switch (numeric->kind) {
            case EKind::Unknown:
                assert(false);
                break;
            case EKind::Int8:
            {
                if (numeric->IsSigned()) {
                    *(int8_t *)elem.payload = value;
                }
                else {
                    *(uint8_t *)elem.payload = value;
                }
                break;
            }
            case EKind::Int16:
            {
                if (numeric->IsSigned()) {
                    *(int16_t *)elem.payload = value;
                }
                else {
                    *(uint16_t *)elem.payload = value;
                }
            }
            // TODO....
            case EKind::Int32:
            case EKind::Int64:
            case EKind::Int128:
            case EKind::Float:
            case EKind::Double:
            case EKind::Bool:
                break;
            }
        }
    }

  private:
    template <typename T>
    static EKind DetectKind()
    {
        using t1 = std::remove_reference_t<T>;

        if constexpr (std::is_same_v<t1, int8_t>) {
            return EKind::Int8;
        }
        else if constexpr (std::is_same_v<t1, int16_t>) {
            return EKind::Int16;
        }
        else if constexpr (std::is_same_v<t1, int32_t>) {
            return EKind::Int32;
        }
        else if constexpr (std::is_same_v<t1, int64_t>) {
            return EKind::Int64;
        }
        else if constexpr (std::is_same_v<t1, signed long long>) {
            return EKind::Int128;
        }
        else if constexpr (std::is_same_v<t1, float>) {
            return EKind::Float;
        }
        else if constexpr (std::is_same_v<t1, double>) {
            return EKind::Double;
        }
        else if constexpr (std::is_same_v<t1, bool>) {
            return EKind::Bool;
        }

        // what type is T?
        fprintf(stderr, "Unknown type: %s\n", typeid(T).name());
        assert(false);
        return EKind::Unknown;
    }
};


class Enum : public Type
{

  public:
    struct Item {
        using value_type = int32_t;
        std::string name;
        value_type  value;
    };

  private:
    std::vector<Item> items;

  public:
    Enum() : Type("Unknown-Enum", Type::EKind::Enum) {}
    Enum(const std::string &name) : Type(name, Type::EKind::Enum) {}

    template <typename T>
    void Add(const std::string &name, T value)
    {
        items.emplace_back(Item{name, static_cast<Item::value_type>(value)});
    }

    const std::vector<Item> &GetItems() const { return items; }
};



class Class : public Type
{
  public:

    class Member
    {
        virtual Any call(const std::vector<Any> &args) = 0;
    };

    template <typename OwnerClass, typename VariableStaticType>
    struct MemberVariable : public Member {
        std::string        name;
        const Type        *type;
        VariableStaticType OwnerClass::*ptr;


        Any call(const std::vector<Any> &args) override
        {
            assert(args.size() == 1 && args[0].GetTypeInfo() == GetType<OwnerClass>());
            OwnerClass        *instance = (OwnerClass *)args[0].GetPayload();
            VariableStaticType value    = instance->*ptr;
            return make_copy(value);
        }


        template <typename T>
        static MemberVariable Create(const std::string &name);
    };

    template <typename T, bool IsConst>
    static std::conditional_t<IsConst, const T &, T> unwrap(Any &any)
    {
        if constexpr (IsConst) {
            assert(any.GetTypeInfo() == GetType<T>());
            return *(const T *)any.GetPayload();
        }
        else {
            return *(T *)any.GetPayload();
        }
    }

    template <typename OwnerClass, typename RetType, size_t... Idx, typename... Args>
    static Any inner_call(RetType (OwnerClass::*ptr)(Args...), const std::vector<Any> &args, std::index_sequence<Idx...>)
    {
        OwnerClass *instance = (OwnerClass *)args[0].GetPayload();
        auto        ret      = instance->*ptr(unwrap<Args>(args[Idx + 1])...);
        return make_copy(ret);
    }

    template <typename OwnerClass, typename RetType, typename... Args>
    struct MemberFunction : public Member {
        std::string               name;
        const Type               *return_type;
        std::vector<const Type *> parameter_types;
        RetType (OwnerClass::*ptr)(Args...);

        Any call(const std::vector<Any> &args) override
        {
            assert(args.size() == parameter_types.size() + 1);
            for (size_t i = 0; i < parameter_types.size(); ++i) {
                assert(args[i + 1].GetTypeInfo() == parameter_types[i]);
            }
            return inner_call(ptr, args, std::make_index_sequence<sizeof...(Args)>());
        }

        template <typename T>
        static MemberFunction Create(const std::string &name);

      private:
        template <typename Params, size_t... Idx>
        static std::vector<const Type *> ConvertTypeList2Vector(std::index_sequence<Idx...>);
    };


  private:
    std::vector<Member> functions;
    std::vector<Member> variables;

  public:
    Class() : Type("Unknown-Class", Type::EKind::Class) {}
    Class(const std::string &name) : Type(name, Type::EKind::Class) {}

    void AddVariable(Member &&member) { variables.emplace_back(std::make_unique(std::move(member))); }
    void AddFunction(Member &&member) { functions.emplace_back(std::move(member)); }

    const std::vector<std::unique_ptr<Member>> &GetFunctions() const { return functions; }
    const std::vector<std::unique_ptr<Member>> &GetVariables() const { return variables; }
};



inline const Numeric *Type::AsNumeric() const
{
    if (kind == EKind::Numeric) {
        return static_cast<const Numeric *>(this);
    }
    return nullptr;
}

inline const Enum *Type::AsEnum() const
{
    if (kind == EKind::Enum) {
        return static_cast<const Enum *>(this);
    }
    return nullptr;
}

inline const Class *Type::AsClass() const
{
    if (kind == EKind::Class) {
        return static_cast<const Class *>(this);
    }
    return nullptr;
}


#pragma endregion type



template <class T>
class NumericFactory final
{

    Numeric info;

  public:
    static NumericFactory &Instance()
    {
        static NumericFactory instance{Numeric::Create<T>()};
        static bool           bInitialized = false;
        if (!bInitialized) {
            GlobalTypes().insert(&instance.Info());
            bInitialized = true;
        }
        return instance;
    }

    NumericFactory(Numeric &&info) : info(std::move(info)) {}
    const Numeric &Info() const { return info; }

    // void Unregister()
    // {
    //      TODO
    //     info = Numeric();
    // }
};


template <class T>
class EnumFactory final
{
    Enum info;

  public:
    static EnumFactory &Instance()
    {
        static EnumFactory instance;
        static bool        bInitialized = false;
        if (!bInitialized) {
            GlobalTypes().insert(&instance.Info());
            bInitialized = true;
        }
        return instance;
    }
    const Enum &Info() const { return info; }


    EnumFactory &Register(const std::string &name)
    {
        info.name = name;
        return *this;
    }

    template <typename U>
    EnumFactory &Add(const std::string &name, U value)
    {
        info.Add(name, value);
        return *this;
    }

    void Unregister()
    {
        // TODO
    }
};

template <class T>
class ClassFactory final
{
    Class info;

  public:
    static ClassFactory &Instance()
    {
        static ClassFactory instance;
        static bool         bInitialized = []() {
            GlobalTypes().insert(&instance.Info());
            return true;
        };
        return instance;
    }
    const Class &Info() const { return info; }


    ClassFactory &Register(const std::string &name)
    {
        info.name = name;
        return *this;
    }

    template <typename U>
    ClassFactory &AddVariable(const std::string &name)
    {
        info.AddVariable(Class::MemberVariable::Create<U>(name));
        return *this;
    }

    template <typename U>
    ClassFactory &AddFunction(const std::string &name)
    {
        info.AddFunction(Class::MemberFunction::Create<U>(name));
        return *this;
    }
};



class TrivialFactory
{
  public:
    static TrivialFactory &Instance()
    {
        static TrivialFactory instance;
        return instance;
    }
};



template <typename T>
class Factory final
{
  public:
    static auto &GetFactory()
    {
        using t1 = std::remove_reference_t<T>;

        if constexpr (std::is_fundamental_v<t1>) {
            return NumericFactory<T>::Instance();
        }
        else if constexpr (std::is_enum_v<t1>) {
            return EnumFactory<T>::Instance();
        }
        else if constexpr (std::is_class_v<t1>) {
            return ClassFactory<T>::Instance();
        }
        else {
            return TrivialFactory::Instance();
        }
    }
};


template <typename ValueType>
auto &Register()
{
    return Factory<ValueType>::GetFactory();
}

template <typename T>
const Type *GetType()
{
    return &Factory<T>::GetFactory().Info();
}

inline const Type *GetType(std::string_view name)
{
    for (auto type : GlobalTypes()) {
        if (type->GetName() == name) {
            return type;
        }
    }
    return nullptr;
}



template <typename T>
inline Class::MemberFunction Class::MemberFunction::Create(const std::string &name)
{
    using traits     = function_traits<T>;
    using parameters = typename traits::parameters;
    return MemberFunction{
        name,
        // TODO: void type
        GetType<typename traits::return_type>(),
        ConvertTypeList2Vector<parameters>(
            std::make_index_sequence<std::tuple_size_v<parameters>>()),
    };
}

template <typename T>
inline Class::MemberVariable Class::MemberVariable::Create(const std::string &name)
{
    using type = typename variable_traits<T>::type;
    return MemberVariable{name, GetType<type>()};
}

template <typename Params, size_t... Idx>
inline std::vector<const Type *> Class::MemberFunction::ConvertTypeList2Vector(std::index_sequence<Idx...>)
{
    // clang-format off
    return {
        GetType<
            std::tuple_element_t<Idx, Params>
            >
        () ...};
    // clang-format on
}



// TODO: wrapping in debug preprocessor
#include "ut.hpp"

enum class EMyEnum
{
    V1 = 1,
    V2 = 2,
};

struct MyClass {
    int  v1 = 1;
    int  f1() { return 1; };
    void f2() {}
    int  f3(int &i) { return 2; }
};



constexpr void dynamic_function_test()
{
    Register<EMyEnum>()
        .Register("EMyEnum")
        .Add("V1", EMyEnum::V1)
        .Add("V2", EMyEnum::V2);

    // auto const *typeinfo = GetType<EMyEnum>();
    auto const *typeinfo = GetType("EMyEnum");
    debug(), "1,", typeinfo->GetName();

    auto enum_type = typeinfo->AsEnum();
    debug(), "2,", enum_type->GetName();
    for (auto item : enum_type->GetItems()) {
        debug(), item.name, " = ", item.value;
    }

    Register<MyClass>()
        .Register("MyClass")
        .AddVariable<decltype(&MyClass::v1)>("v1")
        .AddFunction<decltype(&MyClass::f1)>("f1")
        // TODO for void, std::is_fundamental_v<void> return true?
        //  1. add void to numeric type
        // 2. add void type and void_factory
        // .AddFunction<decltype(&MyClass::f2)>("f2")
        .AddFunction<decltype(&MyClass::f3)>("f3");

    // using t = function_traits<decltype(&MyClass::f1)>::return_type;

    // auto type = GetType<MyClass>();
    auto type = GetType("MyClass");
    debug(), "1,", type->GetName();
    auto class_type = type->AsClass();
    debug(), "2,", class_type->GetName();
    for (auto item : class_type->GetVariables()) {
        debug(), item.name;
    }
    for (auto item : class_type->GetFunctions()) {
        auto d = debug();
        d, item.return_type->GetName(), " ";
        d, item.name, " (";
        for (auto param : item.parameters) {
            d, param->GetName(), ",";
        }
        d, ")";
    }
}

struct Person3 {
    std::string last_name;
    float       height;
    // bool        is_transaxle;
    bool is_female;

    Person3() { printf("Person3 default construct\n"); }

    // Copy
    Person3(const Person3 &other) : last_name(other.last_name), height(other.height), is_female(other.is_female)
    {
        printf("Person3 copy\n");
    }
    // Move
    Person3(Person3 &&other) : last_name(std::move(other.last_name)), height(other.height), is_female(other.is_female)
    {
        printf("Person3 move/steal\n");
    }
    // Copy assign
    Person3 &operator=(const Person3 &other)
    {
        last_name = other.last_name;
        height    = other.height;
        is_female = other.is_female;
        printf("Person3 copy assign\n");
        return *this;
    }

    ~Person3()
    {
        printf("Person3 destruct\n");
    }



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
    bool GetMarriedWith(Person3 other)
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


inline void any_test()
{
    Register<Person3>()
        .Register("Person3")
        .AddVariable<decltype(&Person3::height)>("height")
        .AddFunction<decltype(&Person3::GetMarriedWith)>("GetMarriedWith");

    {
        Person3 p;
        p.last_name = "John";
        p.height    = 1.75f;
        p.is_female = true;
        Any a       = Any::make_copy(p);

        // debug(), Any::StorageType2String(a.GetStorageType()), " ", a.GetTypeInfo()->GetName(), " ", (int)a.GetTypeInfo()->GetKind();
        // auto b = a.GetPayload();
        // auto c = (Person3 *)b;
        // debug(), c->last_name, " ", c->height, " ", c->is_female;
    }
    debug(), "------------------";
    {
        Person3 p;
        p.last_name = "John";
        p.height    = 1.75f;
        p.is_female = true;
        Any a       = Any::make_steal(p);

        debug(), Any::StorageType2String(a.GetStorageType()), " ", a.GetTypeInfo()->GetName(), " ", (int)a.GetTypeInfo()->GetKind();
        auto b = a.GetPayload();
        auto c = (Person3 *)b;
        debug(), c->last_name, " ", c->height, " ", c->is_female;
    }
    debug(), "------------------";
    {
        Person3 p;
        p.last_name = "John";
        p.height    = 1.75f;
        p.is_female = true;
        Any a       = Any::make_ref(p);

        debug(), Any::StorageType2String(a.GetStorageType()), " ", a.GetTypeInfo()->GetName(), " ", Type::EKind2String(a.GetTypeInfo()->GetKind());
        auto b = a.GetPayload();
        auto c = (Person3 *)b;
        debug(), c->last_name, " ", c->height, " ", c->is_female;
    }
    debug(), "------------------";
    {
        Person3 p;
        p.last_name = "John";
        p.height    = 1.75f;
        p.is_female = true;
        Any a       = Any::make_const_ref(p);


        debug(), Any::StorageType2String(a.GetStorageType()), " ", a.GetTypeInfo()->GetName(), " ", Type::EKind2String(a.GetTypeInfo()->GetKind());
        auto b = a.GetPayload();
        auto c = (Person3 *)b;
        debug(), c->last_name, " ", c->height, " ", c->is_female;


        auto e = a.as<Person3>();
        assert(e != nullptr);
        debug(), e->last_name, " ", e->height, " ", e->is_female;
    }
}
