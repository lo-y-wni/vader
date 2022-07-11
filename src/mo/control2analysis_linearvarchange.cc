/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <string>
#include <vector>

#include "mo/constants.h"
#include "mo/control2analysis_linearvarchange.h"

#include "atlas/array/MakeView.h"

using atlas::array::make_view;

namespace mo {

void thetavP2HexnerTL(atlas::FieldSet & fields) {
    const auto rpView = make_view<const double, 2>(fields["height_levels"]);
    const auto thetavView = make_view<const double, 2>(fields["thetav"]);
    const auto pView = make_view<const double, 2>(fields["p"]);
    const auto hexnerView = make_view<const double, 2>(fields["hexner"]);
    const auto thetavIncView = make_view<const double, 2>(fields["thetavInc"]);
    const auto pIncView = make_view<const double, 2>(fields["pInc"]);
    auto hexnerIncView = make_view<double, 2>(fields["hexnerInc"]);

    for (atlas::idx_t jn = 0; jn < fields["hexner"].shape(0); ++jn) {
      hexnerIncView(jn, 0) = constants::rd_over_cp *
        hexnerView(jn, 0) * pIncView(jn, 0) / pView(jn, 0);

      for (atlas::idx_t jl = 1; jl < fields["hexner"].levels(); ++jl) {
        hexnerIncView(jn, jl) = hexnerIncView(jn, jl-1) +
          ((constants::grav * thetavIncView(jn, jl-1) *
            (rpView(jn, jl) - rpView(jn, jl-1))) /
            (constants::cp * thetavView(jn, jl-1) * thetavView(jn, jl-1)));
      }
    }
}

void thetavP2HexnerAD(atlas::FieldSet & fields) {
    const auto rpView = make_view<const double, 2>(fields["height_levels"]);
    const auto thetavView = make_view<const double, 2>(fields["thetav"]);
    const auto pView = make_view<const double, 2>(fields["p"]);
    const auto hexnerView = make_view<const double, 2>(fields["hexner"]);
    auto thetavHatView = make_view<double, 2>(fields["thetavHat"]);
    auto hexnerHatView = make_view<double, 2>(fields["hexnerHat"]);
    auto pHatView = make_view<double, 2>(fields["pHat"]);

    for (atlas::idx_t jn = 0; jn < fields["hexner"].shape(0); ++jn) {
      for (atlas::idx_t jl = fields["hexner"].levels()-1; jl > 0; --jl) {
        thetavHatView(jn, jl-1) = thetavHatView(jn, jl-1) +
          ((constants::grav * hexnerHatView(jn, jl) *
          (rpView(jn, jl) - rpView(jn, jl-1))) /
          (constants::cp * thetavView(jn, jl-1) * thetavView(jn, jl-1)));

        hexnerHatView(jn, jl-1) = hexnerHatView(jn, jl-1) +
          hexnerHatView(jn, jl);
        hexnerHatView(jn, jl) = 0.0;
      }
      pHatView(jn, 0) =  pHatView(jn, 0) +
        constants::rd_over_cp *
        hexnerView(jn, 0) * hexnerHatView(jn, 0) / pView(jn, 0);
      hexnerHatView(jn, 0) = 0.0;
    }
}

void hexner2ThetavTL(atlas::FieldSet & fields) {
  const auto rpView = make_view<const double, 2>(fields["height_levels"]);
  const auto thetavView = make_view<const double, 2>(fields["thetav"]);
  const auto hexnerIncView = make_view<const double, 2>(fields["hexnerInc"]);
  auto thetavIncView = make_view<double, 2>(fields["thetavInc"]);

  atlas::idx_t levelsm1 = fields["thetavInc"].levels()-1;
  for (atlas::idx_t jn = 0; jn < fields["thetavInc"].shape(0); ++jn) {
    for (atlas::idx_t jl = 0; jl < fields["thetavInc"].levels()-1; ++jl) {
      thetavIncView(jn, jl) =
        (hexnerIncView(jn, jl+1) - hexnerIncView(jn, jl)) *
            (constants::cp * thetavView(jn, jl) * thetavView(jn, jl)) /
            (constants::grav * (rpView(jn, jl) - rpView(jn, jl)));
    }
    // note thetavInc at surface is the same as level 1
    // thetavInc at top level is zero because we assume that
    // hexnerIncView(jn,thetalevels minus 1) =
    //   hexnerIncView(jn,thetalevels minus 2)
    // Note that hexner has rho levels not p.
    thetavIncView(jn, levelsm1) = 0.0;
  }
}

void hexner2ThetavAD(atlas::FieldSet & fields) {
  const auto rpView = make_view<const double, 2>(fields["height_levels"]);
  const auto thetavView = make_view<const double, 2>(fields["thetav"]);
  auto thetavHatView = make_view<double, 2>(fields["thetavHat"]);
  auto hexnerHatView = make_view<double, 2>(fields["hexnerHat"]);

  atlas::idx_t levelsm1 = fields["thetavHat"].levels()-1;
  atlas::idx_t levelsm2 = fields["thetavHat"].levels()-2;
  for (atlas::idx_t jn = 0; jn < fields["thetavHat"].shape(0); ++jn) {
    thetavHatView(jn, levelsm1) = 0.0;

    for (atlas::idx_t jl = levelsm2; jl > -1; --jl) {
      hexnerHatView(jn, jl) =  hexnerHatView(jn, jl) + thetavHatView(jn, jl) *
        (constants::cp * thetavView(jn, jl) * thetavView(jn, jl)) /
        (constants::grav * (rpView(jn, jl) - rpView(jn, jl)) );
      hexnerHatView(jn, jl) = hexnerHatView(jn, jl) - thetavHatView(jn, jl) *
        (constants::cp * thetavView(jn, jl) * thetavView(jn, jl)) /
        (constants::grav * (rpView(jn, jl) - rpView(jn, jl)));
      thetavHatView(jn, jl) = 0.0;
    }
  }
}

void thetavExner2RhoTL(atlas::FieldSet & fields) {
  const auto rpView = make_view<const double, 2>(fields["height_levels"]);
  const auto rthetaView = make_view<const double, 2>(fields["height"]);
  const auto exnerView = make_view<const double, 2>(fields["exner"]);
  const auto thetavView = make_view<const double, 2>(fields["thetav"]);
  const auto rhoRRView = make_view<const double, 2>(fields["rhoRR"]);
  const auto exnerIncView = make_view<const double, 2>(fields["exnerInc"]);
  const auto thetavIncView = make_view<const double, 2>(fields["thetavInc"]);
  auto rhoRRIncView = make_view<double, 2>(fields["rhoRRInc"]);

  for (atlas::idx_t jn = 0; jn < rhoRRIncView.shape(0); ++jn) {
    for (atlas::idx_t jl = 1; jl < fields["rhoRRInc"].levels(); ++jl) {
      rhoRRIncView(jn, jl) = rhoRRView(jn, jl) * (exnerIncView(jn, jl) /
        exnerView(jn, jl) - (((rpView(jn, jl) - rthetaView(jn, jl-1)) *
        thetavIncView(jn, jl) + (rthetaView(jn, jl) - rpView(jn, jl)) *
        thetavIncView(jn, jl-1)) / ((rpView(jn, jl) - rthetaView(jn, jl-1)) *
        thetavView(jn, jl) + (rthetaView(jn, jl) - rpView(jn, jl)) *
        thetavView(jn, jl-1))));
    }
    for (atlas::idx_t jl = 0; jl < fields["rhoRRInc"].levels(); ++jl) {
      rhoRRIncView(jn, jl) = rhoRRView(jn, jl) * (exnerIncView(jn, jl) /
      exnerView(jn, jl) - thetavIncView(jn, jl)/ thetavView(jn, jl));
    }
  }
}

void thetavExner2RhoAD(atlas::FieldSet & fields) {
  const auto rpView = make_view<const double, 2>(fields["height_levels"]);
  const auto rthetaView = make_view<const double, 2>(fields["height"]);
  const auto exnerView = make_view<const double, 2>(fields["exner"]);
  const auto thetavView = make_view<const double, 2>(fields["thetav"]);
  const auto rhoRRView = make_view<const double, 2>(fields["rhoRR"]);
  auto exnerHatView = make_view<double, 2>(fields["exnerHat"]);
  auto thetavHatView = make_view<double, 2>(fields["thetavHat"]);
  auto rhoRRHatView = make_view<double, 2>(fields["rhoRRHat"]);

  for (atlas::idx_t jn = 0; jn < rhoRRHatView.shape(0); ++jn) {
    for (atlas::idx_t jl = fields["rhoRRHat"].levels()-1; jl >= 0; --jl) {
      exnerHatView(jn, jl) += rhoRRView(jn, jl) * rhoRRHatView(jn, jl) /
        exnerView(jn, jl);

      thetavHatView(jn, jl+1) -= rhoRRView(jn, jl) * rhoRRHatView(jn, jl) *
        (rpView(jn, jl) - rthetaView(jn, jl)) /
        ((rpView(jn, jl) - rthetaView(jn, jl)) * thetavView(jn, jl+1) +
        (rthetaView(jn, jl+1) - rpView(jn, jl)) * thetavView(jn, jl) );

      thetavHatView(jn, jl) -= rhoRRView(jn, jl) * rhoRRHatView(jn, jl) *
        (rthetaView(jn, jl+1) - rpView(jn, jl)) /
        ((rpView(jn, jl) - rthetaView(jn, jl)) * thetavView(jn, jl+1) +
        (rthetaView(jn, jl+1) - rpView(jn, jl)) * thetavView(jn, jl));

      rhoRRHatView(jn, jl) = 0.0;
    }
  }
}

void qqclqcf2qtTL(atlas::FieldSet & incFields) {
  const auto qIncView = make_view<const double, 2>(incFields["specific_humidity"]);
  const auto qclIncView = make_view<const double, 2>
                    (incFields["mass_content_of_cloud_liquid_water_in_atmosphere_layer"]);
  const auto qcfIncView = make_view<const double, 2>
                    (incFields["mass_content_of_cloud_ice_in_atmosphere_layer"]);
  auto qtIncView = make_view<double, 2>(incFields["qt"]);

  for (atlas::idx_t jn = 0; jn < incFields["specific_humidity"].shape(0); ++jn) {
    for (atlas::idx_t jl = 0; jl < incFields["specific_humidity"].levels(); ++jl) {
      qtIncView(jn, jl) = qIncView(jn, jl) + qclIncView(jn, jl) + qcfIncView(jn, jl);
    }
  }
}

void qqclqcf2qtAD(atlas::FieldSet & hatFields) {
  auto qHatView = make_view<double, 2>(hatFields["specific_humidity"]);
  auto qclHatView = make_view<double, 2>
                    (hatFields["mass_content_of_cloud_liquid_water_in_atmosphere_layer"]);
  auto qcfHatView = make_view<double, 2>
                    (hatFields["mass_content_of_cloud_ice_in_atmosphere_layer"]);
  auto qtHatView = make_view<double, 2>(hatFields["qt"]);

  for (atlas::idx_t jn = 0; jn < hatFields["qt"].shape(0); ++jn) {
    for (atlas::idx_t jl = 0; jl < hatFields["qt"].levels(); ++jl) {
      qHatView(jn, jl) += qtHatView(jn, jl);
      qclHatView(jn, jl) += qtHatView(jn, jl);
      qcfHatView(jn, jl) += qtHatView(jn, jl);
      qtHatView(jn, jl) = 0.0;
    }
  }
}

void qtTemperature2qqclqcfTL(const atlas::FieldSet & stateFields,
                             const atlas::FieldSet & ceffFieldSet,
                             atlas::FieldSet & incFields) {
  const auto qsatView = make_view<const double, 2>(stateFields["qsat"]);
  const auto dlsvpdTView = make_view<const double, 2>(stateFields["dlsvpdT"]);

  const auto qtIncView = make_view<const double, 2>(incFields["qt"]);
  const auto temperIncView = make_view<const double, 2>(incFields["air_temperature"]);
  auto qclIncView = make_view<double, 2>
                    (incFields["mass_content_of_cloud_liquid_water_in_atmosphere_layer"]);
  auto qcfIncView = make_view<double, 2>
                    (incFields["mass_content_of_cloud_ice_in_atmosphere_layer"]);
  auto qIncView = make_view<double, 2>(incFields["specific_humidity"]);

  const auto cleffView = make_view<const double, 2>(ceffFieldSet["cleff"]);
  const auto cfeffView = make_view<const double, 2>(ceffFieldSet["cfeff"]);

  double maxCldInc;
  for (atlas::idx_t jn = 0; jn < incFields["qt"].shape(0); ++jn) {
    for (atlas::idx_t jl = 0; jl < incFields["qt"].levels(); ++jl) {
        maxCldInc = qtIncView(jn, jl) - qsatView(jn, jl) *
            dlsvpdTView(jn, jl) * temperIncView(jn, jl);
        qclIncView(jn, jl) = cleffView(jn, jl) * maxCldInc;
        qcfIncView(jn, jl) = cfeffView(jn, jl) * maxCldInc;
        qIncView(jn, jl) = qtIncView(jn, jl) - qclIncView(jn, jl) - qcfIncView(jn, jl);
    }
  }
}

void qtTemperature2qqclqcfAD(const atlas::FieldSet & stateFields,
                             const atlas::FieldSet & ceffFieldSet,
                             atlas::FieldSet & hatFields) {
  const auto qsatView = make_view<const double, 2>(stateFields["qsat"]);
  const auto dlsvpdTView = make_view<const double, 2>(stateFields["dlsvpdT"]);

  auto temperHatView = make_view<double, 2>(hatFields["air_temperature"]);
  auto qtHatView = make_view<double, 2>(hatFields["qt"]);
  auto qHatView = make_view<double, 2>(hatFields["specific_humidity"]);
  auto qclHatView = make_view<double, 2>
                    (hatFields["mass_content_of_cloud_liquid_water_in_atmosphere_layer"]);
  auto qcfHatView = make_view<double, 2>
                    (hatFields["mass_content_of_cloud_ice_in_atmosphere_layer"]);

  const auto cleffView = make_view<const double, 2>(ceffFieldSet["cleff"]);
  const auto cfeffView = make_view<const double, 2>(ceffFieldSet["cfeff"]);

  double qsatdlsvpdT;
  for (atlas::idx_t jn = 0; jn < hatFields["qt"].shape(0); ++jn) {
    for (atlas::idx_t jl = 0; jl < hatFields["qt"].levels(); ++jl) {
      qsatdlsvpdT = qsatView(jn, jl) * dlsvpdTView(jn, jl);
      temperHatView(jn, jl) += ((cleffView(jn, jl) + cfeffView(jn, jl)) * qHatView(jn, jl)
                                - cleffView(jn, jl) * qclHatView(jn, jl)
                                - cfeffView(jn, jl) * qcfHatView(jn, jl)) * qsatdlsvpdT;
      qtHatView(jn, jl) += cleffView(jn, jl) * qclHatView(jn, jl)
              + cfeffView(jn, jl) * qcfHatView(jn, jl)
              + (1.0 - cleffView(jn, jl) - cfeffView(jn, jl))
              * qHatView(jn, jl);
      qHatView(jn, jl) = 0.0;
      qclHatView(jn, jl) = 0.0;
      qcfHatView(jn, jl) = 0.0;
    }
  }
}

}  // namespace mo
