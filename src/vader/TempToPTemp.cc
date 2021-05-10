/*
 * (C) Copyright 2021  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>
#include <vector>
#include <math.h>

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "oops/util/Logger.h"
#include "TempToPTemp.h"

namespace vader {

// Static attribute initialization
const std::string TempToPTempRecipe::Name = "t_to_pt";
const std::vector<std::string> TempToPTempRecipe::Ingredients = {"t", "ps"};

std::string TempToPTempRecipe::name() {
    return TempToPTempRecipe::Name;
}

std::vector<std::string> TempToPTempRecipe::ingredients() {
    return TempToPTempRecipe::Ingredients;
}

int TempToPTempRecipe::execute(atlas::FieldSet *afieldset) {
    int returnValue = 1;
    const double kappa = 0.286;
    const double p_nought = 1000.0;

    oops::Log::trace() << "entering t_to_pt function" << std::endl;

    atlas::Field temperature = afieldset->field("t");
    atlas::Field pressure = afieldset->field("ps");
    atlas::Field potential_temperature = afieldset->field("pt");
    auto temperature_view = atlas::array::make_view <double , 2>( temperature );
    auto pressure_view = atlas::array::make_view <double , 2>( pressure );
    auto potential_temperature_view = atlas::array::make_view <double , 2>( potential_temperature );

    oops::Log::debug() << "Temperature Size: " << temperature.size() << std::endl;
    oops::Log::debug() << "Temperature Rank: " << temperature.rank() << std::endl;
    oops::Log::debug() << "Temperature Levels: " << temperature.levels() << std::endl;
    oops::Log::debug() << "Temperature shape: " << temperature.shape()[1] << "," << temperature.shape()[2] << std::endl;

    oops::Log::debug() << "Pot. Temperature Size: " << potential_temperature.size() << std::endl;
    oops::Log::debug() << "Pot. Temperature Rank: " << potential_temperature.rank() << std::endl;
    oops::Log::debug() << "Pot. Temperature Levels: " << potential_temperature.levels() << std::endl;
    oops::Log::debug() << "Pot. Temperature shape: " << potential_temperature.shape()[1] << "," << potential_temperature.shape()[2] << std::endl;

    oops::Log::debug() << "Pressure Size: " << pressure.size() << std::endl;
    oops::Log::debug() << "Pressure Rank: " << pressure.rank() << std::endl;
    oops::Log::debug() << "Pressure Levels: " << pressure.levels() << std::endl;
    oops::Log::debug() << "Pressure shape: " << pressure.shape()[1] << "," << pressure.shape()[2] << std::endl;

    int nlevels = temperature.levels();
    for (int level = 0; level < nlevels; ++level) {
        potential_temperature_view(level, 0) = temperature_view(level, 0) * pow(p_nought / pressure_view(1, 0), kappa);
    }
    returnValue = 0;
    oops::Log::debug() << "Pot. Temperature 1st element: " << potential_temperature_view(1,0) << std::endl;
    oops::Log::debug() << "Temperature 1st element: " << temperature_view(1,0) << std::endl;
    oops::Log::debug() << "Pressure 1st element: " << pressure_view(1,0) << std::endl;

    oops::Log::trace() << "leaving t_to_pt function" << std::endl;

    return returnValue;
}

}