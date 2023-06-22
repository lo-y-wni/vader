/*
 * (C) Copyright 2023 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "vader/vader.h"
// Recipe headers
#include "recipes/AirPotentialTemperature.h"
#include "recipes/AirPressureThickness.h"
#include "recipes/AirTemperature.h"
#include "recipes/AirVirtualTemperature.h"
#include "recipes/DryAirDensityLevelsMinusOne.h"
#include "recipes/HydrostaticExnerLevels.h"
#include "recipes/TotalWater.h"
#include "recipes/uwind_at_10m.h"
#include "recipes/VirtualPotentialTemperature.h"
#include "recipes/vwind_at_10m.h"

namespace vader {

const cookbookConfigType Vader::defaultCookbookDefinition = {
        // Default VADER cookbook definition
        {"air_temperature",        {AirTemperature_A::Name, AirTemperature_B::Name}},
        {"dry_air_density_levels_minus_one",  {DryAirDensityLevelsMinusOne_A::Name}},
        {"hydrostatic_exner_levels",  {HydrostaticExnerLevels_A::Name}},
        {"potential_temperature",  {AirPotentialTemperature_A::Name}},
        {"qt",                     {TotalWater_A::Name}},
        {"uwind_at_10m",           {uwind_at_10m_A::Name}},
        {"virtual_potential_temperature",  {VirtualPotentialTemperature_B::Name,
                                            VirtualPotentialTemperature_A::Name}},
        {"virtual_temperature",    {AirVirtualTemperature_A::Name}},
        {"vwind_at_10m",           {vwind_at_10m_A::Name}}
    };

}  // namespace vader
