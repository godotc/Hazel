#include "dynamic.hpp"
#include "dynamic_any.hpp"

TOPLEVEL_NAMESPACE_BEGIN



std::set<const Type *> &getGlobalTypes()
{
    static std::set<const Type *> global_types;
    return global_types;
}


const Numeric *Type::asNumeric() const
{
    if (kind == EKind::Numeric) {
        return static_cast<const class Numeric *>(this);
    }
    return nullptr;
}

namespace Test {

#if WITH_TEST
void typeTest()
{
    Type  t("test", Type::EKind::Numeric);
    auto &global_types = getGlobalTypes();
    global_types.insert(&t);
}
void classTest()
{
    struct A {
        int  b;
        void c();
        int  d(int b);
        int  e(int b) const;
    };

    Class B("A");


    registerType<A>()
        .registerName("A");
}
#endif

} // namespace Test



TOPLEVEL_NAMESPACE_END