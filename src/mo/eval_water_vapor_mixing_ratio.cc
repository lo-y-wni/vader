/*
 * (C) Crown Copyright 2023 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <string>
#include <vector>

#include "atlas/array/MakeView.h"
#include "atlas/field.h"
#include "atlas/parallel/omp/omp.h"

#include "mo/eval_ratio.h"
#include "mo/eval_water_vapor_mixing_ratio.h"

#include "oops/util/Logger.h"

namespace mo {

using atlas::array::make_view;

// --------------------------------------------------------------------------------------

bool eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_nl(
        atlas::FieldSet & stateFlds) {
  oops::Log::trace()
          << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_nl()] starting ..."
          << std::endl;

  std::vector<std::string> fnames {"m_v", "m_t",
                                   "specific_humidity"};

  const bool rvalue = eval_ratio_to_second(stateFlds, fnames);

  oops::Log::trace()
          << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_nl()] ... exit"
          << std::endl;
  return rvalue;
}

// --------------------------------------------------------------------------------------

void eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_tl(
        atlas::FieldSet & incFlds,
        const atlas::FieldSet & stateFlds) {
  oops::Log::trace()
          << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_tl()] starting ..."
          << std::endl;

  std::vector<std::string> fnames {"m_v", "m_t",
                                   "specific_humidity"};

  eval_ratio_to_second_tl(incFlds, stateFlds, fnames);

  oops::Log::trace()
          << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_tl()] ... exit"
          << std::endl;
}
// --------------------------------------------------------------------------------------

void eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_ad(
        atlas::FieldSet & hatFlds,
        const atlas::FieldSet & stateFlds) {
  oops::Log::trace()
          << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_ad()] starting ..."
          << std::endl;

  std::vector<std::string> fnames {"m_v", "m_t",
                                   "specific_humidity"};

  eval_ratio_to_second_ad(hatFlds, stateFlds, fnames);

  oops::Log::trace()
          << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_ad()] ... exit"
          << std::endl;
}

// --------------------------------------------------------------------------------------

bool eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m_nl(
        atlas::FieldSet & stateFlds) {
  oops::Log::trace()
          << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m_nl()]"
          << " starting ..." << std::endl;

  // TODO(Mayeul) remove percent if definition of relative_humidity changes.
  constexpr double percent = 0.01;
  const auto ds_qsat = make_view<const double, 2>(stateFlds["qsat"]);
  const auto ds_rh = make_view<const double, 2>(stateFlds["relative_humidity_2m"]);
  auto ds_q2m = make_view<double, 2>(stateFlds["specific_humidity_at_two_meters_above_surface"]);

  atlas_omp_parallel_for(atlas::idx_t jnode = 0; jnode < ds_q2m.shape(0); jnode++) {
    for (atlas::idx_t jlev = 0; jlev < ds_q2m.shape(1); jlev++) {
      ds_q2m(jnode, jlev) = percent * ds_rh(jnode, jlev) * ds_qsat(jnode, jlev);
    }
  }

  oops::Log::trace()
         << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m_nl()] ... exit"
         << std::endl;
  return true;
}

// --------------------------------------------------------------------------------------

void eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m_tl(
        atlas::FieldSet & incFlds,
        const atlas::FieldSet & stateFlds) {
  oops::Log::trace()
          << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m_tl()]"
          << " starting ..." << std::endl;

  // TODO(Mayeul) remove percent if definition of relative_humidity changes.
  constexpr double percent = 0.01;
  const auto qsat_inc = make_view<const double, 2>(incFlds["qsat"]);
  const auto rh_inc = make_view<const double, 2>(incFlds["relative_humidity_2m"]);
  auto q2m_inc = make_view<double, 2>(incFlds["specific_humidity_at_two_meters_above_surface"]);
  const auto qsat = make_view<const double, 2>(stateFlds["qsat"]);
  const auto rh = make_view<const double, 2>(stateFlds["relative_humidity_2m"]);

  atlas_omp_parallel_for(atlas::idx_t jnode = 0; jnode < q2m_inc.shape(0); jnode++) {
    q2m_inc(jnode, 0) = percent * (rh_inc(jnode, 0) * qsat(jnode, 0)
                                      + rh(jnode, 0) * qsat_inc(jnode, 0));
  }

  oops::Log::trace()
         << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m_tl()] ... exit"
         << std::endl;
}
// --------------------------------------------------------------------------------------

void eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m_ad(
        atlas::FieldSet & hatFlds,
        const atlas::FieldSet & stateFlds) {
  oops::Log::trace()
          << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m_ad()]"
          << " starting ..." << std::endl;

  // TODO(Mayeul) remove percent if definition of relative_humidity changes.
  constexpr double percent = 0.01;
  auto qsat_hat = make_view<double, 2>(hatFlds["qsat"]);
  auto rh_hat = make_view<double, 2>(hatFlds["relative_humidity_2m"]);
  auto q2m_hat = make_view<double, 2>(hatFlds["specific_humidity_at_two_meters_above_surface"]);
  const auto qsat = make_view<const double, 2>(stateFlds["qsat"]);
  const auto rh = make_view<const double, 2>(stateFlds["relative_humidity_2m"]);

  atlas_omp_parallel_for(atlas::idx_t jnode = 0; jnode < qsat.shape(0); jnode++) {
    rh_hat(jnode, 0) += percent * qsat(jnode, 0) * q2m_hat(jnode, 0);
    qsat_hat(jnode, 0) += percent * rh(jnode, 0) * q2m_hat(jnode, 0);
    q2m_hat(jnode, 0) = 0.0;
  }

  oops::Log::trace()
          << "[eval_water_vapor_mixing_ratio_wrt_moist_air_and_condensed_water_at_2m_ad()] ... exit"
          << std::endl;
}

// --------------------------------------------------------------------------------------
}  // namespace mo
