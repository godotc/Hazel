#pragma once

#include <functional>
#include <string_view>

#include "Base.h"


#define __SSO 1

TOPLEVEL_NAMESPACE_BEGIN


class Type;


template <typename T>
const Type *getType();


class DynamicAny
{
  public:
    template <class T>
    friend class any_operations_traits;


    enum class EStorageType
    {
        Empty,
        Owned,
        Stolen,
        Ref,
        ConstRef,

    };

    struct Operations {
        std::function<DynamicAny(const DynamicAny &)> copy;
        std::function<DynamicAny(DynamicAny &)>       steal;
        std::function<void(DynamicAny &)>             release;
    };


  private:

    using Any = DynamicAny;

    template <class T>
    struct operation_traits {

        static DynamicAny copy(const DynamicAny &other)
        {
            assert(other.typeinfo == getType<T>());

            DynamicAny ret;
            ret.payload     = new T{*static_cast<T *>(other.payload)}; // erase type
            ret.typeinfo    = other.typeinfo;
            ret.storageType = DynamicAny::EStorageType::Owned;
            ret.operations  = other.operations;
            return ret;
        }

        static DynamicAny steal(DynamicAny &other)
        {
            assert(other.typeinfo == getType<T>());
            DynamicAny ret;

#if __SSO
            if constexpr (sizeof(T) <= sizeof(DynamicAny::smallSizeObject)) {
                // Emplacement New:  call the construct on a memory that already allocated which on stack
                new (ret.smallSizeObject) T{*static_cast<T *>(other.payload)};
            }
            else {
                ret.payload = new T{*static_cast<T *>(other.payload)};
            }
#else
            ret.payload = new T{std::move(*static_cast<T *>(other.payload))}; // erase type
#endif
            ret.typeinfo      = other.typeinfo;
            ret.storageType   = DynamicAny::EStorageType::Owned;
            other.storageType = DynamicAny::EStorageType::Stolen; // update origin's data state
            ret.operations    = other.operations;
            return ret;
        }


        static void release(DynamicAny &any)
        {
            assert(any.typeinfo == getType<T>());

            delete static_cast<T *>(any.payload); // TODO: maybe force convert (void*)
            any.payload     = nullptr;
            any.storageType = DynamicAny::EStorageType::Empty;
            any.typeinfo    = nullptr;
        }
    };



  private:
#if __SSO
    union
    {
        void *payload;
        char  smallSizeObject[1024]; // SSO
    };
#else
    void *payload;
#endif
    Type const  *typeinfo;
    EStorageType storageType = EStorageType::Empty;
    Operations   operations;

  public:
    template <class T>
    static auto make_copy(const T &other)
    {
        Any ret;
        ret.payload     = new T{other};
        ret.typeinfo    = getType<T>();
        ret.storageType = EStorageType::Owned;
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
    static auto make_steal(T &&other)
    {
        UNIMPLEMENTED();
        return Any();
    }

    template <class T>
    static auto make_ref(T &other)
    {
        UNIMPLEMENTED();
        return Any();
    }

    template <class T>
    static auto make_const_ref(const T &other)
    {
        Any ret;
        // ret.payload      = static_cast<void *>(&other); // also pointer to this deref value
        ret.payload     = (void *)(&other); // also pointer to this deref value
        ret.typeinfo    = getType<T>();
        ret.storageType = EStorageType::ConstRef;
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

    DynamicAny() = default;

    // Copy constructor
    DynamicAny(const DynamicAny &other)
    {
        typeinfo    = other.typeinfo;
        storageType = other.storageType;
        operations  = other.operations;
        if (operations.copy) {
            // payload = operations.copy(other).payload;
            auto new_any               = operations.copy(other);
            payload                    = new_any.payload;
            new_any.payload            = nullptr; //
            new_any.operations.release = nullptr;
        }
        else {
            storageType = EStorageType::Empty;
            typeinfo    = nullptr;
        }
    }

    // Move constructor
    DynamicAny(DynamicAny &&other) // noexcept
    {
        typeinfo    = std::move(other.typeinfo);
        payload     = std::move(other.payload);
        storageType = std::move(other.storageType);
        operations  = std::move(other.operations);
    }

    // Copy assignment
    DynamicAny &operator=(const DynamicAny &other)
    {
        if (this != &other) {
            typeinfo    = other.typeinfo;
            storageType = other.storageType;
            operations  = other.operations;

            if (operations.copy) {
                auto new_any               = operations.copy(other);
                payload                    = new_any.payload;
                new_any.payload            = nullptr;
                new_any.operations.release = nullptr;
            }
            else {
                storageType = EStorageType::Empty;
                typeinfo    = nullptr;
            }
        }
        return *this;
    }


    // Move assignment
    DynamicAny &operator=(DynamicAny &&other) // noexcept
    {
        if (this != &other) {
            typeinfo    = std::move(other.typeinfo);
            payload     = std::move(other.payload);
            storageType = std::move(other.storageType);
            operations  = std::move(other.operations);
        }
        return *this;
    }


    ~DynamicAny()
    {
        if (operations.release && (storageType == EStorageType::Owned || storageType == EStorageType::Stolen)) {
#ifndef NODEBUG
            printf("Any::~Any() deallocate payload, storage_type: %s\n", storageTypeToString(storageType).data());
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
    const void  *getPayload() const { return payload; }
    const Type  *getTypeInfo() const { return typeinfo; }
    EStorageType getStorageType() const { return storageType; }

    template <class T>
    T *as() const
    {
        if (typeinfo == getType<T>()) {
            return static_cast<T *>(payload);
        }
        return nullptr;
    }


    static std::string_view storageTypeToString(EStorageType type)
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

        assert(false);
        return "";
    }

  private:
};


namespace Test {
#if WITH_TEST



extern void any_test();
#endif
} // namespace Test

TOPLEVEL_NAMESPACE_END
