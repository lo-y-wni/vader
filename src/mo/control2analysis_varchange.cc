/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "atlas/array/MakeView.h"
#include "mo/constants.h"
#include "mo/control2analysis_varchange.h"

using atlas::array::make_view;

namespace mo {

bool thetavP2Hexner(atlas::FieldSet & fields) {
  const auto rpView = make_view<const double, 2>(fields["rp"]);
  const auto thetavView = make_view<const double, 2>(fields["thetav"]);
  const auto pView = make_view<const double, 2>(fields["p"]);
  auto hexnerView = make_view<double, 2>(fields["hexner"]);

  for (atlas::idx_t jn = 0; jn < fields["hexner"].shape(0); ++jn) {
    hexnerView(jn, 0) = pow(pView(jn, 0) / constants::p_zero,
      constants::rd_over_cp);
    for (atlas::idx_t jl = 1; jl < fields["hexner"].levels(); ++jl) {
      hexnerView(jn, jl) = hexnerView(jn, jl-1) -
        (constants::grav * (rpView(jn, jl) - rpView(jn, jl-1))) /
        (constants::cp * thetavView(jn, jl));
    }
  }
  return true;
}

void hexner2PThetav(atlas::FieldSet & fields) {
  const auto rpView = make_view<const double, 2>(fields["rp"]);
  const auto hexnerView = make_view<const double, 2>(fields["hexner"]);
  auto pView = make_view<double, 2>(fields["p"]);
  auto thetavView = make_view<double, 2>(fields["thetav"]);

  for (atlas::idx_t jn = 0; jn < fields["hexner"].shape(0); ++jn) {
    pView(jn, 0) = constants::p_zero * pow(hexnerView(jn, 0), (constants::cp / constants::rd));

    for (atlas::idx_t jl = 1; jl < fields["hexner"].levels(); ++jl) {
      thetavView(jn, jl) = -constants::grav * (rpView(jn, jl) - rpView(jn, jl-1)) /
         (constants::cp * (hexnerView(jn, jl) - hexnerView(jn, jl-1)));
    }
    thetavView(jn, 0) = thetavView(jn, 1);
  }
}

}  // namespace mo
