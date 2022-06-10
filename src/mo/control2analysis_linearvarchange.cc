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
#include "atlas/field/FieldSet.h"

namespace mo {

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
