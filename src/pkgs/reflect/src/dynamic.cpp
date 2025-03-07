#include "dynamic.hpp"
#include "enhance_any.hpp"

namespace TOPLEVEL_NAMESPACE {


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

std::string_view Type::kindToString(EKind kind)
{
    switch (kind) {
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


namespace Test {
#if WITH_TEST


#endif
} // namespace Test


} // namespace TOPLEVEL_NAMESPACE