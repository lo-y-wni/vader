/*
 * (C) Copyright 2021  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <math.h>
#include <iostream>
#include <vector>

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "oops/util/Logger.h"
#include "vader/vader/vadervariables.h"
#include "vader/recipies/PressureToDelP.h"

namespace vader
{

// Static attribute initialization
const char PressureToDelP::Name[] = "PressureToDelP";
const std::vector<std::string> PressureToDelP::Ingredients = {VV_PS};

// Register the maker
static RecipeMaker<PressureToDelP> makerPressureToDelP_(PressureToDelP::Name);

PressureToDelP::PressureToDelP(const eckit::Configuration &config)
{
}

std::string PressureToDelP::name() const
{
    return PressureToDelP::Name;
}

std::vector<std::string> PressureToDelP::ingredients() const
{
    return PressureToDelP::Ingredients;
}

bool PressureToDelP::execute(atlas::FieldSet *afieldset)
{
    bool delp_filled = false;

    oops::Log::trace() << "entering ps_to_delp execute function" << std::endl;
    // TODO: Implement conversion logic
    oops::Log::trace() << "leaving ps_to_delp execute function" << std::endl;
    return delp_filled;
}

}  // namespace vader
