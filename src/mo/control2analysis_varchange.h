/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field/FieldSet.h"

/// \details Transformation from virtual potential temperature (thetav) to
///          hydrostatically-balanced exner hexner
///          Uses pressure level 0 and not psurf.
bool thetavP2Hexner(atlas::FieldSet & fields);

/// \details This is a straight inverse of thetavP2Hexner
///          Note that we need to revisit this for the top level for thetav.
void hexner2PThetav(atlas::FieldSet & fields);
