#include "doctest.hpp"

#include <oryx/types.hpp>
#include <oryx/argparse.hpp>

using namespace oryx;

static constexpr i32 kArgc = 7;
static constexpr auto kStringArgValue = "some/filepath";
static constexpr const char* kArgv[kArgc] = {"--help", "--string",   kStringArgValue, "--integral",
                                             "25",     "--floating", "3.7689"};

TEST_CASE("Create filled view") {
    auto cli = argparse::CLI(kArgc, kArgv);
    REQUIRE_FALSE(cli.Empty());
    REQUIRE(cli.Size() == kArgc);
}

TEST_CASE("Contains argument valid / invalid") {
    auto cli = argparse::CLI(kArgc, kArgv);
    CHECK(cli.Contains("--help"));
    CHECK_FALSE(cli.Contains("--hel"));
}

TEST_CASE("Get string value") {
    auto cli = argparse::CLI(kArgc, kArgv);
    REQUIRE(cli.Contains("--string"));
    auto result = cli.GetValue<std::string>("--string");
    REQUIRE(result.has_value());
    CHECK_EQ(result.value(), kStringArgValue);
}

TEST_CASE("Get integral value") {
    auto cli = argparse::CLI(kArgc, kArgv);
    REQUIRE(cli.Contains("--integral"));
    auto result = cli.GetValue<i32>("--integral");
    REQUIRE(result.has_value());
    REQUIRE(result.value() == 25);

    REQUIRE_FALSE(cli.GetValue<i32>("--string").has_value());
}

TEST_CASE("Get floating value") {
    auto cli = argparse::CLI(kArgc, kArgv);
    REQUIRE(cli.Contains("--floating"));
    auto result = cli.GetValue<f64>("--floating");
    REQUIRE(result.has_value());
    REQUIRE(result.value() == 3.7689);

    REQUIRE_FALSE(cli.GetValue<f64>("--string").has_value());
}

TEST_CASE("VisitIfContainss calls visitor if arg present and convertible to string") {
    auto cli = argparse::CLI(kArgc, kArgv);
    std::string value;

    cli.VisitIfContains<std::string>("--string", [&](std::string parsed) { value = std::move(parsed); });
    CHECK_EQ(value, kStringArgValue);
}

TEST_CASE("VisitIfContains does not call visitor if arg is not present") {
    auto cli = argparse::CLI(kArgc, kArgv);
    bool called{};
    cli.VisitIfContains<std::string>("--data", [&](std::string) { called = true; });
    CHECK_FALSE(called);
}

TEST_CASE("VisitIfContains does not call visitor if arg present but parse fails") {
    auto cli = argparse::CLI(kArgc, kArgv);
    bool called{};
    cli.VisitIfContains<f32>("--string", [&](f32 parsed) { called = true; });
    CHECK_FALSE(called);
}
