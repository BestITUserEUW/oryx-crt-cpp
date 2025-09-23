#include "doctest.hpp"

#include <filesystem>

#include <oryx/unique_file_ptr.hpp>

using namespace oryx;

namespace {

struct TempFile {
    TempFile()
        : file(std::filesystem::temp_directory_path()) {
        file.append("tmp.txt");
    }

    ~TempFile() { std::filesystem::remove_all(file); }

    auto ToString() { return file.string(); }

    std::filesystem::path file;
};

}  // namespace

TEST_CASE("Open file works") {
    TempFile tmp_file{};
    const auto file_name = tmp_file.ToString();

    std::string_view data{"Hello World"};

    {
        auto file_ptr = OpenFile(file_name.c_str(), "wr");
        CHECK(file_ptr);
        int bytes_written = fwrite(data.data(), sizeof(std::string_view::value_type), data.size(), file_ptr.get());
        CHECK_EQ(data.size(), bytes_written);
    }

    {
        auto file_ptr = OpenFile(file_name.c_str(), "r");
        CHECK(file_ptr);
        char buffer[12];
        int bytes_read = fread(buffer, sizeof(char), 12, file_ptr.get());
        CHECK_EQ(data.size(), bytes_read);
        std::string_view read_data{buffer};
        CHECK_EQ(read_data, data);
    }
}