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
#include "oops/util/Logger.h"
#include "vader/recipes/AirPressureThickness.h"
#include "vader/vadervariables.h"

namespace vader
{

// Static attribute initialization
const char AirPressureThickness_A::Name[] = "AirPressureThickness_A";
const std::vector<std::string> AirPressureThickness_A::Ingredients = {VV_PS};

// Register the maker
static RecipeMaker<AirPressureThickness_A> makerPressureToDelP_(AirPressureThickness_A::Name);

AirPressureThickness_A::AirPressureThickness_A(const AirPressureThickness_AParameters &params)
{
    oops::Log::trace() << "AirPressureThickness_A::AirPressureThickness_A(params)" << std::endl;
}

std::string AirPressureThickness_A::name() const
{
    return AirPressureThickness_A::Name;
}

std::string AirPressureThickness_A::product() const
{
    return "air_pressure_thickness";
}

std::vector<std::string> AirPressureThickness_A::ingredients() const
{
    return AirPressureThickness_A::Ingredients;
}

size_t AirPressureThickness_A::productLevels(const atlas::FieldSet & afieldset) const
{
    return afieldset.field(VV_PS).levels();
}

atlas::FunctionSpace AirPressureThickness_A::productFunctionSpace
                                            (const atlas::FieldSet & afieldset) const
{
    return afieldset.field(VV_PS).functionspace();
}

bool AirPressureThickness_A::executeNL(atlas::FieldSet & afieldset)
{
    bool delp_filled = false;

    oops::Log::trace() << "entering AirPressureThickness_A execute function" << std::endl;
    // TODO(Someone who knows this algorithm): Implement conversion logic
    oops::Log::trace() << "leaving AirPressureThickness_A execute function" << std::endl;
    return delp_filled;
}

}  // namespace vader
