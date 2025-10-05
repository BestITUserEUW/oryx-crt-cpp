#include "doctest.hpp"

#include <oryx/crt/error_group.hpp>

using namespace oryx::crt;

TEST_CASE("ErrorGroup construction") {
    SUBCASE("Default construction") {
        ErrorGroup group;
        CHECK(group.Empty());
        CHECK(group.Size() == 0);
        CHECK(group.Errors().empty());
    }

    SUBCASE("Initializer list construction") {
        ErrorGroup group{Error("Error 1"), Error("Error 2")};
        CHECK(group.Size() == 2);
        CHECK_FALSE(group.Empty());
        CHECK(group.Errors()[0].what() == "Error 1");
        CHECK(group.Errors()[1].what() == "Error 2");
    }

    SUBCASE("Empty initializer list") {
        ErrorGroup group{};
        CHECK(group.Empty());
        CHECK(group.Size() == 0);
    }
}

TEST_CASE("ErrorGroup Add method") {
    ErrorGroup group;

    SUBCASE("Add single error") {
        group.Add(Error("First error"));
        CHECK(group.Size() == 1);
        CHECK(group.Errors()[0].what() == "First error");
    }

    SUBCASE("Add multiple errors") {
        group.Add(Error("Error 1"));
        group.Add(Error("Error 2"));
        group.Add(Error("Error 3"));

        CHECK(group.Size() == 3);
        CHECK(group.Errors()[0].what() == "Error 1");
        CHECK(group.Errors()[1].what() == "Error 2");
        CHECK(group.Errors()[2].what() == "Error 3");
    }

    SUBCASE("Add with move semantics") {
        Error error("Movable error");
        group.Add(std::move(error));
        CHECK(group.Size() == 1);
        CHECK(group.Errors()[0].what() == "Movable error");
    }
}

TEST_CASE("ErrorGroup what() method") {
    SUBCASE("Empty group") {
        ErrorGroup group;
        std::string expected = "ErrorGroup with 0 error(s):\n";
        CHECK(group.what() == expected);
    }

    SUBCASE("Single error") {
        ErrorGroup group;
        group.Add(Error("Single error occurred"));

        std::string result = group.what();
        CHECK(result.find("ErrorGroup with 1 error(s):") != std::string::npos);
        CHECK(result.find("[0] Single error occurred") != std::string::npos);
    }

    SUBCASE("Multiple errors") {
        ErrorGroup group{Error("First error"), Error("Second error")};

        std::string result = group.what();
        CHECK(result.find("ErrorGroup with 2 error(s):") != std::string::npos);
        CHECK(result.find("[0] First error") != std::string::npos);
        CHECK(result.find("[1] Second error") != std::string::npos);
    }

    SUBCASE("Error ordering in what() output") {
        ErrorGroup group;
        group.Add(Error("Error A"));
        group.Add(Error("Error B"));
        group.Add(Error("Error C"));

        std::string result = group.what();
        size_t pos_a = result.find("[0] Error A");
        size_t pos_b = result.find("[1] Error B");
        size_t pos_c = result.find("[2] Error C");

        CHECK(pos_a != std::string::npos);
        CHECK(pos_b != std::string::npos);
        CHECK(pos_c != std::string::npos);
        CHECK(pos_a < pos_b);
        CHECK(pos_b < pos_c);
    }
}

TEST_CASE("ErrorGroup ToError() method") {
    SUBCASE("Empty group") {
        ErrorGroup group;
        Error error = group.ToError();

        std::string expected_what = "ErrorGroup with 0 error(s):\n";
        CHECK(error.what() == expected_what);
    }

    SUBCASE("Non-empty group") {
        ErrorGroup group{Error("Test error 1"), Error("Test error 2")};
        Error error = group.ToError();

        std::string_view result = error.what();
        CHECK(result.find("ErrorGroup with 2 error(s):") != std::string::npos);
        CHECK(result.find("[0] Test error 1") != std::string::npos);
        CHECK(result.find("[1] Test error 2") != std::string::npos);
    }

    SUBCASE("ToError() preserves all information") {
        ErrorGroup group;
        group.Add(Error("Detailed error message with special chars: !@#$%"));

        Error error = group.ToError();
        std::string_view result = error.what();

        CHECK(result.find("Detailed error message with special chars: !@#$%") != std::string::npos);
    }
}

TEST_CASE("ErrorGroup copy and move semantics") {
    SUBCASE("Copy construction") {
        ErrorGroup original{Error("Error 1"), Error("Error 2")};
        ErrorGroup copy(original);

        CHECK(original.Size() == 2);
        CHECK(copy.Size() == 2);
        CHECK(copy.Errors()[0].what() == "Error 1");
        CHECK(copy.Errors()[1].what() == "Error 2");
    }

    SUBCASE("Copy assignment") {
        ErrorGroup original{Error("Error 1")};
        ErrorGroup copy;
        copy = original;

        CHECK(original.Size() == 1);
        CHECK(copy.Size() == 1);
        CHECK(copy.Errors()[0].what() == "Error 1");
    }

    SUBCASE("Move construction") {
        ErrorGroup original{Error("Error 1"), Error("Error 2")};
        ErrorGroup moved(std::move(original));

        CHECK(moved.Size() == 2);
        CHECK(moved.Errors()[0].what() == "Error 1");
        CHECK(moved.Errors()[1].what() == "Error 2");
        // original might be empty after move, but that's implementation-defined
    }

    SUBCASE("Move assignment") {
        ErrorGroup original{Error("Error 1")};
        ErrorGroup moved;
        moved = std::move(original);

        CHECK(moved.Size() == 1);
        CHECK(moved.Errors()[0].what() == "Error 1");
        // original might be empty after move, but that's implementation-defined
    }
}

TEST_CASE("ErrorGroup method chaining") {
    SUBCASE("Add method returns void, but can be used in sequence") {
        ErrorGroup group;
        group.Add(Error("Error 1"));
        group.Add(Error("Error 2"));
        group.Add(Error("Error 3"));

        CHECK(group.Size() == 3);
    }
}