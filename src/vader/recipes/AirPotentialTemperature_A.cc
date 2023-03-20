/*
 * (C) Copyright 2021-2023  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <math.h>
#include <iostream>
#include <vector>

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "atlas/util/Metadata.h"
#include "oops/util/Logger.h"
#include "vader/recipes/AirPotentialTemperature.h"


namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char AirPotentialTemperature_A::Name[] = "AirPotentialTemperature_A";
const std::vector<std::string> AirPotentialTemperature_A::Ingredients = {"air_temperature",
                                                                         "surface_pressure"};
const double p0_not_in_params = -1.0;
const double default_Pa_p0 = 100000.0;
const double default_hPa_p0 = 1000.0;

// Register the maker
static RecipeMaker<AirPotentialTemperature_A> makerTempToPTemp_(AirPotentialTemperature_A::Name);

AirPotentialTemperature_A::AirPotentialTemperature_A(const Parameters_ & params,
                                    const VaderConfigVars & configVariables) :
    p0_{params.p0.value()},
    kappa_{params.kappa.value()}
{
    oops::Log::trace() << "AirPotentialTemperature_A::AirPotentialTemperature_A(params)"
        << std::endl;
    oops::Log::debug() << "AirPotentialTemperature_A params p0 value: " << params.p0.value()
        << std::endl;
    oops::Log::debug() << "AirPotentialTemperature_A params kappa value: "
        << params.kappa.value() << std::endl;
}

std::string AirPotentialTemperature_A::name() const
{
    return AirPotentialTemperature_A::Name;
}

std::string AirPotentialTemperature_A::product() const
{
    // TODO(vahl): update this to air_potential_temperature (models will also need to adapt)
    return "potential_temperature";
}

std::vector<std::string> AirPotentialTemperature_A::ingredients() const
{
    return AirPotentialTemperature_A::Ingredients;
}

size_t AirPotentialTemperature_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field("air_temperature").levels();
}

atlas::FunctionSpace AirPotentialTemperature_A::productFunctionSpace
                                                (const atlas::FieldSet & afieldset) const
{
    return afieldset.field("air_temperature").functionspace();
}

bool AirPotentialTemperature_A::executeNL(atlas::FieldSet & afieldset)
{
    bool potential_temperature_filled = false;

    oops::Log::trace() << "entering AirPotentialTemperature_A::executeNL function"
        << std::endl;

    atlas::Field temperature = afieldset.field("air_temperature");
    atlas::Field surface_pressure = afieldset.field("surface_pressure");
    atlas::Field potential_temperature = afieldset.field("potential_temperature");
    std::string t_units, ps_units;

    temperature.metadata().get("units", t_units);
    surface_pressure.metadata().get("units", ps_units);
    if (p0_ == p0_not_in_params)
    {
        oops::Log::debug() << "AirPotentialTemperature_A: p0 not in parameters. Deducing "
            "value from pressure units." << std::endl;
        if (ps_units == "Pa")
        {
            p0_ = default_Pa_p0;
        } else if (ps_units == "hPa") {
            p0_ = default_hPa_p0;
        } else {
            oops::Log::error() <<
              "AirPotentialTemperature_A::execute failed because p0 could not be determined."
                << std::endl;
            return false;
        }
    }

    oops::Log::debug() << "AirPotentialTemperature_A::execute: p0 value: " << p0_ <<
        std::endl;
    oops::Log::debug() << "AirPotentialTemperature_A::execute: kappa value: " << kappa_ <<
    std::endl;

    auto temperature_view = atlas::array::make_view<double, 2>(temperature);
    auto surface_pressure_view = atlas::array::make_view<double, 2>(surface_pressure);
    auto potential_temperature_view = atlas::array::make_view<double, 2>(potential_temperature);

    size_t grid_size = surface_pressure.size();

    int nlevels = temperature.levels();
    for (int level = 0; level < nlevels; ++level) {
      for ( size_t jnode = 0; jnode < grid_size ; ++jnode ) {
        potential_temperature_view(jnode, level) =
            temperature_view(jnode, level) * pow(p0_ / surface_pressure_view(jnode, 0), kappa_);
      }
    }

    potential_temperature_filled = true;

    oops::Log::trace() << "leaving AirPotentialTemperature_A::executeNL function" << std::endl;

    return potential_temperature_filled;
}

}  // namespace vader
