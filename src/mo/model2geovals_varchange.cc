
/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */



#include <math.h>
#include <string>
#include <vector>

#include "atlas/array.h"
#include "atlas/functionspace.h"

#include "mo/model2geovals_varchange.h"
#include "mo/utils.h"

#include "oops/util/Logger.h"


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
bool evalRatioToMt(atlas::FieldSet & fields)
{
  oops::Log::trace() << "[evalRatioToMt()] starting ..." << std::endl;

  // fields[0] = m_x = [ mv | mci | mcl | m_r ]
  auto ds_m_x  = atlas::array::make_view<double, 2>(fields[0]);
  auto ds_m_t  = atlas::array::make_view<double, 2>(fields[1]);
  auto ds_tfield  = atlas::array::make_view<double, 2>(fields[2]);

  auto fspace = fields[1].functionspace();

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

  std::vector<std::string> fnames {"m_v", "m_t", "q"};
  checkFieldSetContent(fields, fnames);

  bool rvalue = evalRatioToMt(fields);

  oops::Log::trace() << "[evalSpecificHumidity()] ... exit" << std::endl;

  return rvalue;
}


bool evalMassCloudIce(atlas::FieldSet & fields)
{
  oops::Log::trace() << "[evalMassCloudIce()] starting ..." << std::endl;

  std::vector<std::string> fnames {"m_ci", "m_t", "qci"};
  checkFieldSetContent(fields, fnames);

  bool rvalue = evalRatioToMt(fields);

  oops::Log::trace() << "[evalMassCloudIce()] ... exit" << std::endl;

  return rvalue;
}


bool evalMassCloudLiquid(atlas::FieldSet & fields)
{
  oops::Log::trace() << "[evalMassCloudLiquid()] starting ..." << std::endl;

  std::vector<std::string> fnames {"m_cl", "m_t", "qcl"};
  checkFieldSetContent(fields, fnames);

  bool rvalue = evalRatioToMt(fields);

  oops::Log::trace() << "[evalMassCloudLiquid()] ... exit" << std::endl;

  return rvalue;
}


bool evalMassRain(atlas::FieldSet & fields)
{
  oops::Log::trace() << "[evalMassRain()] starting ..." << std::endl;

  std::vector<std::string> fnames {"m_r", "m_t", "qrain"};
  checkFieldSetContent(fields, fnames);

  bool rvalue = evalRatioToMt(fields);

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


}  // namespace mo
