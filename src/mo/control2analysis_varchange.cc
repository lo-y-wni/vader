/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "atlas/array.h"
#include "atlas/field.h"
#include "atlas/functionspace.h"
#include "atlas/util/Config.h"
#include "mo/constants.h"
#include "mo/control2analysis_varchange.h"
#include "mo/functions.h"

using atlas::array::make_view;
using atlas::util::Config;
using atlas::idx_t;

namespace mo {

bool thetavP2Hexner(atlas::FieldSet & fields) {
  const auto rpView = make_view<const double, 2>(fields["height_levels"]);
  const auto vthetaView = make_view<const double, 2>(fields["virtual_potential_temperature"]);
  const auto pView = make_view<const double, 2>(fields["air_pressure_levels_minus_one"]);
  auto hexnerView = make_view<double, 2>(fields["hydrostatic_exner_levels"]);

  for (idx_t jn = 0; jn < fields["hydrostatic_exner_levels"].shape(0); ++jn) {
    hexnerView(jn, 0) = pow(pView(jn, 0) / constants::p_zero,
      constants::rd_over_cp);
    for (idx_t jl = 1; jl < fields["hydrostatic_exner_levels"].levels(); ++jl) {
      hexnerView(jn, jl) = hexnerView(jn, jl-1) -
        (constants::grav * (rpView(jn, jl) - rpView(jn, jl-1))) /
        (constants::cp * vthetaView(jn, jl));
    }
  }
  return true;
}

void hexner2PThetav(atlas::FieldSet & fields) {
  const auto rpView = make_view<const double, 2>(fields["height_levels"]);
  const auto hexnerView = make_view<const double, 2>(fields["hydrostatic_exner_levels"]);
  auto pView = make_view<double, 2>(fields["air_pressure_levels_minus_one"]);
  auto vthetaView = make_view<double, 2>(fields["virtual_potential_temperature"]);

  for (idx_t jn = 0; jn < fields["hydrostatic_exner_levels"].shape(0); ++jn) {
    pView(jn, 0) = constants::p_zero * pow(hexnerView(jn, 0), (constants::cp / constants::rd));

    for (idx_t jl = 1; jl < fields["hydrostatic_exner_levels"].levels(); ++jl) {
      vthetaView(jn, jl) = -constants::grav * (rpView(jn, jl) - rpView(jn, jl-1)) /
         (constants::cp * (hexnerView(jn, jl) - hexnerView(jn, jl-1)));
    }
    vthetaView(jn, 0) = vthetaView(jn, 1);
  }
}

void evalVirtualPotentialTemperature(atlas::FieldSet & fields) {
  const auto qView = make_view<const double, 2>(fields["specific_humidity"]);
  const auto thetaView = make_view<const double, 2>(fields["potential_temperature"]);
  auto vthetaView = make_view<double, 2>(fields["virtual_potential_temperature"]);

  auto fspace = fields["virtual_potential_temperature"].functionspace();

  auto evaluateVTheta = [&] (idx_t i, idx_t j) {
    vthetaView(i, j) = thetaView(i, j) * (1.0 + constants::c_virtual * qView(i, j)); };

  auto conf = Config("levels", fields["virtual_potential_temperature"].levels()) |
              Config("include_halo", true);

  functions::parallelFor(fspace, evaluateVTheta, conf);
}

/// \details Calculate the dry air density
///          from the air_pressure_levels_minus_one,
///          air_temperature (which needs to be interpolated).
void evalDryAirDensity(atlas::FieldSet & fields) {
  const auto hlView = make_view<const double, 2>(fields["height_levels"]);
  const auto hView = make_view<const double, 2>(fields["height"]);

  const auto tView = make_view<const double, 2>(fields["air_temperature"]);
  const auto pView = make_view<const double, 2>(fields["air_pressure_levels_minus_one"]);
  auto rhoView = make_view<double, 2>(fields["dry_air_density"]);

  for (idx_t jn = 0; jn < fields["dry_air_density"].shape(0); ++jn) {
    rhoView(jn, 0) = pView(jn, 0) / (constants::rd * tView(jn, 0));
    for (idx_t jl = 1; jl < fields["dry_air_density"].levels(); ++jl) {
      rhoView(jn, jl) = pView(jn, jl) * (hView(jn, jl) - hView(jn, jl-1)) /
        (constants::rd * (
        (hView(jn, jl) - hlView(jn, jl)) * tView(jn, jl-1) +
        (hlView(jn, jl) - hView(jn, jl-1)) * tView(jn, jl)));
    }
  }
}

}  // namespace mo
