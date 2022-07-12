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

void thetavP2HexnerTL(atlas::FieldSet & incFlds, const atlas::FieldSet & augStateFlds) {
  const auto rpView = make_view<const double, 2>(augStateFlds["height_levels"]);
  const auto thetavView = make_view<const double, 2>(
    augStateFlds["virtual_potential_temperature"]);
  const auto pView = make_view<const double, 2>(augStateFlds["air_pressure_levels_minus_one"]);
  const auto hexnerView = make_view<const double, 2>(augStateFlds["exner_levels_minus_one"]);
  const auto thetavIncView = make_view<const double, 2>(incFlds["virtual_potential_temperature"]);
  const auto pIncView = make_view<const double, 2>(incFlds["air_pressure_levels_minus_one"]);
  auto hexnerIncView = make_view<double, 2>(incFlds["hydrostatic_exner_levels"]);

  for (atlas::idx_t jn = 0; jn < incFlds["hydrostatic_exner_levels"].shape(0); ++jn) {
    hexnerIncView(jn, 0) = constants::rd_over_cp *
      hexnerView(jn, 0) * pIncView(jn, 0) / pView(jn, 0);

    for (atlas::idx_t jl = 1; jl < incFlds["hydrostatic_exner_levels"].levels(); ++jl) {
      hexnerIncView(jn, jl) = hexnerIncView(jn, jl-1) +
        ((constants::grav * thetavIncView(jn, jl-1) *
          (rpView(jn, jl) - rpView(jn, jl-1))) /
         (constants::cp * thetavView(jn, jl-1) * thetavView(jn, jl-1)));
    }
  }
}

void thetavP2HexnerAD(atlas::FieldSet & hatFlds, const atlas::FieldSet & augStateFlds) {
  const auto rpView = make_view<const double, 2>(augStateFlds["height_levels"]);
  const auto thetavView = make_view<const double, 2>(
    augStateFlds["virtual_potential_temperature"]);
  const auto pView = make_view<const double, 2>(augStateFlds["air_pressure_levels_minus_one"]);
  const auto hexnerView = make_view<const double, 2>(augStateFlds["exner_levels_minus_one"]);
  auto thetavHatView = make_view<double, 2>(hatFlds["virtual_potential_temperature"]);
  auto pHatView = make_view<double, 2>(hatFlds["air_pressure_levels_minus_one"]);
  auto hexnerHatView = make_view<double, 2>(hatFlds["hydrostatic_exner_levels"]);

  for (atlas::idx_t jn = 0; jn < hatFlds["hydrostatic_exner_levels"].shape(0); ++jn) {
    for (atlas::idx_t jl = hatFlds["hydrostatic_exner_levels"].levels() - 1; jl > 0; --jl) {
      thetavHatView(jn, jl-1) = thetavHatView(jn, jl-1) +
        ((constants::grav * hexnerHatView(jn, jl) *
        (rpView(jn, jl) - rpView(jn, jl-1))) /
        (constants::cp * thetavView(jn, jl-1) * thetavView(jn, jl-1)));

      hexnerHatView(jn, jl-1) = hexnerHatView(jn, jl-1) +
        hexnerHatView(jn, jl);
      hexnerHatView(jn, jl) = 0.0;
    }
    pHatView(jn, 0) = pHatView(jn, 0) +
      constants::rd_over_cp *
      hexnerView(jn, 0) * hexnerHatView(jn, 0) / pView(jn, 0);
    hexnerHatView(jn, 0) = 0.0;
  }
}

void hexner2ThetavTL(atlas::FieldSet & incFlds, const atlas::FieldSet & augStateFlds) {
  const auto rpView = make_view<const double, 2>(augStateFlds["height_levels"]);
  const auto thetavView = make_view<const double, 2>(augStateFlds["virtual_potential_temperature"]);
  const auto hexnerIncView = make_view<const double, 2>(incFlds["hydrostatic_exner_levels"]);
  auto thetavIncView = make_view<double, 2>(incFlds["virtual_potential_temperature"]);

  for (atlas::idx_t jn = 0; jn < incFlds["virtual_potential_temperature"].shape(0); ++jn) {
    for (atlas::idx_t jl = 0; jl < incFlds["virtual_potential_temperature"].levels()-1; ++jl) {
      thetavIncView(jn, jl) =
        (hexnerIncView(jn, jl+1) - hexnerIncView(jn, jl)) *
            (constants::cp * thetavView(jn, jl) * thetavView(jn, jl)) /
            (constants::grav * (rpView(jn, jl) - rpView(jn, jl)));
    }
  }
}

void hexner2ThetavAD(atlas::FieldSet & hatFlds, const atlas::FieldSet & augStateFlds) {
  const auto rpView = make_view<const double, 2>(augStateFlds["height_levels"]);
  const auto thetavView = make_view<const double, 2>(augStateFlds["virtual_potential_temperature"]);
  auto thetavHatView = make_view<double, 2>(hatFlds["virtual_potential_temperature"]);
  auto hexnerHatView = make_view<double, 2>(hatFlds["hydrostatic_exner_levels"]);

  atlas::idx_t levelsm1 = hatFlds["virtual_potential_temperature"].levels()-1;
  atlas::idx_t levelsm2 = hatFlds["virtual_potential_temperature"].levels()-2;
  for (atlas::idx_t jn = 0; jn < hatFlds["virtual_potential_temperature"].shape(0); ++jn) {
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

void thetavExner2RhoTL(atlas::FieldSet & incFlds, const atlas::FieldSet & augStateFlds) {
  const auto rpView = make_view<const double, 2>(augStateFlds["height_levels"]);
  const auto rthetaView = make_view<const double, 2>(augStateFlds["height"]);
  const auto exnerView = make_view<const double, 2>(augStateFlds["exner_levels_minus_one"]);
  const auto thetavView = make_view<const double, 2>(augStateFlds["virtual_potential_temperature"]);
  const auto rhoView = make_view<const double, 2>(augStateFlds["air_density_levels_minus_one"]);
  const auto exnerIncView = make_view<const double, 2>(incFlds["exner_levels_minus_one"]);
  const auto thetavIncView = make_view<const double, 2>(incFlds["virtual_potential_temperature"]);
  auto rhoIncView = make_view<double, 2>(incFlds["air_density_levels_minus_one"]);

  for (atlas::idx_t jn = 0; jn < rhoIncView.shape(0); ++jn) {
    for (atlas::idx_t jl = 1; jl < incFlds["air_density_levels_minus_one"].levels(); ++jl) {
      rhoIncView(jn, jl) = rhoView(jn, jl) * (
        exnerIncView(jn, jl) / exnerView(jn, jl) -
        (((rpView(jn, jl) - rthetaView(jn, jl-1)) * thetavIncView(jn, jl) +
          (rthetaView(jn, jl) - rpView(jn, jl)) * thetavIncView(jn, jl-1)) /
         ((rpView(jn, jl) - rthetaView(jn, jl-1)) * thetavView(jn, jl) +
          (rthetaView(jn, jl) - rpView(jn, jl)) * thetavView(jn, jl-1))));
    }

    rhoIncView(jn, 0) = rhoView(jn, 0) * (
        exnerIncView(jn, 0) / exnerView(jn, 0) -
        thetavIncView(jn, 0)/ thetavView(jn, 0));
  }
}

void thetavExner2RhoAD(atlas::FieldSet & hatFlds, const atlas::FieldSet & augStateFlds) {
  const auto rpView = make_view<const double, 2>(augStateFlds["height_levels"]);
  const auto rthetaView = make_view<const double, 2>(augStateFlds["height"]);
  const auto exnerView = make_view<const double, 2>(augStateFlds["exner_levels_minus_one"]);
  const auto thetavView = make_view<const double, 2>(augStateFlds["virtual_potential_temperature"]);
  const auto rhoView = make_view<const double, 2>(augStateFlds["air_density_levels_minus_one"]);
  auto exnerHatView = make_view<double, 2>(hatFlds["exner_levels_minus_one"]);
  auto thetavHatView = make_view<double, 2>(hatFlds["virtual_potential_temperature"]);
  auto rhoHatView = make_view<double, 2>(hatFlds["air_density_levels_minus_one"]);

  for (atlas::idx_t jn = 0; jn < rhoHatView.shape(0); ++jn) {
    exnerHatView(jn, 0) += rhoView(jn, 0) * rhoHatView(jn, 0) /
      exnerView(jn, 0);
    thetavHatView(jn, 0) -= rhoView(jn, 0) * rhoHatView(jn, 0) /
      thetavView(jn, 0);
    rhoHatView(jn, 0) = 0.0;

    for (atlas::idx_t jl = hatFlds["air_density_levels_minus_one"].levels()-1; jl >= 1; --jl) {
      exnerHatView(jn, jl) += rhoView(jn, jl) * rhoHatView(jn, jl) /
        exnerView(jn, jl);
      thetavHatView(jn, jl) -= rhoView(jn, jl) * rhoHatView(jn, jl) *
        (rpView(jn, jl) - rthetaView(jn, jl-1)) /
        ((rpView(jn, jl) - rthetaView(jn, jl-1)) * thetavView(jn, jl) +
        (rthetaView(jn, jl) - rpView(jn, jl)) * thetavView(jn, jl-1) );
      thetavHatView(jn, jl-1) -= rhoView(jn, jl) * rhoHatView(jn, jl) *
        (rthetaView(jn, jl) - rpView(jn, jl)) /
        ((rpView(jn, jl) - rthetaView(jn, jl-1)) * thetavView(jn, jl) +
        (rthetaView(jn, jl) - rpView(jn, jl)) * thetavView(jn, jl-1));
      rhoHatView(jn, jl) = 0.0;
    }
  }
}


/// \details This calculates air temperature increments.
void evalAirTemperatureTL(atlas::FieldSet & incFlds, const atlas::FieldSet & augStateFlds) {
  const auto rpView = make_view<const double, 2>(augStateFlds["height_levels"]);
  const auto rthetaView = make_view<const double, 2>(augStateFlds["height"]);
  const auto exnerLevelsView = make_view<const double, 2>(augStateFlds["exner_levels_minus_one"]);
  const auto thetaView = make_view<const double, 2>(augStateFlds["theta"]);
  const auto exnerLevelsIncView = make_view<const double, 2>(incFlds["exner_levels_minus_one"]);
  const auto thetaIncView = make_view<const double, 2>(incFlds["theta"]);
  auto tIncView = make_view<double, 2>(incFlds["air_temperature"]);

  atlas::idx_t lvls(incFlds["air_temperature"].levels());
  atlas::idx_t lvlsm1 = lvls - 1;
  double exnerTopVal;
  double exnerTopIncVal;
  for (atlas::idx_t jn = 0; jn < tIncView.shape(0); ++jn) {
    for (atlas::idx_t jl= 0; jl < lvls - 1; ++jl) {
       tIncView(jn, jl) = (
         ( (rthetaView(jn, jl) - rpView(jn, jl)) * exnerLevelsView(jn, jl + 1) +
           (rpView(jn, jl+1)  - rthetaView(jn, jl)) * exnerLevelsView(jn, jl) ) *
           thetaIncView(jn, jl) +
         ( (rthetaView(jn, jl) - rpView(jn, jl)) * exnerLevelsIncView(jn, jl + 1) +
           (rpView(jn, jl+1)  - rthetaView(jn, jl)) * exnerLevelsIncView(jn, jl) ) *
           thetaView(jn, jl) ) /
         (rpView(jn, jl+1) - rpView(jn, jl));
    }

    // Value above model top is assumed to be in hydrostatic balance.
    exnerTopVal = exnerLevelsView(jn, lvlsm1) -
      (constants::grav * (rpView(jn, lvls) - rpView(jn, lvlsm1))) /
      (constants::cp * thetaView(jn, lvlsm1));

    exnerTopIncVal = exnerLevelsIncView(jn, lvlsm1) +
      thetaIncView(jn, lvlsm1) * (exnerLevelsView(jn, lvlsm1) - exnerTopVal) /
      thetaView(jn, lvlsm1);

    tIncView(jn, lvlsm1) = (
      ( (rthetaView(jn, lvlsm1) - rpView(jn, lvlsm1)) * exnerTopVal +
        (rpView(jn, lvls)  - rthetaView(jn, lvlsm1)) * exnerLevelsView(jn, lvlsm1) ) *
         thetaIncView(jn, lvlsm1) +
      ( (rthetaView(jn, lvlsm1) - rpView(jn, lvlsm1)) * exnerTopIncVal +
        (rpView(jn, lvls)  - rthetaView(jn, lvlsm1)) * exnerLevelsIncView(jn, lvlsm1) ) *
         thetaView(jn, lvlsm1)) /
      (rpView(jn, lvls) - rpView(jn, lvlsm1));
  }
}


/// \details This calculates air temperature increments.
void evalAirTemperatureAD(atlas::FieldSet & hatFlds, const atlas::FieldSet & augStateFlds) {
  const auto rpView = make_view<const double, 2>(augStateFlds["height_levels"]);
  const auto rthetaView = make_view<const double, 2>(augStateFlds["height"]);
  const auto exnerLevelsView = make_view<const double, 2>(augStateFlds["exner_levels_minus_one"]);
  const auto thetaView = make_view<const double, 2>(augStateFlds["theta"]);
  auto exnerLevelsHatView = make_view<double, 2>(hatFlds["exner_levels_minus_one"]);
  auto thetaHatView = make_view<double, 2>(hatFlds["theta"]);
  auto tHatView = make_view<double, 2>(hatFlds["air_temperature"]);

  atlas::idx_t lvls(hatFlds["air_temperature"].levels());
  atlas::idx_t lvlsm1 = lvls - 1;
  double exnerTopVal(0);
  double exnerTopHatVal(0);

  for (atlas::idx_t jn = 0; jn < tHatView.shape(0); ++jn) {
    // Value above model top is assumed to be in hydrostatic balance.
    exnerTopVal = exnerLevelsView(jn, lvlsm1) -
      (constants::grav * (rpView(jn, lvls) - rpView(jn, lvlsm1))) /
      (constants::cp * thetaView(jn, lvlsm1));

    thetaHatView(jn, lvlsm1) += ( (rthetaView(jn, lvlsm1) - rpView(jn, lvlsm1)) * exnerTopVal +
      (rpView(jn, lvls)  - rthetaView(jn, lvlsm1)) * exnerLevelsView(jn, lvlsm1) ) *
      tHatView(jn, lvlsm1) /
      (rpView(jn, lvls) - rpView(jn, lvlsm1));

    exnerTopHatVal = (rthetaView(jn, lvlsm1) - rpView(jn, lvlsm1)) *
      tHatView(jn, lvlsm1) /
      (rpView(jn, lvls) - rpView(jn, lvlsm1));

    exnerLevelsHatView(jn, lvlsm1) += (rpView(jn, lvls)  - rthetaView(jn, lvlsm1)) *
      tHatView(jn, lvlsm1) /
      (rpView(jn, lvls) - rpView(jn, lvlsm1));

    tHatView(jn, lvlsm1) = 0.0;

    tHatView(jn, lvlsm1) = (rthetaView(jn, lvlsm1) - rpView(jn, lvlsm1)) *
      exnerTopHatVal /
      (rpView(jn, lvls) - rpView(jn, lvlsm1));

    exnerTopHatVal = 0.0;

    for (atlas::idx_t jl = lvls - 2; jl >= 0; --jl) {
      thetaHatView(jn, jl) += ( (rthetaView(jn, jl) - rpView(jn, jl)) * exnerTopVal +
        (rpView(jn, jl + 1) - rthetaView(jn, jl)) * exnerLevelsView(jn, jl) ) *
      tHatView(jn, jl) /
       (rpView(jn, jl + 1) - rpView(jn, jl));

      exnerLevelsHatView(jn, jl + 1) += (rthetaView(jn, jl) - rpView(jn, jl)) *
        tHatView(jn, jl) /
        (rpView(jn, jl + 1) - rpView(jn, jl));

      exnerLevelsHatView(jn, jl) += (rpView(jn, jl + 1)  - rthetaView(jn, jl)) *
        tHatView(jn, jl) /
        (rpView(jn, jl + 1) - rpView(jn, jl));

      tHatView(jn, jl) = 0.0;
    }
  }
}


void qqclqcf2qtTL(atlas::FieldSet & incFields, const atlas::FieldSet &) {
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

void qqclqcf2qtAD(atlas::FieldSet & hatFields, const atlas::FieldSet &) {
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

void qtTemperature2qqclqcfTL(atlas::FieldSet & incFlds,
                             const atlas::FieldSet & augStateFlds) {
  const auto qsatView = make_view<const double, 2>(augStateFlds["qsat"]);
  const auto dlsvpdTView = make_view<const double, 2>(augStateFlds["dlsvpdT"]);
  const auto cleffView = make_view<const double, 2>(augStateFlds["cleff"]);
  const auto cfeffView = make_view<const double, 2>(augStateFlds["cfeff"]);

  const auto qtIncView = make_view<const double, 2>(incFlds["qt"]);
  const auto temperIncView = make_view<const double, 2>(incFlds["air_temperature"]);
  auto qclIncView = make_view<double, 2>
                    (incFlds["mass_content_of_cloud_liquid_water_in_atmosphere_layer"]);
  auto qcfIncView = make_view<double, 2>
                    (incFlds["mass_content_of_cloud_ice_in_atmosphere_layer"]);
  auto qIncView = make_view<double, 2>(incFlds["specific_humidity"]);


  double maxCldInc;
  for (atlas::idx_t jn = 0; jn < incFlds["qt"].shape(0); ++jn) {
    for (atlas::idx_t jl = 0; jl < incFlds["qt"].levels(); ++jl) {
        maxCldInc = qtIncView(jn, jl) - qsatView(jn, jl) *
            dlsvpdTView(jn, jl) * temperIncView(jn, jl);
        qclIncView(jn, jl) = cleffView(jn, jl) * maxCldInc;
        qcfIncView(jn, jl) = cfeffView(jn, jl) * maxCldInc;
        qIncView(jn, jl) = qtIncView(jn, jl) - qclIncView(jn, jl) - qcfIncView(jn, jl);
    }
  }
}

void qtTemperature2qqclqcfAD(atlas::FieldSet & hatFlds,
                             const atlas::FieldSet & augStateFlds) {
  const auto qsatView = make_view<const double, 2>(augStateFlds["qsat"]);
  const auto dlsvpdTView = make_view<const double, 2>(augStateFlds["dlsvpdT"]);
  const auto cleffView = make_view<const double, 2>(augStateFlds["cleff"]);
  const auto cfeffView = make_view<const double, 2>(augStateFlds["cfeff"]);

  auto temperHatView = make_view<double, 2>(hatFlds["air_temperature"]);
  auto qtHatView = make_view<double, 2>(hatFlds["qt"]);
  auto qHatView = make_view<double, 2>(hatFlds["specific_humidity"]);
  auto qclHatView = make_view<double, 2>
                    (hatFlds["mass_content_of_cloud_liquid_water_in_atmosphere_layer"]);
  auto qcfHatView = make_view<double, 2>
                    (hatFlds["mass_content_of_cloud_ice_in_atmosphere_layer"]);

  double qsatdlsvpdT;
  for (atlas::idx_t jn = 0; jn < hatFlds["qt"].shape(0); ++jn) {
    for (atlas::idx_t jl = 0; jl < hatFlds["qt"].levels(); ++jl) {
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
