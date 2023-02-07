/*
 * (C) Crown Copyright 2023 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field/FieldSet.h"

namespace mo {

/// \details Calculate the dry air density
void eval_dry_air_density_nl(atlas::FieldSet & stateFlds);

void eval_dry_air_density_tl(atlas::FieldSet & incFlds, const atlas::FieldSet & stateFlds);

void eval_dry_air_density_ad(atlas::FieldSet & hatFlds, const atlas::FieldSet & stateFlds);

}
