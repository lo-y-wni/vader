/*
 * (C) Copyright 2022 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#define ECKIT_TESTING_SELF_REGISTER_CASES 0

#include "atlas/field/Field.h"
#include "atlas/field/FieldSet.h"
#include "oops/runs/Test.h"
#include "oops/util/Logger.h"
#include "test/TestEnvironment.h"

#include "vader/vader.h"

/// \brief Top-level options taken by the PrintVader test.
class PrintVaderTestParameters : public oops::Parameters {
  OOPS_CONCRETE_PARAMETERS(PrintVaderTestParameters, Parameters)

 public:
  oops::Parameter<vader::VaderParameters> vader{"vader",
        "vader parameters", {}, this};
};

namespace vader {
namespace test {

// -----------------------------------------------------------------------------
/// \brief Tests print method of vader.
void testPrintVader() {
  PrintVaderTestParameters params;
  params.validateAndDeserialize(::test::TestEnvironment::config());

  vader::Vader vader(params.vader);
  oops::Log::info() << vader << std::endl;
}

// -----------------------------------------------------------------------------

class PrintVader : public oops::Test {
 public:
  PrintVader() {}
  virtual ~PrintVader() {}
 private:
  std::string testid() const override {return "vader::test::PrintVader";}

  void register_tests() const override {
    std::vector<eckit::testing::Test>& ts = eckit::testing::specification();

    ts.emplace_back(CASE("vader/PrintVader/testPrintVader")
      { testPrintVader(); });
  }

  void clear() const override {}
};

// -----------------------------------------------------------------------------

}  // namespace test
}  // namespace vader
