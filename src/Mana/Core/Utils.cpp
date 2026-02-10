#include "Utils.h"

namespace Mana {
namespace Utils::String {
std::vector<std::string> splitString(const std::string &str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        result.push_back(token);
    }

    return result;
}
} // namespace Utils::String

namespace Utils::Files {
void WriteSized(std::ostream &out, const std::string &data) {
    // Write first string
    size_t strLength = data.size();
    out.write(reinterpret_cast<const char *>(&strLength), sizeof(strLength));
    out.write(data.c_str(), strLength);
}

std::string ReadNextString(std::istream &in) {
    size_t strLength;
    in.read(reinterpret_cast<char *>(&strLength), sizeof(strLength));
    std::string out(strLength, '\0');
    in.read(&out[0], strLength);

    return out;
}

void WriteSized(std::ostream &out, const std::vector<uint8_t> &data) {
    // Write first binary data
    size_t binLength = data.size();
    out.write(reinterpret_cast<const char *>(&binLength), sizeof(binLength));
    out.write(reinterpret_cast<const char *>(data.data()), binLength);
}

std::vector<uint8_t> ReadNextData(std::istream &in) {
    size_t binLength;
    in.read(reinterpret_cast<char *>(&binLength), sizeof(binLength));
    std::vector<uint8_t> out(binLength);
    in.read(reinterpret_cast<char *>(out.data()), binLength);

    return out;
}

std::filesystem::path FindCorrectPath(uint8_t pcount, const char *paths[]) {
    for (uint8_t i = 0; i < pcount; i++) {
        auto p = paths[i];
        if (std::filesystem::is_directory(p)) {
            return p;
        }
    }

    M_ASSERT(false, "Couldn't find any valid path!");
    return "";
}

TempFile::TempFile(const std::filesystem::path path,
                   const std::vector<uint8_t> &data)
    : m_Path(path) {
    std::ofstream out(path, std::ios::binary);
    out.write(reinterpret_cast<const char *>(data.data()), data.size());
    out.close();
}

TempFile::TempFile(const std::filesystem::path path) : m_Path(path) {}

TempFile::~TempFile() { std::filesystem::remove(m_Path); }

std::vector<uint8_t> loadBinaryFromFile(const std::filesystem::path &path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        MC_ERROR("Failed to read file at: {}", path.c_str());
        return {};
    }

    // Read all data into the vector first, then close the file
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    if (size > 0) {
        file.read(reinterpret_cast<char *>(buffer.data()), size);
    }

    file.close();
    return buffer;
}
} // namespace Utils::Files
} // namespace Mana
