/*
 * (C) Crown Copyright 2023 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field.h"

namespace mo {

/// \details This calculates the vertically-regressed geostrophic pressure increment field
///          in grid point space and adds it to the unbalanced pressure increment field to give
///          hydrostatic balance increments
void eval_hydrostatic_pressure_levels_minus_one_tl(atlas::FieldSet & incFlds,
                                                   const atlas::FieldSet & augStateFlds);

/// \details This is the adjoint of the calculation of hydrostatic balance increment fields
void eval_hydrostatic_pressure_levels_minus_one_ad(atlas::FieldSet & hatFlds,
                                                   const atlas::FieldSet & augStateFlds);

/// \details This is the inverse of eval_hydrostatic_pressure_levels_tl.
///          This function provides the unbalanced pressure increment from the geostrophic pressure
///          and the hydrostatic pressure.
void eval_hydrostatic_pressure_levels_minus_one_tl_inv(atlas::FieldSet & incFlds,
                                                       const atlas::FieldSet & augStateFlds);

}  // namespace mo
