/*
 * (C) Crown Copyright 2023 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field/FieldSet.h"

namespace mo {

/// \brief function to evaluate the rain mixing ratio
///        wrt moist air and condensed water:
///   qrain = rain_mixing_ratio_wrt_dry_air/total_water_mixing_ratio_wrt_dry_air
///
void eval_rain_mixing_ratio_wrt_moist_air_and_condensed_water_nl(
        atlas::FieldSet & stateFlds);


void eval_rain_mixing_ratio_wrt_moist_air_and_condensed_water_tl(
        atlas::FieldSet & incFlds,
        const atlas::FieldSet & stateFlds);


void eval_rain_mixing_ratio_wrt_moist_air_and_condensed_water_ad(
        atlas::FieldSet & hatFlds,
        const atlas::FieldSet & stateFlds);

}  // namespace mo

