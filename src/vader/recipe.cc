/*
 * (C) Copyright 2021  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>
#include <vector>

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "oops/util/Logger.h"
#include "recipe.h"

namespace vader {

// -----------------------------------------------------------------------------
Recipe::Recipe() {}
// -----------------------------------------------------------------------------
Recipe::Recipe(std::vector<std::string> ingredients, std::function<int(atlas::FieldSet *)> execute)
          : ingredients{ingredients}, execute(execute) { }
// -----------------------------------------------------------------------------
Recipe::~Recipe() {}
// -----------------------------------------------------------------------------

// Implementation of vader "execute" functions for the instantiated recipes (Is this the right place for these?)

int ps_to_delp(atlas::FieldSet *afieldset) {
    int returnValue = 1;

	oops::Log::trace() << "entering ps_to_delp function" << std::endl;
    // TODO: Implement conversion logic
	oops::Log::trace() << "leaving ps_to_delp function" << std::endl;
    return returnValue;
}

int t_to_pt(atlas::FieldSet *afieldset) {
    int returnValue = 1;

	oops::Log::trace() << "entering t_to_pt function" << std::endl;

	atlas::Field temperature = afieldset->field("t");
	atlas::Field pressure = afieldset->field("ps");
	oops::Log::debug() << "Temperature Size: " << temperature.size() << std::endl;
	oops::Log::debug() << "Temperature Rank: " << temperature.rank() << std::endl;
	oops::Log::debug() << "Temperature Levels: " << temperature.levels() << std::endl;
	oops::Log::debug() << "Temperature shape: " << temperature.shape()[1] << "," << temperature.shape()[2] << std::endl;

	oops::Log::debug() << "Pressure Size: " << pressure.size() << std::endl;
	oops::Log::debug() << "Pressure Rank: " << pressure.rank() << std::endl;
	oops::Log::debug() << "Pressure Levels: " << pressure.levels() << std::endl;
	oops::Log::debug() << "Pressure shape: " << pressure.shape()[1] << "," << pressure.shape()[2] << std::endl;

	oops::Log::trace() << "leaving t_to_pt function" << std::endl;

    return returnValue;
}

}  // namespace vader

