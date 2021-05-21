/*
 * (C) Copyright 2021  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>
#include <vector>
#include <math.h>

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "oops/util/Logger.h"
#include "vader/vadervariables.h"
#include "PressureToDelP.h"

namespace vader {

// Static attribute initialization
const std::string PressureToDelPRecipe::Name = "ps_to_delp";
const std::vector<std::string> PressureToDelPRecipe::Ingredients = {VV_PS};

PressureToDelPRecipe::PressureToDelPRecipe(const eckit::Configuration & config) {
}

std::string PressureToDelPRecipe::name() const {
   return PressureToDelPRecipe::Name;
}

std::vector<std::string> PressureToDelPRecipe::ingredients() const {
   return PressureToDelPRecipe::Ingredients;
}

bool PressureToDelPRecipe::execute(atlas::FieldSet *afieldset) {
   bool delp_filled = false;

   oops::Log::trace() << "entering ps_to_delp execute function" << std::endl;
   // TODO: Implement conversion logic
   oops::Log::trace() << "leaving ps_to_delp execute function" << std::endl;
   return delp_filled;
}

}