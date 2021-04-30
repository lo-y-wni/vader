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
#include "vader.h"
#include "oops/util/Logger.h"
#include "cookbook.h"

namespace vader {

// -----------------------------------------------------------------------------
Vader::Vader() {}
// -----------------------------------------------------------------------------
Vader::~Vader() {}
// -----------------------------------------------------------------------------

// Create the static cookbook used to search for recipes
const std::unordered_map<std::string, std::vector<vader::Recipe>> Vader::cookbook  {
	{ "delp", delp_recipes},
	{ "pt", pt_recipes}
};


void Vader::changeVar(atlas::FieldSet * afieldset, const oops::Variables & vars) const {

	oops::Log::trace() << "entering Vader::changeVar: " << std::endl;

	// Actual changeVar functionality not implemented yet
	// Demonstrate that VADER has received the model fields via the Atlas fieldset
	std::string firstFieldName = afieldset->field_names().front();
	oops::Log::debug() << "First field name in fieldset: " << firstFieldName << std::endl;
	atlas::Field field1 = afieldset->field(firstFieldName);
	oops::Log::debug() << "Size: " << field1.size() << std::endl;
	oops::Log::debug() << "Rank: " << field1.rank() << std::endl;
	if (field1.rank() == 2 && field1.size() > 0) {
		auto field1_view = atlas::array::make_view <double , 2>( field1 );
		oops::Log::debug() << "First data element: " << field1_view(0, 0) << std::endl;
	}

	oops::Log::trace() << "leaving Vader::changeVar: " << std::endl;
}

}  // namespace vader

