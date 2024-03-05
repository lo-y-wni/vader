/*
 * (C) Crown Copyright 2024 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <array>
#include <cmath>
#include <string>
#include <vector>

#define ECKIT_TESTING_SELF_REGISTER_CASES 0

#include "mo/constants.h"
#include "mo/lookups.h"

#include "oops/runs/Test.h"
#include "oops/util/Logger.h"
#include "test/TestEnvironment.h"

namespace mo {
namespace test {

// Analytic functions
// From UM documentation paper 029, section 6.
// (With a fix for the svpW equation - see comment)

// Triple point of water
static constexpr double tTrip = 273.16;
static constexpr double tFreezing = 273.15;

static constexpr double ln10 = M_LN10;

// Saturated vapour pressure at temperatures above freezing point of water.
// With log derivative.
namespace svpW {
  static constexpr double a1 = 10.79574;
  static constexpr double a2 = -5.028;
  static constexpr double a3 = 1.50475e-4;
  static constexpr double a4 = -8.2969;
  static constexpr double a5 = 0.42873e-3;
  static constexpr double a6 = 4.76955;
  static constexpr double a7 = 2.78614;

  double log10svpW(const double t) {
    return a1 * (1.0 - tTrip/t) + a2 * std::log10(t/tTrip) + a3
        * (1.0 - std::pow(10, (a4 * (t/tTrip - 1.0))))
        // This final - 1.0 here is not in the UM docs, but is needed.
        + a5 * (std::pow(10, (a6 * (1.0 - tTrip/t))) - 1.0) + a7;
  }

  double dlsvpW(const double t) {
    return a1 * ln10 * tTrip/(t * t) + a2/t - a3 * ln10 * ln10
        * std::pow(10, (a4 * (t/tTrip - 1.0))) * a4/tTrip
        + a5 * ln10 * ln10 * std::pow(10, (a6 * (1.0 - tTrip/t))) * a6 * tTrip/(t * t);
  }
}  // namespace svpW

// Saturated vapour pressure below the freezing point of water.
// With log derivative.
namespace svpSubZero {
  static constexpr double a1 = -9.09685;
  static constexpr double a2 = -3.56654;
  static constexpr double a3 = 0.87682;
  static constexpr double a4 = 0.78614 + 2.0;

  double log10svpSubZero(const double t) {
    return a1 * (tTrip/t - 1.0) + a2 * std::log10(tTrip/t) + a3 * (1.0 - t/tTrip) + a4;
  }

  double dlsvpSubZero(const double t) {
    return ln10 * (-a1 * tTrip/(t * t) - (a2/t)/ln10 - a3/tTrip);
  }
}  // namespace svpSubZero

double log10svp(const double t) {
  if (t > tFreezing) {  // If above freezing point, use W function (water).
    return svpW::log10svpW(t);
  } else {
    return svpSubZero::log10svpSubZero(t);
  }
}

double dlsvp(const double t) {
  if (t > tFreezing) {
    return svpW::dlsvpW(t);
  } else {
    return svpSubZero::dlsvpSubZero(t);
  }
}

template<typename AnalyticF>
void testLookupTable(const size_t idx, const double maxRelDiff, const AnalyticF& functor) {
  const auto& lookupTable = mo::lookups::allSvpLookupTable[idx];

  // Make temperature space
  double t = mo::constants::TLoBound;

  for (const auto& element : lookupTable) {
    const double analytical = functor(t);
    const double relDiff = std::abs(element - analytical) / element;

    EXPECT(relDiff < maxRelDiff);
    t += mo::constants::Tinc;
  }
}


// -----------------------------------------------------------------------------
void testSvpLookup() {
  oops::Log::info() << "CheckLookupTables: Testing svp lookup table..." << std::endl;

  testLookupTable(0, 4e-5, [](const double temp) -> double {
    return std::pow(10.0, log10svp(temp));
  });

  oops::Log::info() << "CheckLookupTables: svp lookup table validated." << std::endl;
}

// -----------------------------------------------------------------------------

void testDlsvpLookup() {
  oops::Log::info() << "CheckLookupTables: Testing dlsvp lookup table..." << std::endl;
  // This is a high relative tolerance.
  // There are small differences between the existing lookup table and the analytical functions.
  testLookupTable(1, 2e-2, dlsvp);

  oops::Log::info() << "CheckLookupTables: dlsvp lookup table validated." << std::endl;
}

// -----------------------------------------------------------------------------

void testSvpWLookup() {
  oops::Log::info() << "CheckLookupTables: Testing svpW lookup table..." << std::endl;

  testLookupTable(2, 4e-5, [](const double temp) -> double {
    return std::pow(10.0, svpW::log10svpW(temp));
  });

  oops::Log::info() << "CheckLookupTables: svpW lookup table validated." << std::endl;
}

// -----------------------------------------------------------------------------

void testDlsvpWLookup() {
  oops::Log::info() << "CheckLookupTables: Testing dlsvpW lookup table..." << std::endl;

  testLookupTable(3, 4e-3, svpW::dlsvpW);

  oops::Log::info() << "CheckLookupTables: dlsvpW lookup table validated." << std::endl;
}

// -----------------------------------------------------------------------------

class CheckLookupTables : public oops::Test {
 public:
  CheckLookupTables() {}
  virtual ~CheckLookupTables() {}
 private:
  std::string testid() const override {return "mo::test::CheckLookupTables";}

  void register_tests() const override {
    std::vector<eckit::testing::Test>& ts = eckit::testing::specification();

    ts.emplace_back(CASE("mo/CheckLookupTables/testSvpLookup")
      { testSvpLookup(); });
    ts.emplace_back(CASE("mo/CheckLookupTables/testDlsvpLookup")
      { testDlsvpLookup(); });
    ts.emplace_back(CASE("mo/CheckLookupTables/testSvpWLookup")
      { testSvpWLookup(); });
    ts.emplace_back(CASE("mo/CheckLookupTables/testDlsvpWLookup")
      { testDlsvpWLookup(); });
  }

  void clear() const override {}
};

// -----------------------------------------------------------------------------

}  // namespace test
}  // namespace mo
