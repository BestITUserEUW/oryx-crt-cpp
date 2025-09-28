#include <oryx/uuid.hpp>

#include <array>
#include <format>
#include <random>
#include <string>
#include <algorithm>

namespace oryx {

auto uuid4::Generate() -> std::string {
    std::random_device rd;
    std::uniform_int_distribution<unsigned int> dis(0, 255);

    // generate 16 random bytes
    std::array<uint8_t, 16> bytes;
    std::ranges::generate(bytes, [&dis, &rd] { return static_cast<uint8_t>(dis(rd)); });

    // set version to 4
    bytes[6] = (bytes[6] & 0x0F) | 0x40;

    // set variant to RFC 4122
    bytes[8] = (bytes[8] & 0x3F) | 0x80;

    return std::format(
        "{0:02x}{1:02x}{2:02x}{3:02x}-"
        "{4:02x}{5:02x}-"
        "{6:02x}{7:02x}-"
        "{8:02x}{9:02x}-"
        "{10:02x}{11:02x}{12:02x}{13:02x}{14:02x}{15:02x}",
        static_cast<int>(bytes[0]), static_cast<int>(bytes[1]), static_cast<int>(bytes[2]), static_cast<int>(bytes[3]),
        static_cast<int>(bytes[4]), static_cast<int>(bytes[5]), static_cast<int>(bytes[6]), static_cast<int>(bytes[7]),
        static_cast<int>(bytes[8]), static_cast<int>(bytes[9]), static_cast<int>(bytes[10]),
        static_cast<int>(bytes[11]), static_cast<int>(bytes[12]), static_cast<int>(bytes[13]),
        static_cast<int>(bytes[14]), static_cast<int>(bytes[15]));
}

}  // namespace oryx