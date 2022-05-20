/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "mo/control2analysis_varchange.h"
/*
#include "atlas/array/MakeView.h"
#include "mo/utils.h"

namespace mo {

bool thetavP2Hexner(const atlas::Field & rp,
                    const atlas::Field & thetav,
                    const atlas::Field & p,
                    atlas::Field & hexner) {

  auto rpView = atlas::array::make_view<double, 2>(rp);
  auto thetavView = atlas::array::make_view<double, 2>(thetav);
  auto pView = atlas::array::make_view<double, 2>(p);
  auto hexnerView = atlas::array::make_view<double, 2>(hexner);

  for (atlas::idx_t jn = 0; jn < hexner.shape(0); ++jn) {
    hexnerView(jn, 0) = pow(pView(jn, 0) / mo::Constants::p_zero,
                        mo::Constants::rd_over_cp);
    for (atlas::idx_t jl = 1; jl < hexner.levels(); ++jl) {
      hexnerView(jn, jl) = hexnerView(jn, jl-1) -
        (mo::Constants::grav * (rpView(jn,jl) - rpView(jn, jl-1))) /
        (mo::Constants::cp * thetavView(jn,jl));
    }
  }
  return true;
}

}
*/
