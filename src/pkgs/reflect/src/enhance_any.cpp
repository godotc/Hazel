#include "enhance_any.hpp"

#include "bus.hpp"

namespace TOPLEVEL_NAMESPACE {

namespace Test {
#if WITH_TEST

void any_test()
{

    struct Person3 {
        std::string lastName;
        float       height;
        bool        bFemale;
        bool        bMarried;
        Person3()
        {
            printf("Person3 default construct\n");
        }
        // Copy
        Person3(const Person3 &other)
            : lastName(other.lastName),
              height(other.height),
              bFemale(other.bFemale)
        {
            printf("Person3 copy\n");
        }
        // Move
        Person3(Person3 &&other)
            : lastName(std::move(other.lastName)), height(other.height), bFemale(other.bFemale)
        {
            printf("Person3 move/steal\n");
        }
        // Copy assign
        Person3 &operator=(const Person3 &other)
        {
            lastName = other.lastName;
            height   = other.height;
            bFemale  = other.bFemale;
            printf("Person3 copy assign\n");
            return *this;
        }

        bool getMarriedWith(Person3 &other)
        {
            if (bMarried) {
                return false;
            }
            bMarried = true;
            if (other.bFemale != bFemale) //&& is_transaxle != other.is_transaxle;)
            {
                if (bFemale) {
                    lastName = "Mrs." + other.lastName;
                }
                else {
                    lastName = "Mr." + lastName;
                }
                return true;
            }
            return false;
        }
        void sleep() const { printf("Zzzzz....\n"); }
    };



    using Any = EnhanceAny;


    registerType<Person3>()
        .registerName("Person3")
        .addVariable<decltype(&Person3::height)>("height")
        .addFunction<decltype(&Person3::getMarriedWith)>("getMarriedWith")
        .addFunction<decltype(&Person3::sleep)>("sleep");

    {
        Person3 p;
        p.lastName = "John";
        p.height   = 1.75f;
        p.bFemale  = true;


        Any a = Any::make_copy(p);

        debug(),
            Any::storageTypeToString(a.getStorageType()),
            " ", a.getTypeInfo()->getName(),
            " ", (int)a.getTypeInfo()->getKind();

        auto b = a.getPayload();
        auto c = (Person3 *)b;
        debug(), c->lastName, " ", c->height, " ", c->bFemale;
    }
    debug(), "------------------";
    {
        Person3 p;
        p.lastName = "John";
        p.height   = 1.75f;
        p.bFemale  = true;
        Any a      = Any::make_steal(p);

        debug(), Any::storageTypeToString(a.getStorageType()), " ", a.getTypeInfo()->getName(), " ", (int)a.getTypeInfo()->getKind();
        auto b = a.getPayload();
        auto c = (Person3 *)b;
        debug(), c->lastName, " ", c->height, " ", c->bFemale;
    }
    debug(), "------------------";
    {
        Person3 p;
        p.lastName = "John";
        p.height   = 1.75f;
        p.bFemale  = true;
        Any a      = Any::make_ref(p);

        debug(), Any::storageTypeToString(a.getStorageType()), " ", a.getTypeInfo()->getName(), " ", Type::kindToString(a.getTypeInfo()->getKind());
        auto b = a.getPayload();
        auto c = (Person3 *)b;
        debug(), c->lastName, " ", c->height, " ", c->bFemale;
    }
    debug(), "------------------";
    {
        Person3 p;
        p.lastName = "John";
        p.height   = 1.75f;
        p.bFemale  = true;
        Any a      = Any::make_const_ref(p);


        debug(), Any::storageTypeToString(a.getStorageType()), " ", a.getTypeInfo()->getName(), " ", Type::kindToString(a.getTypeInfo()->getKind());
        auto b = a.getPayload();
        auto c = (Person3 *)b;
        debug(), c->lastName, " ", c->height, " ", c->bFemale;


        auto e = a.as<Person3>();
        assert(e != nullptr);
        debug(), e->lastName, " ", e->height, " ", e->bFemale;
    }
}

#endif
} // namespace Test

} // namespace TOPLEVEL_NAMESPACE