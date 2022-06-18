/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "atlas/array/MakeView.h"
#include "mo/control2analysis_varchange.h"
#include "mo/utils.h"

namespace mo {

bool thetavP2Hexner(atlas::FieldSet & fields) {
  const auto rpView = atlas::array::make_view<double, 2>(fields["rp"]);
  const auto thetavView = atlas::array::make_view<double, 2>(fields["thetav"]);
  const auto pView = atlas::array::make_view<double, 2>(fields["p"]);
  auto hexnerView = atlas::array::make_view<double, 2>(fields["hexner"]);

  for (atlas::idx_t jn = 0; jn < fields["hexner"].shape(0); ++jn) {
    hexnerView(jn, 0) = pow(pView(jn, 0) / mo::Constants::p_zero,
      mo::Constants::rd_over_cp);
    for (atlas::idx_t jl = 1; jl < fields["hexner"].levels(); ++jl) {
      hexnerView(jn, jl) = hexnerView(jn, jl-1) -
        (mo::Constants::grav * (rpView(jn, jl) - rpView(jn, jl-1))) /
        (mo::Constants::cp * thetavView(jn, jl));
    }
  }
  return true;
}

void hexner2PThetav(atlas::FieldSet & fields) {
  auto rpView = atlas::array::make_view<double, 2>(fields["rp"]);
  auto thetavView = atlas::array::make_view<double, 2>(fields["thetav"]);
  auto pView = atlas::array::make_view<double, 2>(fields["p"]);
  auto hexnerView = atlas::array::make_view<double, 2>(fields["hexner"]);

  for (atlas::idx_t jn = 0; jn < fields["hexner"].shape(0); ++jn) {
    pView(jn, 0) =  mo::Constants::p_zero *
      pow(hexnerView(jn, 0), (mo::Constants::cp/mo::Constants::rd));

    for (atlas::idx_t jl = 1; jl < fields["hexner"].levels(); ++jl) {
      thetavView(jn, jl) = -mo::Constants::grav * (rpView(jn, jl) -
        rpView(jn, jl-1)) / (mo::Constants::cp * (hexnerView(jn, jl) -
        hexnerView(jn, jl-1)));
    }
    thetavView(jn, 0) = thetavView(jn, 1);
  }
}

}  // namespace mo
