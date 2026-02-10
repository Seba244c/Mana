#pragma once

#include "mpch.h"

#define COMPONENT_NAME(name)                                                   \
    static constexpr const char *ComponentName() { return #name; }
#define COMPONENT_ENSURE(...)                                                  \
    using EnsuredComponents = Mana::TypesList<__VA_ARGS__>;
