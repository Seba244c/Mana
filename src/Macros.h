#pragma once
#ifdef M_TRACE
#error macros.h should not have anything else defined
#endif
#define M_TRACE(...)                                                           \
    (::Mana::Log::GetLogger())                                                 \
        ->log(spdlog::source_loc{__FILE__, __LINE__,                           \
                                 static_cast<const char *>(__FUNCTION__)},     \
              spdlog::level::trace, __VA_ARGS__);
#define M_INFO(...)                                                            \
    (::Mana::Log::GetLogger())                                                 \
        ->log(spdlog::source_loc{__FILE__, __LINE__,                           \
                                 static_cast<const char *>(__FUNCTION__)},     \
              spdlog::level::info, __VA_ARGS__);
#define M_WARN(...)                                                            \
    (::Mana::Log::GetLogger())                                                 \
        ->log(spdlog::source_loc{__FILE__, __LINE__,                           \
                                 static_cast<const char *>(__FUNCTION__)},     \
              spdlog::level::warn, __VA_ARGS__);
#define M_ERROR(...)                                                           \
    (::Mana::Log::GetLogger())                                                 \
        ->log(spdlog::source_loc{__FILE__, __LINE__,                           \
                                 static_cast<const char *>(__FUNCTION__)},     \
              spdlog::level::error, __VA_ARGS__);
#define M_CRITICAL(...)                                                        \
    (::Mana::Log::GetLogger())                                                 \
        ->log(spdlog::source_loc{__FILE__, __LINE__,                           \
                                 static_cast<const char *>(__FUNCTION__)},     \
              spdlog::level::critical, __VA_ARGS__);
#define COMPONENT_NAME(name)                                                   \
    static constexpr const char *ComponentName() { return #name; }
#define COMPONENT_ENSURE(...)                                                  \
    using EnsuredComponents = Mana::TypesList<__VA_ARGS__>;
#define M_ASSETNAME(name) ::Mana::Crypto::HashC_fnv1a(name)
#define M_ASSET(type, name)                                                    \
    ::Mana::Application::Instance()->GetAssets().GetAsset<type>(               \
        M_ASSETNAME(name))
