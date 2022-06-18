/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <string>
#include <vector>

#include "mo/control2analysis_linearvarchange.h"
#include "mo/utils.h"

#include "atlas/array/MakeView.h"

namespace mo {

void thetavP2HexnerTL(atlas::FieldSet & fields) {
    const auto rpView = atlas::array::make_view<double, 2>(fields["height_levels"]);
    const auto thetavView = atlas::array::make_view<double, 2>(fields["thetav"]);
    const auto pView = atlas::array::make_view<double, 2>(fields["p"]);
    const auto hexnerView = atlas::array::make_view<double, 2>(fields["hexner"]);
    const auto thetavIncView = atlas::array::make_view<double, 2>(fields["thetavInc"]);
    const auto pIncView = atlas::array::make_view<double, 2>(fields["pInc"]);
    auto hexnerIncView = atlas::array::make_view<double, 2>(fields["hexnerInc"]);

    for (atlas::idx_t jn = 0; jn < fields["hexner"].shape(0); ++jn) {
      hexnerIncView(jn, 0) = mo::Constants::rd_over_cp *
        hexnerView(jn, 0) * pIncView(jn, 0) / pView(jn, 0);

      for (atlas::idx_t jl = 1; jl < fields["hexner"].levels(); ++jl) {
        hexnerIncView(jn, jl) = hexnerIncView(jn, jl-1) +
          ((mo::Constants::grav * thetavIncView(jn, jl-1) *
            (rpView(jn, jl) - rpView(jn, jl-1))) /
            (mo::Constants::cp * thetavView(jn, jl-1) * thetavView(jn, jl-1)));
      }
    }
}

void thetavP2HexnerAD(atlas::FieldSet & fields) {
    const auto rpView = atlas::array::make_view<double, 2>(fields["height_levels"]);
    const auto thetavView = atlas::array::make_view<double, 2>(fields["thetav"]);
    const auto pView = atlas::array::make_view<double, 2>(fields["p"]);
    const auto hexnerView = atlas::array::make_view<double, 2>(fields["hexner"]);
    auto thetavHatView = atlas::array::make_view<double, 2>(fields["thetavHat"]);
    auto pHatView = atlas::array::make_view<double, 2>(fields["pHat"]);
    auto hexnerHatView = atlas::array::make_view<double, 2>(fields["hexnerHat"]);

    for (atlas::idx_t jn = 0; jn < fields["hexner"].shape(0); ++jn) {
      for (atlas::idx_t jl = fields["hexner"].levels()-1; jl > 0; --jl) {
        thetavHatView(jn, jl-1) = thetavHatView(jn, jl-1) +
          ( (mo::Constants::grav * hexnerHatView(jn, jl) *
          (rpView(jn, jl) - rpView(jn, jl-1))) /
          (mo::Constants::cp * thetavView(jn, jl-1) * thetavView(jn, jl-1)));

        hexnerHatView(jn, jl-1) = hexnerHatView(jn, jl-1) +
          hexnerHatView(jn, jl);
        hexnerHatView(jn, jl) = 0.0;
      }

      pHatView(jn, 0) =  pHatView(jn, 0) +
        mo::Constants::rd_over_cp *
        hexnerView(jn, 0) * hexnerHatView(jn, 0) / pView(jn, 0);
      hexnerHatView(jn, 0) = 0.0;
    }
}

void hexner2ThetavTL(atlas::FieldSet & fields) {
  const auto rpView = atlas::array::make_view<double, 2>(fields["height_levels"]);
  const auto thetavView = atlas::array::make_view<double, 2>(fields["thetav"]);
  const auto hexnerIncView = atlas::array::make_view<double, 2>(fields["hexnerInc"]);
  auto thetavIncView = atlas::array::make_view<double, 2>(fields["thetavInc"]);

  atlas::idx_t levelsm1 = fields["thetavInc"].levels()-1;
  for (atlas::idx_t jn = 0; jn < fields["thetavInc"].shape(0); ++jn) {
    for (atlas::idx_t jl = 0; jl < fields["thetavInc"].levels()-1; ++jl) {
      thetavIncView(jn, jl) =
        (hexnerIncView(jn, jl+1) - hexnerIncView(jn, jl)) *
            (mo::Constants::cp * thetavView(jn, jl) * thetavView(jn, jl)) /
            (mo::Constants::grav * (rpView(jn, jl) - rpView(jn, jl)));
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
  const auto rpView = atlas::array::make_view<double, 2>(fields["height_levels"]);
  const auto thetavView = atlas::array::make_view<double, 2>(fields["thetav"]);
  auto thetavHatView = atlas::array::make_view<double, 2>(fields["thetavHat"]);
  auto hexnerHatView = atlas::array::make_view<double, 2>(fields["hexnerHat"]);

  atlas::idx_t levelsm1 = fields["thetavHat"].levels()-1;
  atlas::idx_t levelsm2 = fields["thetavHat"].levels()-2;
  for (atlas::idx_t jn = 0; jn < fields["thetavHat"].shape(0); ++jn) {
    thetavHatView(jn, levelsm1) = 0.0;

    for (atlas::idx_t jl = levelsm2; jl > -1; --jl) {
      hexnerHatView(jn, jl) =  hexnerHatView(jn, jl) + thetavHatView(jn, jl) *
        (mo::Constants::cp * thetavView(jn, jl) * thetavView(jn, jl)) /
        (mo::Constants::grav * (rpView(jn, jl) - rpView(jn, jl)) );
      hexnerHatView(jn, jl) = hexnerHatView(jn, jl) - thetavHatView(jn, jl) *
        (mo::Constants::cp * thetavView(jn, jl) * thetavView(jn, jl)) /
        (mo::Constants::grav * (rpView(jn, jl) - rpView(jn, jl)));
      thetavHatView(jn, jl) = 0.0;
    }
  }
}

void thetavExner2RhoTL(atlas::FieldSet & fields) {
  const auto rpView = atlas::array::make_view<double, 2>(fields["height_levels"]);
  const auto rthetaView = atlas::array::make_view<double, 2>(fields["height"]);
  const auto exnerView = atlas::array::make_view<double, 2>(fields["exner"]);
  const auto thetavView = atlas::array::make_view<double, 2>(fields["thetav"]);
  const auto rhoRRView = atlas::array::make_view<double, 2>(fields["rhoRR"]);
  const auto exnerIncView = atlas::array::make_view<double, 2>(fields["exnerInc"]);
  const auto thetavIncView = atlas::array::make_view<double, 2>(fields["thetavInc"]);
  auto rhoRRIncView = atlas::array::make_view<double, 2>(fields["rhoRRInc"]);

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
  const auto rpView = atlas::array::make_view<double, 2>(fields["height_levels"]);
  const auto rthetaView = atlas::array::make_view<double, 2>(fields["height"]);
  const auto exnerView = atlas::array::make_view<double, 2>(fields["exner"]);
  const auto thetavView = atlas::array::make_view<double, 2>(fields["thetav"]);
  const auto rhoRRView = atlas::array::make_view<double, 2>(fields["rhoRR"]);
  auto exnerHatView = atlas::array::make_view<double, 2>(fields["exnerHat"]);
  auto thetavHatView = atlas::array::make_view<double, 2>(fields["thetavHat"]);
  auto rhoRRHatView = atlas::array::make_view<double, 2>(fields["rhoRRHat"]);

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
  const std::vector<std::string> fnames {"specific_humidity",
              "mass_content_of_cloud_liquid_water_in_atmosphere_layer",
              "mass_content_of_cloud_ice_in_atmosphere_layer", "qt"};
  checkFieldSetContent(incFields, fnames);

  auto qIncView = atlas::array::make_view<const double, 2>(incFields["specific_humidity"]);
  auto qclIncView = atlas::array::make_view<const double, 2>
                    (incFields["mass_content_of_cloud_liquid_water_in_atmosphere_layer"]);
  auto qcfIncView = atlas::array::make_view<const double, 2>
                    (incFields["mass_content_of_cloud_ice_in_atmosphere_layer"]);
  auto qtIncView = atlas::array::make_view<double, 2>(incFields["qt"]);

  for (atlas::idx_t jn = 0; jn < incFields["specific_humidity"].shape(0); ++jn) {
    for (atlas::idx_t jl = 0; jl < incFields["specific_humidity"].levels(); ++jl) {
      qtIncView(jn, jl) = qIncView(jn, jl) + qclIncView(jn, jl) + qcfIncView(jn, jl);
    }
  }
}

void qqclqcf2qtAD(atlas::FieldSet & hatFields) {
  const std::vector<std::string> fnames {"specific_humidity",
              "mass_content_of_cloud_liquid_water_in_atmosphere_layer",
              "mass_content_of_cloud_ice_in_atmosphere_layer", "qt"};
  checkFieldSetContent(hatFields, fnames);

  auto qHatView = atlas::array::make_view<double, 2>(hatFields["specific_humidity"]);
  auto qclHatView = atlas::array::make_view<double, 2>
                    (hatFields["mass_content_of_cloud_liquid_water_in_atmosphere_layer"]);
  auto qcfHatView = atlas::array::make_view<double, 2>
                    (hatFields["mass_content_of_cloud_ice_in_atmosphere_layer"]);
  auto qtHatView = atlas::array::make_view<double, 2>(hatFields["qt"]);

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
  const std::vector<std::string> fnames {"dlsvpdT", "qsat", "qt"};
  checkFieldSetContent(stateFields, fnames);

  auto qtView = atlas::array::make_view<double, 2>(stateFields["qt"]);
  auto qsatView = atlas::array::make_view<double, 2>(stateFields["qsat"]);
  auto dlsvpdTView = atlas::array::make_view<double, 2>(stateFields["dlsvpdT"]);

  const std::vector<std::string> inc_fnames
                                 {"qt", "air_temperature",
                                  "specific_humidity",
                                  "mass_content_of_cloud_liquid_water_in_atmosphere_layer",
                                  "mass_content_of_cloud_ice_in_atmosphere_layer"};
  checkFieldSetContent(incFields, inc_fnames);

  auto qtIncView = atlas::array::make_view<const double, 2>(incFields["qt"]);
  auto temperIncView = atlas::array::make_view<const double, 2>(incFields["air_temperature"]);
  auto qIncView = atlas::array::make_view<double, 2>
                  (incFields["specific_humidity"]);
  auto qclIncView = atlas::array::make_view<double, 2>
                    (incFields["mass_content_of_cloud_liquid_water_in_atmosphere_layer"]);
  auto qcfIncView = atlas::array::make_view<double, 2>
                    (incFields["mass_content_of_cloud_ice_in_atmosphere_layer"]);

  double maxCldInc;

  const std::vector<std::string> ceff_fnames {"cleff", "cfeff"};
  checkFieldSetContent(ceffFieldSet, ceff_fnames);

  auto cleffView = atlas::array::make_view<double, 2>(ceffFieldSet["cleff"]);
  auto cfeffView = atlas::array::make_view<double, 2>(ceffFieldSet["cfeff"]);

  for (atlas::idx_t jn = 0; jn < incFields["qt"].shape(0); ++jn) {
    for (atlas::idx_t jl = 0; jl < incFields["qt"].levels(); ++jl) {
        maxCldInc = qtIncView(jn, jl)
                - qsatView(jn, jl) * dlsvpdTView(jn, jl) * temperIncView(jn, jl);
        qclIncView(jn, jl) =  cleffView(jn, jl) * maxCldInc;
        qcfIncView(jn, jl) =  cfeffView(jn, jl) * maxCldInc;
        qIncView(jn, jl) = qtIncView(jn, jl) - qclIncView(jn, jl) - qcfIncView(jn, jl);
    }
  }
}

void qtTemperature2qqclqcfAD(const atlas::FieldSet & stateFields,
                             const atlas::FieldSet & ceffFieldSet,
                             atlas::FieldSet & hatFields) {
  const std::vector<std::string> fnames {"dlsvpdT", "qsat", "qt"};
  checkFieldSetContent(stateFields, fnames);

  auto qtView = atlas::array::make_view<double, 2>(stateFields["qt"]);
  auto qsatView = atlas::array::make_view<double, 2>(stateFields["qsat"]);
  auto dlsvpdTView = atlas::array::make_view<double, 2>(stateFields["dlsvpdT"]);

  const std::vector<std::string> hat_fnames
                                 {"qt", "air_temperature",
                                  "specific_humidity",
                                  "mass_content_of_cloud_liquid_water_in_atmosphere_layer",
                                  "mass_content_of_cloud_ice_in_atmosphere_layer"};
  checkFieldSetContent(hatFields, hat_fnames);

  auto qtHatView = atlas::array::make_view<double, 2>(hatFields["qt"]);
  auto temperHatView = atlas::array::make_view<double, 2>(hatFields["air_temperature"]);
  auto qHatView = atlas::array::make_view<double, 2>(hatFields["specific_humidity"]);
  auto qclHatView = atlas::array::make_view<double, 2>
                    (hatFields["mass_content_of_cloud_liquid_water_in_atmosphere_layer"]);
  auto qcfHatView = atlas::array::make_view<double, 2>
                    (hatFields["mass_content_of_cloud_ice_in_atmosphere_layer"]);

  double qsatdlsvpdT;

  const std::vector<std::string> ceff_fnames {"cleff", "cfeff"};
  checkFieldSetContent(ceffFieldSet, ceff_fnames);

  auto cleffView = atlas::array::make_view<double, 2>(ceffFieldSet["cleff"]);
  auto cfeffView = atlas::array::make_view<double, 2>(ceffFieldSet["cfeff"]);

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
