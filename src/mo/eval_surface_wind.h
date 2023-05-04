/*
 * (C) Crown Copyright 2023 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field.h"

namespace mo {

/// \details This calculates the wind increment at 10 m from the atmospheric
///          wind increment at the lowest level
// TODO(Mayeul) Remove evalSurfWindTL in model2geovals_linearvarchange.h/.cc
void eval_surface_wind_tl(atlas::FieldSet & incFlds);

/// \details This calculates the adjoint of the wind increment at the lowest level
///          from the adjoint of the atmospheric wind at 10 m
// TODO(Mayeul) Remove evalSurfWindAD in model2geovals_linearvarchange.h/.cc
void eval_surface_wind_ad(atlas::FieldSet & hatFlds);

}  // namespace mo
