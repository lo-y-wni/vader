/*
 * (C) Copyright 2021-2022  UCAR.
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
#include "vader/recipes/TempToPTemp.h"
#include "vader/vadervariables.h"

namespace vader
{
// ------------------------------------------------------------------------------------------------

// Static attribute initialization
const char TempToPTemp::Name[] = "TempToPTemp";
const std::vector<std::string> TempToPTemp::Ingredients = {VV_TS, VV_PS};
const double default_kappa = 0.2857;
const double p0_not_in_params = -1.0;
const double default_Pa_p0 = 100000.0;
const double default_hPa_p0 = 1000.0;

// Register the maker
static RecipeMaker<TempToPTemp> makerTempToPTemp_(TempToPTemp::Name);

TempToPTemp::TempToPTemp() :
    p0_{p0_not_in_params},
    kappa_{default_kappa}
{
    oops::Log::trace() << "TempToPTemp::TempToPTemp()" << std::endl;
}

TempToPTemp::TempToPTemp(const Parameters_ &params) :
    p0_{params.p0.value()},
    kappa_{params.kappa.value()}
{
    oops::Log::trace() << "TempToPTemp::TempToPTemp(params)" << std::endl;
    oops::Log::debug() << "TempToPTemp params p0 value: " << params.p0.value()
        << std::endl;
    oops::Log::debug() << "TempToPTemp params kappa value: "
        << params.kappa.value() << std::endl;
}

std::string TempToPTemp::name() const
{
    return TempToPTemp::Name;
}

std::vector<std::string> TempToPTemp::ingredients() const
{
    return TempToPTemp::Ingredients;
}

bool TempToPTemp::execute(atlas::FieldSet *afieldset)
{
    bool potential_temperature_filled = false;

    oops::Log::trace() << "entering TempToPTemp::execute function"
        << std::endl;

    atlas::Field temperature = afieldset->field(VV_TS);
    atlas::Field surface_pressure = afieldset->field(VV_PS);
    atlas::Field potential_temperature = afieldset->field(VV_PT);
    std::string t_units, ps_units;

    temperature.metadata().get("units", t_units);
    surface_pressure.metadata().get("units", ps_units);
    if (p0_ == p0_not_in_params)
    {
        oops::Log::debug() << "TempToPTemp: p0 not in parameters. Deducing "
            "value from pressure units." << std::endl;
        if (ps_units == "Pa")
        {
            p0_ = default_Pa_p0;
        } else if (ps_units == "hPa") {
            p0_ = default_hPa_p0;
        } else {
            oops::Log::error() <<
              "TempToPTemp::execute failed because p0 could not be determined." << std::endl;
            return false;
        }
    }

    oops::Log::debug() << "TempToPTemp::execute: p0 value: " << p0_ <<
        std::endl;
    oops::Log::debug() << "TempToPTemp::execute: kappa value: " << kappa_ <<
    std::endl;

    auto temperature_view = atlas::array::make_view<double, 2>(temperature);
    auto surface_pressure_view = atlas::array::make_view<double, 1>(surface_pressure);
    auto potential_temperature_view = atlas::array::make_view<double, 2>(potential_temperature);

    size_t grid_size = surface_pressure.size();

    int nlevels = temperature.levels();
    for (int level = 0; level < nlevels; ++level) {
      for ( size_t jnode = 0; jnode < grid_size ; ++jnode ) {
        potential_temperature_view(jnode, level) =
            temperature_view(jnode, level) * pow(p0_ / surface_pressure_view(jnode), kappa_);
      }
    }

    potential_temperature_filled = true;

    oops::Log::trace() << "leaving TempToPTemp::execute function" << std::endl;

    return potential_temperature_filled;
}

}  // namespace vader
