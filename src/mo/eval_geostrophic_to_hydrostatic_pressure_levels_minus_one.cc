/*
 * (C) Crown Copyright 2023-2024 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include "atlas/array.h"
#include "atlas/field.h"

#include "mo/constants.h"
#include "mo/eval_geostrophic_to_hydrostatic_pressure_levels_minus_one.h"

#include "oops/util/FunctionSpaceHelpers.h"
#include "oops/util/Logger.h"

using atlas::array::make_view;
using atlas::idx_t;

namespace mo {

// ------------------------------------------------------------------------------------------------
// Note this is a copy of evalHydrostaticPressureTL
void eval_hydrostatic_pressure_levels_minus_one_tl(atlas::FieldSet & incFlds,
                                                   const atlas::FieldSet & augStateFlds) {
  oops::Log::trace()
    << "[eval_hydrostatic_pressure_levels_minus_one_tl()] starting ..." << std::endl;
  const auto gPIncView = make_view<const double, 2>(
    incFlds["geostrophic_pressure_levels_minus_one"]);
  const auto uPIncView = make_view<const double, 2>(
    incFlds["unbalanced_pressure_levels_minus_one"]);
  auto hPIncView = make_view<double, 2>(incFlds["hydrostatic_pressure_levels_minus_one"]);

  const idx_t sizeOwned =
    util::getSizeOwned(incFlds["geostrophic_pressure_levels_minus_one"].functionspace());
  const idx_t levels = incFlds["geostrophic_pressure_levels_minus_one"].shape(1);

  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    for (idx_t jl = 0; jl < levels; ++jl) {
      hPIncView(jn, jl) = uPIncView(jn, jl);
    }
  }

  if (augStateFlds.has("interpolation_weights")) {
    // Bins Vertical regression matrix stored in one field
    // B = (vertical regression matrix bin_0)
    //     (vertical regression matrix bin_1)
    //     (          ...                   )
    //     (vertical regression matrix bin_m)
    // Since each matrix is square we can easily infer the bin index from the row index
    // First index of vertRegView is bin_index * number of levels + level index,
    // the second is number of levels associated with matrix column.
    const auto vertRegView = make_view<const double, 2>(
      augStateFlds["vertical_regression_matrices"]);
    const auto interpWeightView = make_view<const double, 2>(
      augStateFlds["interpolation_weights"]);
    // First index of interpWeightView is horizontal index, the second is bin index here
    const idx_t nBins = augStateFlds["interpolation_weights"].shape(1);

    for (idx_t jn = 0; jn < sizeOwned; ++jn) {
      for (idx_t jl = 0; jl < levels; ++jl) {
        for (idx_t b = 0; b < nBins; ++b) {
          for (idx_t jl2 = 0; jl2 < levels; ++jl2) {
            hPIncView(jn, jl) += interpWeightView(jn, b) *
                                 vertRegView(b * levels + jl, jl2) *
                                 gPIncView(jn, jl2);
          }
        }
      }
    }
  }

  incFlds["hydrostatic_pressure_levels_minus_one"].set_dirty();

  oops::Log::trace()
    << "[eval_hydrostatic_pressure_levels_minus_one_tl()] ... done" << std::endl;
}

// ------------------------------------------------------------------------------------------------
// Note this is a copy of evalHydrostaticPressureAD
void eval_hydrostatic_pressure_levels_minus_one_ad(atlas::FieldSet & hatFlds,
                                                   const atlas::FieldSet & augStateFlds) {
  oops::Log::trace()
    << "[eval_hydrostatic_pressure_levels_minus_one_ad()] starting ..." << std::endl;
  auto gPHatView = make_view<double, 2>(hatFlds["geostrophic_pressure_levels_minus_one"]);
  auto uPHatView = make_view<double, 2>(hatFlds["unbalanced_pressure_levels_minus_one"]);
  auto hPHatView = make_view<double, 2>(hatFlds["hydrostatic_pressure_levels_minus_one"]);

  const idx_t levels = hatFlds["geostrophic_pressure_levels_minus_one"].shape(1);
  const idx_t sizeOwned =
        util::getSizeOwned(hatFlds["geostrophic_pressure_levels_minus_one"].functionspace());

  if (augStateFlds.has("interpolation_weights")) {
    // Bins Vertical regression matrix stored in one field
    // B = (vertical regression matrix bin_0)
    //     (vertical regression matrix bin_1)
    //     (          ...                   )
    //     (vertical regression matrix bin_m)
    // Since each matrix is square we can easily infer the bin index from the row index
    // First index of vertRegView is bin_index * number of levels + level index,
    //     the second is level index
    const auto vertRegView = make_view<const double, 2>(
      augStateFlds["vertical_regression_matrices"]);
    const auto interpWeightView = make_view<const double, 2>(
      augStateFlds["interpolation_weights"]);
    const idx_t nBins = augStateFlds["interpolation_weights"].shape(1);

    for (idx_t jn = 0; jn < sizeOwned; ++jn) {
      for (idx_t jl = levels - 1; jl >= 0; --jl) {
        for (idx_t b = nBins -1; b >= 0; --b) {
          for (idx_t jl2 = levels - 1; jl2 >= 0; --jl2) {
            gPHatView(jn, jl2) += interpWeightView(jn, b) *
                                  vertRegView(b * levels + jl, jl2) *
                                  hPHatView(jn, jl);
          }
        }
        uPHatView(jn, jl) += hPHatView(jn, jl);
        hPHatView(jn, jl) = 0.0;
      }
    }
  }

  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    for (idx_t jl = levels - 1; jl >= 0; --jl) {
      uPHatView(jn, jl) += hPHatView(jn, jl);
      hPHatView(jn, jl) = 0.0;
    }
  }

  hatFlds["geostrophic_pressure_levels_minus_one"].set_dirty();
  hatFlds["unbalanced_pressure_levels_minus_one"].set_dirty();
  hatFlds["hydrostatic_pressure_levels_minus_one"].set_dirty();

  oops::Log::trace()
    << "[eval_hydrostatic_pressure_levels_minus_one_ad()] ... done" << std::endl;
}

// ------------------------------------------------------------------------------------------------
void eval_hydrostatic_pressure_levels_minus_one_tl_inv(atlas::FieldSet & incFlds,
                                                       const atlas::FieldSet & augStateFlds) {
  oops::Log::trace()
    << "[eval_hydrostatic_pressure_levels_minus_one_tl_inv()] starting ..." << std::endl;
  const auto gPIncView =
    make_view<const double, 2>(incFlds["geostrophic_pressure_levels_minus_one"]);
  const auto hPIncView =
    make_view<const double, 2>(incFlds["hydrostatic_pressure_levels_minus_one"]);
  auto uPIncView =
    make_view<double, 2>(incFlds["unbalanced_pressure_levels_minus_one"]);

  const idx_t levels = incFlds["unbalanced_pressure_levels_minus_one"].shape(1);
  const idx_t sizeOwned =
    util::getSizeOwned(incFlds["unbalanced_pressure_levels_minus_one"].functionspace());

  for (idx_t jn = 0; jn < sizeOwned; ++jn) {
    for (idx_t jl = 0; jl < levels; ++jl) {
      uPIncView(jn, jl) = hPIncView(jn, jl);
    }
  }

  if (augStateFlds.has("interpolation_weights")) {
    // Bins Vertical regression matrix stored in one field
    // B = (vertical regression matrix bin_0)
    //     (vertical regression matrix bin_1)
    //     (          ...                   )
    //     (vertical regression matrix bin_m)
    // Since each matrix is square we can easily infer the bin index from the row index
    // First index of vertRegView is bin_index * number of levels + level index,
    // the second is number of levels associated with matrix column.
    const auto vertRegView = make_view<const double, 2>(
      augStateFlds["vertical_regression_matrices"]);
    const auto interpWeightView = make_view<const double, 2>(
      augStateFlds["interpolation_weights"]);
    // First index of interpWeightView is horizontal index, the second is bin index here
    const idx_t nBins = augStateFlds["interpolation_weights"].shape(1);

    for (idx_t jn = 0; jn < sizeOwned; ++jn) {
      for (idx_t jl = 0; jl < levels; ++jl) {
        for (idx_t b = 0; b < nBins; ++b) {
          for (idx_t jl2 = 0; jl2 < levels; ++jl2) {
            uPIncView(jn, jl) -= interpWeightView(jn, b) *
                                 vertRegView(b * levels + jl, jl2) *
                                 gPIncView(jn, jl2);
          }
        }
      }
    }
  }

  incFlds["unbalanced_pressure_levels_minus_one"].set_dirty();

  oops::Log::trace()
    << "[eval_hydrostatic_pressure_levels_minus_one_tl_inv()] ... done" << std::endl;
}

}  // namespace mo
