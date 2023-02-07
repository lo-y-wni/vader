/*
 * (C) Crown Copyright 2023 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "atlas/array/MakeView.h"
#include "atlas/field/FieldSet.h"

#include "mo/constants.h"
#include "mo/eval_dry_air_density.h"

#include "oops/util/Logger.h"

using atlas::array::make_view;
using atlas::idx_t;

namespace mo {

/// \details Calculate the hydrostatic pressure (on levels)
///          from hydrostatic exner.
void eval_dry_air_density_nl(atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_dry_air_density_nl()] starting ..." << std::endl;
  const auto hlView = make_view<const double, 2>(stateFlds["height_levels"]);
  const auto hView = make_view<const double, 2>(stateFlds["height"]);
  const auto tView = make_view<const double, 2>(stateFlds["air_temperature"]);
  const auto pView = make_view<const double, 2>(stateFlds["air_pressure_levels_minus_one"]);
  auto rhoView = make_view<double, 2>(stateFlds["dry_air_density_levels_minus_one"]);

  for (idx_t jn = 0; jn < stateFlds["dry_air_density_levels_minus_one"].shape(0); ++jn) {
    rhoView(jn, 0) = pView(jn, 0) / (constants::rd * tView(jn, 0));
    for (idx_t jl = 1; jl < stateFlds["dry_air_density_levels_minus_one"].levels(); ++jl) {
      rhoView(jn, jl) = pView(jn, jl) * (hView(jn, jl) - hView(jn, jl-1)) /
        (constants::rd * (
        (hView(jn, jl) - hlView(jn, jl)) * tView(jn, jl-1) +
        (hlView(jn, jl) - hView(jn, jl-1)) * tView(jn, jl)));
    }
  }
  oops::Log::trace() << "[eval_dry_air_density_nl()] ... exit" << std::endl;
}

void eval_dry_air_density_tl(atlas::FieldSet & incFlds, const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_dry_air_density_tl()] starting ..." << std::endl;
  const auto hlView = make_view<const double, 2>(stateFlds["height_levels"]);
  const auto hView = make_view<const double, 2>(stateFlds["height"]);
  const auto exnerView = make_view<const double, 2>(stateFlds["exner_levels_minus_one"]);
  const auto thetaView = make_view<const double, 2>(stateFlds["potential_temperature"]);
  const auto rhoView = make_view<const double, 2>(stateFlds["dry_air_density_levels_minus_one"]);
  const auto exnerIncView = make_view<const double, 2>(incFlds["exner_levels_minus_one"]);
  const auto thetaIncView = make_view<const double, 2>(incFlds["potential_temperature"]);
  auto rhoIncView = make_view<double, 2>(incFlds["dry_air_density_levels_minus_one"]);

  for (idx_t jn = 0; jn < rhoIncView.shape(0); ++jn) {
    for (idx_t jl = 1; jl < incFlds["dry_air_density_levels_minus_one"].levels(); ++jl) {
      rhoIncView(jn, jl) = rhoView(jn, jl) * (
        exnerIncView(jn, jl) / exnerView(jn, jl) -
        (((hlView(jn, jl) - hView(jn, jl-1)) * thetaIncView(jn, jl) +
          (hView(jn, jl) - hlView(jn, jl)) * thetaIncView(jn, jl-1)) /
         ((hlView(jn, jl) - hView(jn, jl-1)) * thetaView(jn, jl) +
          (hView(jn, jl) - hlView(jn, jl)) * thetaView(jn, jl-1))));
    }

    rhoIncView(jn, 0) = rhoView(jn, 0) * (
        exnerIncView(jn, 0) / exnerView(jn, 0) -
        thetaIncView(jn, 0)/ thetaView(jn, 0));
  }
  oops::Log::trace() << "[eval_dry_air_density_tl()] ... exit" << std::endl;
}

void eval_dry_air_density_ad(atlas::FieldSet & hatFlds, const atlas::FieldSet & stateFlds) {
  oops::Log::trace() << "[eval_dry_air_density_ad()] starting ..." << std::endl;
  const auto hlView = make_view<const double, 2>(stateFlds["height_levels"]);
  const auto hView = make_view<const double, 2>(stateFlds["height"]);
  const auto exnerView = make_view<const double, 2>(stateFlds["exner_levels_minus_one"]);
  const auto thetaView = make_view<const double, 2>(stateFlds["potential_temperature"]);
  const auto rhoView = make_view<const double, 2>(stateFlds["dry_air_density_levels_minus_one"]);
  auto exnerHatView = make_view<double, 2>(hatFlds["exner_levels_minus_one"]);
  auto thetaHatView = make_view<double, 2>(hatFlds["potential_temperature"]);
  auto rhoHatView = make_view<double, 2>(hatFlds["dry_air_density_levels_minus_one"]);

  for (idx_t jn = 0; jn < rhoHatView.shape(0); ++jn) {
    exnerHatView(jn, 0) += rhoView(jn, 0) * rhoHatView(jn, 0) /
      exnerView(jn, 0);
    thetaHatView(jn, 0) -= rhoView(jn, 0) * rhoHatView(jn, 0) /
      thetaView(jn, 0);
    rhoHatView(jn, 0) = 0.0;

    for (idx_t jl = hatFlds["dry_air_density_levels_minus_one"].levels()-1; jl >= 1; --jl) {
      exnerHatView(jn, jl) += rhoView(jn, jl) * rhoHatView(jn, jl) /
        exnerView(jn, jl);
      thetaHatView(jn, jl) -= rhoView(jn, jl) * rhoHatView(jn, jl) *
        (hlView(jn, jl) - hView(jn, jl-1)) /
        ((hlView(jn, jl) - hView(jn, jl-1)) * thetaView(jn, jl) +
        (hView(jn, jl) - hlView(jn, jl)) * thetaView(jn, jl-1) );
      thetaHatView(jn, jl-1) -= rhoView(jn, jl) * rhoHatView(jn, jl) *
        (hView(jn, jl) - hlView(jn, jl)) /
        ((hlView(jn, jl) - hView(jn, jl-1)) * thetaView(jn, jl) +
        (hView(jn, jl) - hlView(jn, jl)) * thetaView(jn, jl-1));
      rhoHatView(jn, jl) = 0.0;
    }
  }
  oops::Log::trace() << "[eval_dry_air_density_ad()] ... exit" << std::endl;
}

}  // namespace mo
