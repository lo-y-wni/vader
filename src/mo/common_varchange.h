
/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */


#pragma once

#include <string>
#include <vector>

#include "atlas/field/Field.h"
#include "atlas/field/FieldSet.h"

namespace mo {

const std::string constantsFilePath = "Data/parameters/svp_dlsvp_svpW_dlsvpW.nc";

/// \brief function to evaluate saturation water pressure (svp) [Pa]
/// the Atlas field in the argument must contain an inizialised air temperature field
/// and to have a defined svp field which is then calculated and returned as output
///
bool evalSatVaporPressure(atlas::FieldSet & fields);

/// \brief function to evaluate saturation specific humidity (qsat)
/// Needs air pressure [Pa] and svp [Pa] Atlas fields and returns the qsat Atlas field
bool evalSatSpecificHumidity(const atlas::Field & pbar, const atlas::Field & svp, atlas::Field & qsat);

}
