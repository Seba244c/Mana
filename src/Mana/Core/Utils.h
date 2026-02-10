#pragma once
#include <mpch.h>

#include <fmt/chrono.h>
#include <istream>
#include <ostream>

namespace Mana {
namespace Utils::Files {
class BufferWriter {
  public:
    explicit BufferWriter(std::vector<uint8_t> &buffer) : m_Buffer(buffer) {}

    template <typename T> void Write(const T &value) {
        const auto *ptr = reinterpret_cast<const uint8_t *>(&value);
        m_Buffer.insert(m_Buffer.end(), ptr, ptr + sizeof(T));
    }

    void WriteSized(const std::string &str) {
        const size_t len = str.size();
        Write(len); // Write the length first
        const auto *ptr = reinterpret_cast<const uint8_t *>(str.data());
        m_Buffer.insert(m_Buffer.end(), ptr, ptr + len);
    }

  private:
    std::vector<uint8_t> &m_Buffer;
};

class BufferReader {
  public:
    // Constructor from vector
    explicit BufferReader(const std::vector<uint8_t> &buffer)
        : m_Buffer(buffer.data(), buffer.size()), m_Pos(0) {}

    // Constructor from span
    explicit BufferReader(const std::span<const uint8_t> buffer)
        : m_Buffer(buffer), m_Pos(0) {}

    template <typename T> T Read() {
        if (m_Pos + sizeof(T) > m_Buffer.size()) {
            throw std::runtime_error("Buffer underflow");
        }
        T value;
        std::memcpy(&value, m_Buffer.data() + m_Pos, sizeof(T));
        m_Pos += sizeof(T);
        return value;
    }

    std::string ReadNextString() {
        const auto len = Read<size_t>();
        if (m_Pos + len > m_Buffer.size()) {
            throw std::runtime_error("Buffer underflow in string");
        }
        std::string str(reinterpret_cast<const char *>(m_Buffer.data() + m_Pos),
                        len);
        m_Pos += len;
        return str;
    }

  private:
    const std::span<const uint8_t> m_Buffer;
    size_t m_Pos;
};

std::vector<uint8_t> loadBinaryFromFile(const std::filesystem::path &path);

template <typename T> static void Write(std::ostream &out, const T &data) {
    out.write(reinterpret_cast<const char *>(&data), sizeof(data));
}

template <typename T> static void Read(std::istream &in, T &data) {
    in.read(reinterpret_cast<char *>(&data), sizeof(data));
}

static void WriteSized(std::ostream &out, const std::string &data);
static std::string ReadNextString(std::istream &in);
static void WriteSized(std::ostream &out, const std::vector<uint8_t> &data);
static std::vector<uint8_t> ReadNextData(std::istream &in);
static std::filesystem::path FindCorrectPath(uint8_t pcount,
                                             const char *paths[]);

class TempFile {
  public:
    TempFile(const std::filesystem::path path,
             const std::vector<uint8_t> &data);
    TempFile(const std::filesystem::path path);
    ~TempFile();

    const std::filesystem::path &Path() { return m_Path; }

  private:
    const std::filesystem::path m_Path;
};
} // namespace Utils::Files
namespace Utils::String {
std::vector<std::string> splitString(const std::string &str, char delimiter);
}

namespace Utils::Time {
template <typename Clock = std::chrono::system_clock>
std::chrono::time_point<Clock, std::chrono::seconds>
TimeFromString(const std::string &timeStr) {
// Try to use C++20 parsing if available
#if defined(__cpp_lib_chrono) && __cpp_lib_chrono >= 201907L &&                \
    defined(__cpp_lib_format) && __cpp_lib_format >= 202106L
    try {
        std::istringstream ss(timeStr);
        std::chrono::sys_time<std::chrono::seconds> tp;
        std::chrono::from_stream(ss, "%Y-%m-%d %H:%M:%S", tp);

        // Convert to desired clock
        if constexpr (std::is_same_v<Clock, std::chrono::system_clock>) {
            return tp;
        } else {
// If clock_cast is available
#if defined(__cpp_lib_chrono_clock_casting) &&                                 \
    __cpp_lib_chrono_clock_casting >= 202304L
            return std::chrono::clock_cast<Clock>(tp);
#else
            // Manual conversion
            auto sys_now = std::chrono::system_clock::now();
            auto clock_now = Clock::now();
            auto offset = tp - sys_now;
            return clock_now + offset;
#endif
        }
    } catch (...) {
        // If C++20 parsing fails, fall back to manual parsing
    }
#endif

    // Fall back to pre-C++20 parsing
    std::tm tm = {};
    std::istringstream ss(timeStr);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

    M_ASSERT(!ss.fail());
    if (ss.fail()) {
        throw std::runtime_error("Failed to parse time string: " + timeStr);
    }

    // Convert tm to time_t
    std::time_t time = std::mktime(&tm);

    // Convert time_t to system_clock::time_point with seconds precision
    auto sys_tp = std::chrono::time_point_cast<std::chrono::seconds>(
        std::chrono::system_clock::from_time_t(time));

    // Convert to desired clock
    if constexpr (std::is_same_v<Clock, std::chrono::system_clock>) {
        return sys_tp;
    } else {
        auto sys_now = std::chrono::system_clock::now();
        auto clock_now = Clock::now();
        auto offset = sys_tp - sys_now;
        return clock_now + offset;
    }
}

template <typename Clock, typename Duration = typename Clock::duration>
std::string TimeToString(const std::chrono::time_point<Clock, Duration> &tp) {
    // Convert to system_clock time point
    std::chrono::time_point<std::chrono::system_clock> sys_time;

    if constexpr (std::is_same_v<Clock, std::chrono::system_clock>) {
        sys_time = std::chrono::time_point_cast<std::chrono::seconds>(tp);
    } else {
        // Manual conversion between clock domains
        auto clock_now = Clock::now();
        auto sys_now = std::chrono::system_clock::now();
        auto duration_since_clock_epoch = tp - clock_now;
        sys_time = std::chrono::time_point_cast<std::chrono::seconds>(
            sys_now + duration_since_clock_epoch);
    }

    return fmt::format("{:%Y-%m-%d %H:%M:%S}", fmt::gmtime(sys_time));
}
} // namespace Utils::Time

} // namespace Mana
