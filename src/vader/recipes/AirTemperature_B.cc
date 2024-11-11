/*
 * (C) Copyright 2022- UCAR.
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
#include "vader/recipes/AirTemperature.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char AirTemperature_B::Name[] = "AirTemperature_B";
const oops::Variables AirTemperature_B::Ingredients{std::vector<std::string>{
                                                        "virtual_temperature",
                                                        "water_vapor_mixing_ratio_wrt_moist_air"}};

// Register the maker
static RecipeMaker<AirTemperature_B> makerAirTemperature_B_(AirTemperature_B::Name);

AirTemperature_B::AirTemperature_B(const Parameters_ & params,
                                   const VaderConfigVars & configVariables) :
    configVariables_{configVariables}
{
    oops::Log::trace() << "AirTemperature_B::AirTemperature_B(params)" << std::endl;
}

std::string AirTemperature_B::name() const
{
    return AirTemperature_B::Name;
}

oops::Variable AirTemperature_B::product() const
{
    return oops::Variable{"air_temperature"};
}

oops::Variables AirTemperature_B::ingredients() const
{
    return AirTemperature_B::Ingredients;
}

size_t AirTemperature_B::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("virtual_temperature").shape(1);
}

atlas::FunctionSpace AirTemperature_B::productFunctionSpace(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("virtual_temperature").functionspace();
}

void AirTemperature_B::executeNL(atlas::FieldSet & afieldset)
{
    oops::Log::trace() << "entering AirTemperature_B::executeNL function"
        << std::endl;

    const double epsilon = configVariables_.getDouble("epsilon");

    atlas::Field virtual_temperature = afieldset.field("virtual_temperature");
    atlas::Field temperature = afieldset.field("air_temperature");
    atlas::Field specific_humidity = afieldset.field("water_vapor_mixing_ratio_wrt_moist_air");

    auto virtual_temperature_view = atlas::array::make_view<double, 2>(virtual_temperature);
    auto temperature_view = atlas::array::make_view<double, 2>(temperature);
    auto specific_humidity_view = atlas::array::make_view<double, 2>(specific_humidity);

    size_t grid_size = virtual_temperature.shape(0);

    int nlevels = virtual_temperature.shape(1);
    for (int level = 0; level < nlevels; ++level) {
      for ( size_t jnode = 0; jnode < grid_size ; ++jnode ) {
        temperature_view(jnode, level) =
            virtual_temperature_view(jnode, level) /
            (1.0 + epsilon * specific_humidity_view(jnode, level));
      }
    }
    oops::Log::trace() << "leaving AirTemperature_B::executeNL function" << std::endl;
}

}  // namespace vader
