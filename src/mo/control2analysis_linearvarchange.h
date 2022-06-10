/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field/FieldSet.h"

namespace mo {

/// \details Calculate qT increment from the sum of q, qcl and qcf increments
void qqclqcf2qtTL(atlas::FieldSet & incFields);

/// \details This is the adjoint of qqclqcf2qtTL
void qqclqcf2qtAD(atlas::FieldSet & hatFields);

/// \details This is the moisture incrementing operator (MIO) that determines the cloud condensate
///          increments from increments in total water and temperature
void qtTemperature2qqclqcfTL(const atlas::FieldSet & stateFields,
                        const atlas::FieldSet & ceffFieldSet,
                        atlas::FieldSet & incFields);

/// \details This is the adjoint of the MIO
void qtTemperature2qqclqcfAD(const atlas::FieldSet & stateFields,
                       const atlas::FieldSet & ceffFieldSet,
                       atlas::FieldSet & hatFields);

}  // namespace mo
