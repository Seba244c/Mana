#pragma once
#include <mpch.h>

namespace Mana {
class Crypto {
  public:
    [[nodiscard]] constexpr static uint64_t Hash_fnv1a(const char *str) {
        uint64_t hash = 14695981039346656037ULL; // FNV offset basis
        while (*str) {
            hash ^= static_cast<uint64_t>(*str);
            hash *= 1099511628211ULL; // FNV prime
            ++str;
        }
        return hash;
    }
    [[nodiscard]] consteval static uint64_t HashC_fnv1a(const char *str) {
        uint64_t hash = 14695981039346656037ULL; // FNV offset basis
        while (*str) {
            hash ^= static_cast<uint64_t>(*str);
            hash *= 1099511628211ULL; // FNV prime
            ++str;
        }
        return hash;
    }

    [[nodiscard]] constexpr static uint64_t HashC_fnv1a(std::string_view sv) {
        uint64_t hash = 14695981039346656037ULL; // FNV offset basis
        for (char c : sv) {
            hash ^= static_cast<uint64_t>(c);
            hash *= 1099511628211ULL; // FNV prime
        }
        return hash;
    }

    struct StringHash {
        using is_transparent = void; // enables heterogeneous lookup

        size_t operator()(std::string_view key) const noexcept {
            return std::hash<std::string_view>{}(key);
        }
    };

    struct StringEq {
        using is_transparent = void;

        bool operator()(std::string_view a, std::string_view b) const noexcept {
            return a == b;
        }
    };
};
} // namespace Mana
