#include <doctest/doctest.h>
#include <cloudcompanion/version.h>

#include <string>

TEST_CASE("CloudCompanion version") {
  static_assert(std::string_view(CLOUDCOMPANION_VERSION) == std::string_view("1.0"));
  CHECK(std::string(CLOUDCOMPANION_VERSION) == std::string("1.0"));
}