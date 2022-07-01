
/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */



#include <cmath>
#include <string>
#include <vector>

#include "atlas/array.h"
#include "atlas/functionspace.h"

#include "mo/model2geovals_varchange.h"
#include "mo/utils.h"

#include "oops/util/Logger.h"

using atlas::array::make_view;

namespace mo {


void initField_rank2(atlas::Field & field, const double value_init)
{
  setUniformValue_rank2(field, value_init);
}


void setUniformValue_rank2(atlas::Field & field, const double value)
{
  auto ds_view = atlas::array::make_view<double, 2>(field);

  ds_view.assign(value);
}


bool evalTotalMassMoistAir(atlas::FieldSet & fields)
{
  oops::Log::trace() << "[evalTotalMassMoistAir()] starting ..." << std::endl;

  std::vector<std::string> fnames {"m_v", "m_ci", "m_cl", "m_r", "m_t"};
  checkFieldSetContent(fields, fnames);

  auto ds_m_v  = atlas::array::make_view<double, 2>(fields["m_v"]);
  auto ds_m_ci = atlas::array::make_view<double, 2>(fields["m_ci"]);
  auto ds_m_cl = atlas::array::make_view<double, 2>(fields["m_cl"]);
  auto ds_m_r  = atlas::array::make_view<double, 2>(fields["m_r"]);
  auto ds_m_t  = atlas::array::make_view<double, 2>(fields["m_t"]);

  auto fspace = fields["m_t"].functionspace();

  auto evaluateMt = [&] (atlas::idx_t i, atlas::idx_t j) {
    ds_m_t(i, j) = 1 + ds_m_v(i, j) + ds_m_ci(i, j) + ds_m_cl(i, j) + ds_m_r(i, j); };

  auto conf = atlas::util::Config("levels", fields["m_t"].levels()) |
              atlas::util::Config("include_halo", true);

  parallelFor(fspace, evaluateMt, conf);

  oops::Log::trace() << "[evalTotalMassMoistAir()] ... exit" << std::endl;

  return true;
}

/// \brief function to evaluate the quantity:
///   qx = m_x/m_t
/// where ...
///   m_x = [ mv | mci | mcl | m_r ]
///   m_t  = total mass of moist air
///
bool evalRatioToMt(atlas::FieldSet & fields, const std::vector<std::string> & vars)
{
  oops::Log::trace() << "[evalRatioToMt()] starting ..." << std::endl;

  // fields[0] = m_x = [ mv | mci | mcl | m_r ]
  auto ds_m_x  = atlas::array::make_view<double, 2>(fields[vars[0]]);
  auto ds_m_t  = atlas::array::make_view<double, 2>(fields[vars[1]]);
  auto ds_tfield  = atlas::array::make_view<double, 2>(fields[vars[2]]);

  auto fspace = fields[vars[1]].functionspace();

  auto evaluateRatioToMt = [&] (atlas::idx_t i, atlas::idx_t j) {
    ds_tfield(i, j) = ds_m_x(i, j) / ds_m_t(i, j); };

  auto conf = atlas::util::Config("levels", fields[1].levels()) |
              atlas::util::Config("include_halo", true);

  parallelFor(fspace, evaluateRatioToMt, conf);

  oops::Log::trace() << "[evalRatioToMt()] ... exit" << std::endl;

  return true;
}


bool evalSpecificHumidity(atlas::FieldSet & fields)
{
  oops::Log::trace() << "[evalSpecificHumidity()] starting ..." << std::endl;

  std::vector<std::string> fnames {"m_v", "m_t", "specific_humidity"};
  checkFieldSetContent(fields, fnames);

  bool rvalue = evalRatioToMt(fields, fnames);

  oops::Log::trace() << "[evalSpecificHumidity()] ... exit" << std::endl;

  return rvalue;
}

bool evalRelativeHumidity(atlas::FieldSet & fields)
{
  oops::Log::trace() << "[evalRelativeHumidity()] starting ..." << std::endl;

  std::vector<std::string> fnames {"specific_humidity",
                                   "qsat", "relative_humidity"};
  checkFieldSetContent(fields, fnames);

  auto rhView = atlas::array::make_view<double, 2>(fields["relative_humidity"]);
  auto qView = atlas::array::make_view<double, 2>(fields["specific_humidity"]);
  auto qsatView = atlas::array::make_view<double, 2>(fields["qsat"]);

  auto conf = atlas::util::Config("levels", fields["relative_humidity"].levels()) |
              atlas::util::Config("include_halo", true);

  auto evaluateRH = [&] (atlas::idx_t i, atlas::idx_t j) {
    rhView(i, j) = qView(i, j)/qsatView(i, j) * 100.0;
    if (rhView(i, j) < 0.0) {rhView(i, j) = 0.0;} };

  auto fspace = fields["relative_humidity"].functionspace();

  parallelFor(fspace, evaluateRH, conf);

  oops::Log::trace() << "[evalRelativeHumidity()] ... exit" << std::endl;

  return true;
}

bool evalTotalRelativeHumidity(atlas::FieldSet & fields)
{
  oops::Log::trace() << "[evalTotalRelativeHumidity()] starting ..." << std::endl;

  std::vector<std::string> fnames {"specific_humidity",
                                   "mass_content_of_cloud_liquid_water_in_atmosphere_layer",
                                   "mass_content_of_cloud_ice_in_atmosphere_layer", "qrain",
                                   "qsat", "rht"};
  checkFieldSetContent(fields, fnames);

  auto rhtView = atlas::array::make_view<double, 2>(fields["rht"]);
  auto qView = atlas::array::make_view<double, 2>(fields["specific_humidity"]);
  auto qclView = atlas::array::make_view<double, 2>
                 (fields["mass_content_of_cloud_liquid_water_in_atmosphere_layer"]);
  auto qciView = atlas::array::make_view<double, 2>
                 (fields["mass_content_of_cloud_ice_in_atmosphere_layer"]);
  auto qrainView = atlas::array::make_view<double, 2>(fields["qrain"]);
  auto qsatView = atlas::array::make_view<double, 2>(fields["qsat"]);

  auto conf = atlas::util::Config("levels", fields["rht"].levels()) |
              atlas::util::Config("include_halo", true);

  auto evaluateRHT = [&] (atlas::idx_t i, atlas::idx_t j) {
    rhtView(i, j) = (qView(i, j)+qclView(i, j)+qciView(i, j)+qrainView(i, j))/qsatView(i, j)
                 * 100.0;
    if (rhtView(i, j) < 0.0) {rhtView(i, j) = 0.0;} };

  auto fspace = fields["rht"].functionspace();

  parallelFor(fspace, evaluateRHT, conf);

  oops::Log::trace() << "[evalTotalRelativeHumidity()] ... exit" << std::endl;

  return true;
}

bool evalMassCloudIce(atlas::FieldSet & fields)
{
  oops::Log::trace() << "[evalMassCloudIce()] starting ..." << std::endl;

  std::vector<std::string> fnames {"m_ci", "m_t",
                                   "mass_content_of_cloud_ice_in_atmosphere_layer"};
  checkFieldSetContent(fields, fnames);

  bool rvalue = evalRatioToMt(fields, fnames);

  oops::Log::trace() << "[evalMassCloudIce()] ... exit" << std::endl;

  return rvalue;
}


bool evalMassCloudLiquid(atlas::FieldSet & fields)
{
  oops::Log::trace() << "[evalMassCloudLiquid()] starting ..." << std::endl;

  std::vector<std::string> fnames {"m_cl", "m_t",
                                   "mass_content_of_cloud_liquid_water_in_atmosphere_layer"};
  checkFieldSetContent(fields, fnames);

  bool rvalue = evalRatioToMt(fields, fnames);

  oops::Log::trace() << "[evalMassCloudLiquid()] ... exit" << std::endl;

  return rvalue;
}


bool evalMassRain(atlas::FieldSet & fields)
{
  oops::Log::trace() << "[evalMassRain()] starting ..." << std::endl;

  std::vector<std::string> fnames {"m_r", "m_t", "qrain"};
  checkFieldSetContent(fields, fnames);

  bool rvalue = evalRatioToMt(fields, fnames);

  oops::Log::trace() << "[evalMassRain()] ... exit" << std::endl;

  return rvalue;
}


bool evalAirTemperature(atlas::FieldSet & fields)
{
  oops::Log::trace() << "[evalAirTemperature()] starting ..." << std::endl;

  std::vector<std::string> fnames {"theta", "exner", "air_temperature"};
  checkFieldSetContent(fields, fnames);

  auto ds_theta  = atlas::array::make_view<double, 2>(fields["theta"]);
  auto ds_exner = atlas::array::make_view<double, 2>(fields["exner"]);
  auto ds_atemp = atlas::array::make_view<double, 2>(fields["air_temperature"]);

  auto fspace = fields["air_temperature"].functionspace();

  auto evaluateAirTemp = [&] (atlas::idx_t i, atlas::idx_t j) {
    ds_atemp(i, j) = ds_theta(i, j) * ds_exner(i, j); };

  auto conf = atlas::util::Config("levels", fields["air_temperature"].levels()) |
              atlas::util::Config("include_halo", true);

  parallelFor(fspace, evaluateAirTemp, conf);

  oops::Log::trace() << "[evalAirTemperature()] ... exit" << std::endl;

  return true;
}


bool evalPressureLevelsMinusOne(atlas::FieldSet & fields)
{
  oops::Log::trace() << "[evalPressureLevelsMinusOne()] starting ..." << std::endl;

  std::vector<std::string> fnames {"exner_levels", "pressure_levels_minus_one"};
  checkFieldSetContent(fields, fnames);

  auto ds_el  = atlas::array::make_view<double, 2>(fields["exner_levels"]);
  auto ds_plmo = atlas::array::make_view<double, 2>(fields["pressure_levels_minus_one"]);

  auto fspace = fields["pressure_levels_minus_one"].functionspace();

  auto evaluatePresLevelsMinusOne = [&] (atlas::idx_t i, atlas::idx_t j) {
    ds_plmo(i, j) = Constants::p_zero * pow(ds_el(i, j), (1/Constants::rd_over_cp)); };

  auto conf = atlas::util::Config("levels", fields["pressure_levels_minus_one"].levels()) |
              atlas::util::Config("include_halo", true);

  parallelFor(fspace, evaluatePresLevelsMinusOne, conf);

  oops::Log::trace() << "[evalPressureLevelsMinusOne()] ... exit" << std::endl;

  return true;
}


bool evalSpecificHumidityFromRH_2m(atlas::FieldSet & fields)
{
  oops::Log::trace() << "[evalSpecificHumidityFromRH_2m()] starting ..." << std::endl;

  std::vector<std::string> fnames {"qsat",
                                   "relative_humidity_2m",
                                   "specific_humidity_at_two_meters_above_surface"};
  checkFieldSetContent(fields, fnames);

  auto ds_qsat = atlas::array::make_view<double, 2>(fields["qsat"]);
  auto ds_rh = atlas::array::make_view<double, 2>(fields["relative_humidity_2m"]);
  auto ds_q2m = atlas::array::make_view<double, 2>(
    fields["specific_humidity_at_two_meters_above_surface"]);

  auto fspace = fields["specific_humidity_at_two_meters_above_surface"].functionspace();

  auto evaluateSpecificHumidity_2m = [&] (atlas::idx_t i, atlas::idx_t j) {
    ds_q2m(i, j) = ds_rh(i, j) * ds_qsat(i, j); };

  auto conf = atlas::util::Config("levels",
    fields["specific_humidity_at_two_meters_above_surface"].levels()) |
              atlas::util::Config("include_halo", true);

  parallelFor(fspace, evaluateSpecificHumidity_2m, conf);

  oops::Log::trace() << "[evalSpecificHumidityFromRH_2m()] ... exit" << std::endl;

  return true;
}


bool evalParamAParamB(atlas::FieldSet & fields)
{
  oops::Log::trace() << "[evalParamAParamB2()] starting ..." << std::endl;

  std::vector<std::string> fnames {"height", "height_levels",
                                   "air_pressure_levels_minus_one",
                                   "specific_humidity",
                                   "param_a", "param_b"};
  checkFieldSetContent(fields, fnames);

  std::size_t blindex;
  if (!fields["height"].metadata().has("boundary_layer_index")) {
    oops::Log::error() << "ERROR - data validation failed "
                          "we expect boundary_layer_index value "
                          "in the meta data of the height field" << std::endl;
  }
  fields["height"].metadata().get("boundary_layer_index", blindex);

  auto heightView = make_view<const double, 2>(fields["height"]);
  auto heightLevelsView = make_view<const double, 2>(fields["height_levels"]);
  auto pressureLevelsView = make_view<const double, 2>(fields["air_pressure_levels_minus_one"]);
  auto specificHumidityView = make_view<const double, 2>(fields["specific_humidity"]);
  auto param_aView = make_view<double, 2>(fields["param_a"]);
  auto param_bView = make_view<double, 2>(fields["param_b"]);

  // temperature at level above boundary layer
  double t_bl;
  // temperature at model surface height
  double t_msh;

  double exp_pmsh = mo::Constants::Lclr * mo::Constants::rd / mo::Constants::grav;

  for (atlas::idx_t jn = 0; jn < param_aView.shape(0); ++jn) {
    t_bl = (-mo::Constants::grav / mo::Constants::rd) *
           (heightLevelsView(jn, blindex + 1) - heightLevelsView(jn, blindex)) /
           log(pressureLevelsView(jn, blindex + 1) / pressureLevelsView(jn, blindex));

    t_bl = t_bl / (1.0 + mo::Constants::c_virtual * specificHumidityView(jn, blindex));

    t_msh = t_bl + mo::Constants::Lclr * (heightView(jn, blindex) - heightLevelsView(jn, 0));

    param_aView(jn, 0) = heightLevelsView(jn, 0) + t_msh / mo::Constants::Lclr;
    param_bView(jn, 0) = t_msh / (pow(pressureLevelsView(jn, 0), exp_pmsh) * mo::Constants::Lclr);
  }

  oops::Log::trace() << "[evalParamAParamB()] ... exit" << std::endl;

  return true;
}

}  // namespace mo
