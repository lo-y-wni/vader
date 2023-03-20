/*
 * (C) Copyright 2021-2022  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <cmath>
#include <iostream>
#include <vector>

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "atlas/util/Metadata.h"
#include "oops/util/Logger.h"
#include "vader/recipes/DryAirDensityLevelsMinusOne.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char DryAirDensityLevelsMinusOne_A::Name[] = "DryAirDensityLevelsMinusOne_A";
const std::vector<std::string> DryAirDensityLevelsMinusOne_A::
    Ingredients = {"height", "height_levels", "air_temperature", "air_pressure_levels_minus_one"};

// Register the maker
static RecipeMaker<DryAirDensityLevelsMinusOne_A>
    makerDryAirDensityLevelsMinusOne_A_(DryAirDensityLevelsMinusOne_A::Name);

DryAirDensityLevelsMinusOne_A::DryAirDensityLevelsMinusOne_A(const Parameters_ & params,
                                        const VaderConfigVars & configVariables) :
    rd_{params.rd.value()}
{
    oops::Log::trace() <<
        "DryAirDensityLevelsMinusOne_A::DryAirDensityLevelsMinusOne_A(params)" << std::endl;
}

std::string DryAirDensityLevelsMinusOne_A::name() const
{
    return DryAirDensityLevelsMinusOne_A::Name;
}

std::string DryAirDensityLevelsMinusOne_A::product() const
{
    return "dry_air_density_levels_minus_one";
}

std::vector<std::string> DryAirDensityLevelsMinusOne_A::ingredients() const
{
    return DryAirDensityLevelsMinusOne_A::Ingredients;
}

size_t DryAirDensityLevelsMinusOne_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset["air_pressure_levels_minus_one"].levels();
}

atlas::FunctionSpace DryAirDensityLevelsMinusOne_A::productFunctionSpace(const atlas::FieldSet
                                                                 & afieldset) const
{
    return afieldset.field("air_pressure_levels_minus_one").functionspace();
}

bool DryAirDensityLevelsMinusOne_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering DryAirDensityLevelsMinusOne_A::executeNL function"
      << std::endl;

    const auto height_levels_view = atlas::array::make_view<const double, 2>
                                    (afieldset["height_levels"]);
    const auto height_view = atlas::array::make_view<const double, 2>
                             (afieldset["height"]);
    const auto temperature_view = atlas::array::make_view<const double, 2>
                                  (afieldset["air_temperature"]);
    const auto pressure_view = atlas::array::make_view<const double, 2>
                         (afieldset["air_pressure_levels_minus_one"]);
    auto rho_view = atlas::array::make_view<double, 2>
                    (afieldset["dry_air_density_levels_minus_one"]);

    size_t grid_size = rho_view.shape(0);
    int nlevels = afieldset["dry_air_density_levels_minus_one"].levels();
    for ( size_t jnode = 0; jnode < grid_size ; ++jnode ) {
      rho_view(jnode, 0) = pressure_view(jnode, 0) / (rd_ * temperature_view(jnode, 0));
      for (int level = 1; level < nlevels; ++level) {
        double delta_h = height_view(jnode, level) - height_view(jnode, level-1);
        double delta_hhl = height_view(jnode, level) - height_levels_view(jnode, level);
        double delta_hlh = height_levels_view(jnode, level) - height_view(jnode, level-1);
        double temperature_hl = (delta_hhl * temperature_view(jnode, level-1) +
                                     delta_hlh * temperature_view(jnode, level)) / delta_h;
        rho_view(jnode, level) = pressure_view(jnode, level) /
          (rd_ * temperature_hl);
      }
    }
    oops::Log::trace() << "leaving DryAirDensityLevelsMinusOne_A::executeNL function" << std::endl;

    return true;
}

bool DryAirDensityLevelsMinusOne_A::executeTL(atlas::FieldSet & afieldsetTL,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering DryAirDensityLevelsMinusOne_A::executeTL function"
        << std::endl;

    atlas::Field traj_height = afieldsetTraj["height"];
    atlas::Field traj_height_levels = afieldsetTraj["height_levels"];

    atlas::Field traj_pressure = afieldsetTraj["air_pressure_levels_minus_one"];
    atlas::Field traj_temperature = afieldsetTraj["air_temperature"];

    atlas::Field tl_temperature = afieldsetTL["air_temperature"];
    atlas::Field tl_pressure = afieldsetTL["air_pressure_levels_minus_one"];
    atlas::Field tl_rho = afieldsetTL["dry_air_density_levels_minus_one"];

    const auto traj_height_view = atlas::array::make_view<const double, 2>(traj_height);
    const auto traj_height_levels_view = atlas::array::make_view<const double, 2>
                                         (traj_height_levels);
    const auto tl_temperature_view = atlas::array::make_view<const double, 2>(tl_temperature);
    const auto traj_temperature_view = atlas::array::make_view<const double, 2>(traj_temperature);
    const auto tl_pressure_view = atlas::array::make_view<const double, 2>(tl_pressure);
    const auto traj_pressure_view = atlas::array::make_view<const double, 2>(traj_pressure);
    auto tl_rho_view = atlas::array::make_view<double, 2>(tl_rho);

    double traj_rho;
    double delta_h;
    double delta_hhl;
    double delta_hlh;
    double traj_temperature_hl;

    size_t grid_size = tl_rho_view.shape(0);

    int nlevels = afieldsetTL["dry_air_density_levels_minus_one"].levels();
    for ( size_t jnode = 0; jnode < grid_size ; ++jnode ) {
      traj_temperature_hl = traj_temperature_view(jnode, 0);
      traj_rho = traj_pressure_view(jnode, 0) / (rd_ * traj_temperature_hl);
      tl_rho_view(jnode, 0) = traj_rho * (
        tl_pressure_view(jnode, 0) / traj_pressure_view(jnode, 0) -
        tl_temperature_view(jnode, 0) / traj_temperature_hl);
      for (int level = 1; level < nlevels; ++level) {
        delta_h = traj_height_view(jnode, level) - traj_height_view(jnode, level-1);
        delta_hhl = traj_height_view(jnode, level) - traj_height_levels_view(jnode, level);
        delta_hlh = traj_height_levels_view(jnode, level) - traj_height_view(jnode, level-1);
        // air temperature at height_levels
        traj_temperature_hl = (delta_hhl * traj_temperature_view(jnode, level-1) +
                                     delta_hlh * traj_temperature_view(jnode, level)) / delta_h;
        // dry air density
        traj_rho = traj_pressure_view(jnode, level) / (rd_ * traj_temperature_hl);

        tl_rho_view(jnode, level) = traj_rho * (tl_pressure_view(jnode, level)
                                    / traj_pressure_view(jnode, level)
                                    - (delta_hhl / delta_h) * tl_temperature_view(jnode, level-1)
                                    / traj_temperature_hl
                                    - (delta_hlh / delta_h) * tl_temperature_view(jnode, level)
                                    / traj_temperature_hl);
      }
    }

    oops::Log::trace() << "leaving DryAirDensityLevelsMinusOne_A::executeTL function" << std::endl;

    return true;
}

bool DryAirDensityLevelsMinusOne_A::executeAD(atlas::FieldSet & afieldsetAD,
                                      const atlas::FieldSet & afieldsetTraj)
{
    oops::Log::trace() << "entering DryAirDensityLevelsMinusOne::executeAD function"
        << std::endl;

    atlas::Field traj_height = afieldsetTraj["height"];
    atlas::Field traj_height_levels = afieldsetTraj["height_levels"];

    atlas::Field traj_pressure = afieldsetTraj["air_pressure_levels_minus_one"];
    atlas::Field traj_temperature = afieldsetTraj["air_temperature"];

    atlas::Field ad_temperature = afieldsetAD["air_temperature"];
    atlas::Field ad_pressure = afieldsetAD["air_pressure_levels_minus_one"];
    atlas::Field ad_rho = afieldsetAD["dry_air_density_levels_minus_one"];

    auto traj_height_view = atlas::array::make_view<double, 2>(traj_height);
    auto traj_height_levels_view = atlas::array::make_view<double, 2>(traj_height_levels);
    auto ad_temperature_view = atlas::array::make_view<double, 2>(ad_temperature);
    auto traj_temperature_view = atlas::array::make_view<double, 2>(traj_temperature);
    auto ad_pressure_view = atlas::array::make_view<double, 2>(ad_pressure);
    auto traj_pressure_view = atlas::array::make_view<double, 2>(traj_pressure);
    auto ad_rho_view = atlas::array::make_view<double, 2>(ad_rho);

    double traj_rho;
    double delta_h;
    double delta_hhl;
    double delta_hlh;
    double traj_temperature_hl;

    size_t grid_size = ad_rho.shape(0);
    int nlevels = afieldsetAD["dry_air_density_levels_minus_one"].levels();
    for ( size_t jnode = 0; jnode < grid_size ; ++jnode ) {
      for (int level = nlevels-1; level >= 1; --level) {
        delta_h = traj_height_view(jnode, level) - traj_height_view(jnode, level-1);
        delta_hhl = traj_height_view(jnode, level) - traj_height_levels_view(jnode, level);
        delta_hlh = traj_height_levels_view(jnode, level) - traj_height_view(jnode, level-1);
        // air temperature at height_levels
        traj_temperature_hl = (delta_hhl * traj_temperature_view(jnode, level-1) +
                               delta_hlh * traj_temperature_view(jnode, level)) / delta_h;
        // dry air density
        traj_rho = traj_pressure_view(jnode, level) / (rd_ * traj_temperature_hl);

        ad_pressure_view(jnode, level) += traj_rho * ad_rho_view(jnode, level)
                                          / traj_pressure_view(jnode, level);
        ad_temperature_view(jnode, level-1) -= (delta_hhl / delta_h) * traj_rho
                                               * ad_rho_view(jnode, level) / traj_temperature_hl;
        ad_temperature_view(jnode, level) -= (delta_hlh / delta_h) * traj_rho
                                             * ad_rho_view(jnode, level) / traj_temperature_hl;
        ad_rho_view(jnode, level) = 0.0;
      }
      traj_temperature_hl = traj_temperature_view(jnode, 0);
      traj_rho = traj_pressure_view(jnode, 0) / (rd_ * traj_temperature_hl);

      ad_pressure_view(jnode, 0) += traj_rho *
        ad_rho_view(jnode, 0) / traj_pressure_view(jnode, 0);
      ad_temperature_view(jnode, 0) -= traj_rho * ad_rho_view(jnode, 0) / traj_temperature_hl;
      ad_rho_view(jnode, 0) = 0.0;
    }

    oops::Log::trace() << "leaving DryAirDensityLevelsMinusOne_A::executeAD function" << std::endl;

    return true;
}

}  // namespace vader
