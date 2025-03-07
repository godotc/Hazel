
#pragma once

#include "base.h"
#include "dynamic.hpp"



namespace TOPLEVEL_NAMESPACE {


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


} // namespace TOPLEVEL_NAMESPACE