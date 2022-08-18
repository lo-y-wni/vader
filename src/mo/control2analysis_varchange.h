/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field/FieldSet.h"

namespace mo {

/// \details Transformation from virtual potential temperature (thetav) to
///          hydrostatically-balanced exner hexner
///          Uses pressure level 0 and not psurf.
bool thetavP2Hexner(atlas::FieldSet & fields);

/// \details This is a straight inverse of thetavP2Hexner
///          Note that we need to revisit this for the top level for thetav.
void hexner2PThetav(atlas::FieldSet & fields);

/// \details Calculate the virtual potential temperature
///          from the specific humidity and the potential temperature.
void evalVirtualPotentialTemperature(atlas::FieldSet & fields);

/// \details Calculate qT increment from the sum of q, qcl and qcf increments
void qqclqcf2qt(atlas::FieldSet & fields);

/// \details Calculate the dry air density
///          from the air_pressure_levels_minus_one,
///          air_temperature (which needs to be interpolated).
void evalDryAirDensity(atlas::FieldSet & fields);

/// \details Calculate exner pressure levels
void evalExnerPressureLevels(atlas::FieldSet & fields);

/// \details Calculate the moisture control variable dependencies
///          (excluding the fields derived from covariance file.)
void evalMoistureControlDependencies(atlas::FieldSet & fields);

}  // namespace mo
