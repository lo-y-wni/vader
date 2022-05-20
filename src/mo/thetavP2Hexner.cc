/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "atlas/array/MakeView.h"
#include "atlas/field/Field.h"
#include "atlas/field/FieldSet.h"
#include "atlas/functionspace/StructuredColumns.h"
#include "atlas/mesh/Mesh.h"
#include "atlas/parallel/mpi/mpi.h"
#include "atlas/runtime/Log.h"

#include "src/mo/thetavP2Hexner.h"

#include "ufo/utils/Constants.h"
#include "um-jedi/Utilities/Parameters.h"

namespace unifiedmodel {


void hexner2PThetav(const atlas::Field & rp,
                    const atlas::Field & hexner,
                    atlas::Field & p,
                    atlas::Field & thetav) {
  auto rpView = atlas::array::make_view<double, 2>(rp);
  auto thetavView = atlas::array::make_view<double, 2>(thetav);
  auto pView = atlas::array::make_view<double, 2>(p);
  auto hexnerView = atlas::array::make_view<double, 2>(hexner);

  for (atlas::idx_t jn = 0; jn < hexner.shape(0); ++jn) {
    pView(jn, 0) =  pZero *
      pow(hexnerView(jn, 0), (ufo::Constants::cp/ufo::Constants::rd));

    for (atlas::idx_t jl = 1; jl < hexner.levels(); ++jl) {
      thetavView(jn, jl) = - ufo::Constants::grav * (rpView(jn, jl) - rpView(jn, jl-1))
        / (ufo::Constants::cp * (hexnerView(jn, jl) - hexnerView(jn, jl-1)));
    }
    thetavView(jn, 0) = thetavView(jn, 1);
  }
}


void thetavP2HexnerTL(const atlas::Field & rp,
                      const atlas::Field & thetav,
                      const atlas::Field & p,
                      const atlas::Field & hexner,
                      const atlas::Field & thetavInc,
                      const atlas::Field & pInc,
                      atlas::Field & hexnerInc) {
    auto rpView = atlas::array::make_view<double, 2>(rp);
    auto thetavView = atlas::array::make_view<double, 2>(thetav);
    auto pView = atlas::array::make_view<double, 2>(p);
    auto hexnerView = atlas::array::make_view<double, 2>(hexner);
    auto thetavIncView = atlas::array::make_view<double, 2>(thetavInc);
    auto pIncView = atlas::array::make_view<double, 2>(pInc);
    auto hexnerIncView = atlas::array::make_view<double, 2>(hexnerInc);

    for (atlas::idx_t jn = 0; jn < hexner.shape(0); ++jn) {
      hexnerIncView(jn, 0) = ufo::Constants::rd_over_cp *
        hexnerView(jn, 0) * pIncView(jn, 0) / pView(jn, 0);

      for (atlas::idx_t jl = 1; jl < hexner.levels(); ++jl) {
        hexnerIncView(jn, jl) = hexnerIncView(jn, jl-1) +
          ( (ufo::Constants::grav * thetavIncView(jn, jl-1) *
            (rpView(jn, jl) - rpView(jn, jl-1))
             )
          / (ufo::Constants::cp * thetavView(jn, jl-1) * thetavView(jn, jl-1)));
      }
    }
}


void thetavP2HexnerAD(const atlas::Field & rp,
                      const atlas::Field & thetav,
                      const atlas::Field & p,
                      const atlas::Field & hexner,
                      atlas::Field & thetavHat,
                      atlas::Field & pHat,
                      atlas::Field & hexnerHat) {
    auto rpView = atlas::array::make_view<double, 2>(rp);
    auto thetavView = atlas::array::make_view<double, 2>(thetav);
    auto pView = atlas::array::make_view<double, 2>(p);
    auto hexnerView = atlas::array::make_view<double, 2>(hexner);
    auto thetavHatView = atlas::array::make_view<double, 2>(thetavHat);
    auto pHatView = atlas::array::make_view<double, 2>(pHat);
    auto hexnerHatView = atlas::array::make_view<double, 2>(hexnerHat);

    for (atlas::idx_t jn = 0; jn < hexner.shape(0); ++jn) {
      for (atlas::idx_t jl = hexner.levels()-1; jl > 0; --jl) {
        thetavHatView(jn, jl-1) = thetavHatView(jn, jl-1) +
          ( (ufo::Constants::grav * hexnerHatView(jn, jl) *
                  (rpView(jn, jl) - rpView(jn, jl-1))
                   )
            / (ufo::Constants::cp * thetavView(jn, jl-1) * thetavView(jn, jl-1)));

        hexnerHatView(jn, jl-1) = hexnerHatView(jn, jl-1) +
          hexnerHatView(jn, jl);
        hexnerHatView(jn, jl) = 0.0;
      }

      pHatView(jn, 0) =  pHatView(jn, 0) +
        ufo::Constants::rd_over_cp *
        hexnerView(jn, 0) * hexnerHatView(jn, 0) / pView(jn, 0);
      hexnerHatView(jn, 0) = 0.0;
    }
}

void hexner2ThetavTL(const atlas::Field & rp,
                     const atlas::Field & thetav,
                     const atlas::Field & hexnerInc,
                     atlas::Field & thetavInc) {
  auto rpView = atlas::array::make_view<double, 2>(rp);
  auto thetavView = atlas::array::make_view<double, 2>(thetav);
  auto hexnerIncView = atlas::array::make_view<double, 2>(hexnerInc);
  auto thetavIncView = atlas::array::make_view<double, 2>(thetavInc);

  atlas::idx_t levelsm1 = thetavInc.levels()-1;
  for (atlas::idx_t jn = 0; jn < thetavInc.shape(0); ++jn) {
    for (atlas::idx_t jl = 1; jl < thetavInc.levels()-1; ++jl) {
      thetavIncView(jn, jl) =
        (hexnerIncView(jn, jl) - hexnerIncView(jn, jl-1)) *
        (ufo::Constants::cp * thetavView(jn, jl) * thetavView(jn, jl)) /
        (ufo::Constants::grav * (rpView(jn, jl) - rpView(jn, jl-1)));
    }
    // note thetavInc at surface is the same as level 1
    // thetavInc at top level is zero because we assume that
    // hexnerIncView(jn,thetalevels minus 1) =
    //   hexnerIncView(jn,thetalevels minus 2)
    // Note that hexner has rho levels not p.
    thetavIncView(jn, levelsm1) = 0.0;
    thetavIncView(jn, 0) = thetavIncView(jn, 1);
  }
}

void hexner2ThetavAD(const atlas::Field & rp,
                     const atlas::Field & thetav,
                     atlas::Field & thetavHat,
                     atlas::Field & hexnerHat) {
  auto rpView = atlas::array::make_view<double, 2>(rp);
  auto thetavView = atlas::array::make_view<double, 2>(thetav);
  auto thetavHatView = atlas::array::make_view<double, 2>(thetavHat);
  auto hexnerHatView = atlas::array::make_view<double, 2>(hexnerHat);

  atlas::idx_t levelsm1 = thetavHat.levels()-1;
  atlas::idx_t levelsm2 = thetavHat.levels()-2;
  for (atlas::idx_t jn = 0; jn < thetavHat.shape(0); ++jn) {
    thetavHatView(jn, levelsm1) = 0.0;
    thetavHatView(jn, 1) += thetavHatView(jn, 0);
    thetavHatView(jn, 0) = 0.0;

    for (atlas::idx_t jl = levelsm2; jl > 0; --jl) {
      hexnerHatView(jn, jl) =  hexnerHatView(jn, jl) + thetavHatView(jn, jl) *
        (ufo::Constants::cp * thetavView(jn, jl) * thetavView(jn, jl)) /
        (ufo::Constants::grav * (rpView(jn, jl) - rpView(jn, jl-1)) );
      hexnerHatView(jn, jl-1) = hexnerHatView(jn, jl-1) - thetavHatView(jn, jl) *
        (ufo::Constants::cp * thetavView(jn, jl) * thetavView(jn, jl)) /
        (ufo::Constants::grav * (rpView(jn, jl) - rpView(jn, jl-1)));
      thetavHatView(jn, jl) = 0.0;
    }
  }
}


void thetavExner2RhoTL(const atlas::Field & rp,
                     const atlas::Field & rtheta,
                     const atlas::Field & exner,
                     const atlas::Field & thetav,
                     const atlas::Field & rhoRR,
                     const atlas::Field & exnerInc,
                     const atlas::Field & thetavInc,
                     atlas::Field & rhoRRInc) {
  auto rpView = atlas::array::make_view<double, 2>(rp);
  auto rthetaView = atlas::array::make_view<double, 2>(rtheta);
  auto exnerView = atlas::array::make_view<double, 2>(exner);
  auto thetavView = atlas::array::make_view<double, 2>(thetav);
  auto rhoRRView = atlas::array::make_view<double, 2>(rhoRR);
  auto exnerIncView = atlas::array::make_view<double, 2>(exnerInc);
  auto thetavIncView = atlas::array::make_view<double, 2>(thetavInc);
  auto rhoRRIncView = atlas::array::make_view<double, 2>(rhoRRInc);

  for (atlas::idx_t jn = 0; jn < rhoRRIncView.shape(0); ++jn) {
    for (atlas::idx_t jl = 0; jl < rhoRRInc.levels(); ++jl) {
      rhoRRIncView(jn, jl) = rhoRRView(jn, jl) * (
        exnerIncView(jn, jl)/exnerView(jn, jl) -
        ( ( (rpView(jn, jl) - rthetaView(jn, jl)) * thetavIncView(jn, jl+1)
         +  (rthetaView(jn, jl+1) - rpView(jn, jl)) * thetavIncView(jn, jl) ) /
          ( (rpView(jn, jl) - rthetaView(jn, jl)) * thetavView(jn, jl+1)
         +  (rthetaView(jn, jl+1) - rpView(jn, jl)) * thetavView(jn, jl) )));
    }
  }
}


void thetavExner2RhoAD(const atlas::Field & rp,
                     const atlas::Field & rtheta,
                     const atlas::Field & exner,
                     const atlas::Field & thetav,
                     const atlas::Field & rhoRR,
                     atlas::Field & exnerHat,
                     atlas::Field & thetavHat,
                     atlas::Field & rhoRRHat) {
  auto rpView = atlas::array::make_view<double, 2>(rp);
  auto rthetaView = atlas::array::make_view<double, 2>(rtheta);
  auto exnerView = atlas::array::make_view<double, 2>(exner);
  auto thetavView = atlas::array::make_view<double, 2>(thetav);
  auto rhoRRView = atlas::array::make_view<double, 2>(rhoRR);
  auto exnerHatView = atlas::array::make_view<double, 2>(exnerHat);
  auto thetavHatView = atlas::array::make_view<double, 2>(thetavHat);
  auto rhoRRHatView = atlas::array::make_view<double, 2>(rhoRRHat);

  for (atlas::idx_t jn = 0; jn < rhoRRHatView.shape(0); ++jn) {
    for (atlas::idx_t jl = rhoRRHat.levels()-1; jl >= 0; --jl) {
      exnerHatView(jn, jl) += rhoRRView(jn, jl) * rhoRRHatView(jn, jl) /
        exnerView(jn, jl);

      thetavHatView(jn, jl+1) -= rhoRRView(jn, jl) * rhoRRHatView(jn, jl) *
        (rpView(jn, jl) - rthetaView(jn, jl)) /
        ( (rpView(jn, jl) - rthetaView(jn, jl)) * thetavView(jn, jl+1)
        + (rthetaView(jn, jl+1) - rpView(jn, jl)) * thetavView(jn, jl) );

      thetavHatView(jn, jl) -= rhoRRView(jn, jl) * rhoRRHatView(jn, jl) *
        (rthetaView(jn, jl+1) - rpView(jn, jl)) /
        ( (rpView(jn, jl) - rthetaView(jn, jl)) * thetavView(jn, jl+1)
        + (rthetaView(jn, jl+1) - rpView(jn, jl)) * thetavView(jn, jl));

      rhoRRHatView(jn, jl) = 0.0;
    }
  }
}

void hExner2exnerTL(const atlas::Field & hexnerInc,
                    atlas::Field & exnerInc) {
  auto hexnerIncView = atlas::array::make_view<double, 2>(hexnerInc);
  auto exnerIncView = atlas::array::make_view<double, 2>(exnerInc);

  for (atlas::idx_t jn = 0; jn < exnerInc.shape(0); ++jn) {
    for (atlas::idx_t jl = 0; jl < exnerInc.levels() - 1; ++jl) {
      exnerIncView(jn, jl) = hexnerIncView(jn, jl);
    }
     exnerIncView(jn, exnerInc.levels()-1) =
       exnerIncView(jn, exnerInc.levels()-2);
  }
}

void hExner2exnerAD(atlas::Field & exnerHat,
                    atlas::Field & hexnerHat) {
  auto hexnerHatView = atlas::array::make_view<double, 2>(hexnerHat);
  auto exnerHatView = atlas::array::make_view<double, 2>(exnerHat);

  for (atlas::idx_t jn = 0; jn < exnerHat.shape(0); ++jn) {
    exnerHatView(jn, exnerHat.levels()-2) +=
        exnerHatView(jn, exnerHat.levels()-1);
    for (atlas::idx_t jl = 0; jl < exnerHat.levels()- 1; ++jl) {
      hexnerHatView(jn, jl) += exnerHatView(jn, jl);
    }
  }
}

}  // namespace unifiedmodel



