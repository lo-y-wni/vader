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
#include "vader/recipes/AirVirtualTemperature.h"
#include "vader/vadervariables.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char AirVirtualTemperature_A::Name[] = "AirVirtualTemperature_A";
const std::vector<std::string> AirVirtualTemperature_A::Ingredients = {VV_TS, VV_Q};
const double default_epsilon = 0.62196;

// Register the maker
static RecipeMaker<AirVirtualTemperature_A> makerTempToVTemp_(AirVirtualTemperature_A::Name);

AirVirtualTemperature_A::AirVirtualTemperature_A(const Parameters_ &params) :
    epsilon_{params.epsilon.value()}
{
    oops::Log::trace() << "AirVirtualTemperature_A::AirVirtualTemperature_A(params)" << std::endl;
}

std::string AirVirtualTemperature_A::name() const
{
    return AirVirtualTemperature_A::Name;
}

std::string AirVirtualTemperature_A::product() const
{
    return "virtual_temperature";
}

std::vector<std::string> AirVirtualTemperature_A::ingredients() const
{
    return AirVirtualTemperature_A::Ingredients;
}

size_t AirVirtualTemperature_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field(VV_TS).levels();
}

atlas::FunctionSpace AirVirtualTemperature_A::productFunctionSpace
                                              (const atlas::FieldSet & afieldset) const
{
    return afieldset.field(VV_TS).functionspace();
}

bool AirVirtualTemperature_A::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering AirVirtualTemperature_A::executeNL function"
        << std::endl;

    atlas::Field temperature = afieldset.field(VV_TS);
    atlas::Field specific_humidity = afieldset.field(VV_Q);
    atlas::Field virtual_temperature = afieldset.field(VV_TV);

    auto temperature_view = atlas::array::make_view<double, 2>(temperature);
    auto specific_humidity_view = atlas::array::make_view<double, 2>(specific_humidity);
    auto virtual_temperature_view = atlas::array::make_view<double, 2>(virtual_temperature);

    size_t grid_size = temperature.shape(0);

    int nlevels = temperature.levels();
    for (int level = 0; level < nlevels; ++level) {
      for ( size_t jnode = 0; jnode < grid_size ; ++jnode ) {
        virtual_temperature_view(jnode, level) =
            temperature_view(jnode, level) *
            (1.0 + epsilon_ * specific_humidity_view(jnode, level));
      }
    }

    oops::Log::trace() << "leaving AirVirtualTemperature_A::executeNL function" << std::endl;

    return true;
}

bool AirVirtualTemperature_A::executeTL(atlas::FieldSet & afieldsetTL,
                                        const atlas::FieldSet & afieldsetTraj)
{
    bool tl_virtual_temperature_filled = false;

    oops::Log::trace() << "entering AirVirtualTemperature_A::executeTL function"
        << std::endl;

    atlas::Field traj_temperature = afieldsetTraj.field(VV_TS);
    atlas::Field traj_specific_humidity = afieldsetTraj.field(VV_Q);
    atlas::Field tl_temperature = afieldsetTL.field(VV_TS);
    atlas::Field tl_specific_humidity = afieldsetTL.field(VV_Q);
    atlas::Field tl_virtual_temperature = afieldsetTL.field(VV_TV);

    auto tl_temperature_view = atlas::array::make_view<double, 2>(tl_temperature);
    auto traj_temperature_view = atlas::array::make_view<double, 2>(traj_temperature);
    auto traj_specific_humidity_view = atlas::array::make_view<double, 2>(traj_specific_humidity);
    auto tl_specific_humidity_view = atlas::array::make_view<double, 2>(tl_specific_humidity);
    auto tl_virtual_temperature_view = atlas::array::make_view<double, 2>(tl_virtual_temperature);

    size_t grid_size = tl_specific_humidity.shape(0);

    int nlevels = tl_temperature.levels();
    for (int level = 0; level < nlevels; ++level) {
      for ( size_t jnode = 0; jnode < grid_size ; ++jnode ) {
        tl_virtual_temperature_view(jnode, level) =
            tl_temperature_view(jnode, level) *
            (1.0 + epsilon_ * traj_specific_humidity_view(jnode, level)) +
            (traj_temperature_view(jnode, level) * epsilon_ *
             tl_specific_humidity_view(jnode, level));
      }
    }

    tl_virtual_temperature_filled = true;

    oops::Log::trace() << "leaving AirVirtualTemperature_A::executeTL function" << std::endl;

    return tl_virtual_temperature_filled;
}

bool AirVirtualTemperature_A::executeAD(atlas::FieldSet & afieldsetAD,
                                        const atlas::FieldSet & afieldsetTraj)
{
    bool adjointSuccess = false;

    oops::Log::trace() << "entering AirVirtualTemperature_A::executeAD function"
        << std::endl;

    atlas::Field traj_temperature = afieldsetTraj.field(VV_TS);
    atlas::Field traj_specific_humidity = afieldsetTraj.field(VV_Q);
    atlas::Field ad_temperature = afieldsetAD.field(VV_TS);
    atlas::Field ad_specific_humidity = afieldsetAD.field(VV_Q);
    atlas::Field ad_virtual_temperature = afieldsetAD.field(VV_TV);

    auto ad_temperature_view = atlas::array::make_view<double, 2>(ad_temperature);
    auto traj_temperature_view = atlas::array::make_view<double, 2>(traj_temperature);
    auto traj_specific_humidity_view = atlas::array::make_view<double, 2>(traj_specific_humidity);
    auto ad_specific_humidity_view = atlas::array::make_view<double, 2>(ad_specific_humidity);
    auto ad_virtual_temperature_view = atlas::array::make_view<double, 2>(ad_virtual_temperature);

    size_t grid_size = ad_specific_humidity.shape(0);

    int nlevels = ad_temperature.levels();
    for (int level = 0; level < nlevels; ++level) {
      for ( size_t jnode = 0; jnode < grid_size ; ++jnode ) {
        ad_temperature_view(jnode, level) += ad_virtual_temperature_view(jnode, level) *
            (1.0 + epsilon_ * traj_specific_humidity_view(jnode, level));

        ad_specific_humidity_view(jnode, level) +=
            ad_virtual_temperature_view(jnode, level) * epsilon_ *
            traj_temperature_view(jnode, level);

        ad_virtual_temperature_view(jnode, level) = 0.0;
      }
    }

    adjointSuccess = true;

    oops::Log::trace() << "leaving AirVirtualTemperature_A::executeAD function" << std::endl;

    return adjointSuccess;
}

}  // namespace vader
