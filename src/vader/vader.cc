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
#include "vader.h"
#include "cookbook.h"

namespace vader {

// -----------------------------------------------------------------------------
Vader::Vader() {}
// -----------------------------------------------------------------------------
Vader::~Vader() {}
// -----------------------------------------------------------------------------

// Create the static cookbook used to search for recipes
// (Recipe vectors are defined in cookbook.h)
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

	auto fieldSetFieldNames = afieldset->field_names();
	// Loop through all the requested fields in the output fieldset
	for (auto neededField : vars.variables()) {
		if (std::find(fieldSetFieldNames.begin(), fieldSetFieldNames.end(), neededField) == fieldSetFieldNames.end()) {
			oops::Log::error() << "Field '" << neededField <<
				"' is not allocated the fieldset. Calling function needs to do this. Vader can't." << std::endl;
		}
		else { // Check for nonzero data (How else can we know if field has been populated previously?)
			field1 = afieldset->field(neededField);
			int rank = field1.rank();
			bool alreadyFilled;
			if (rank == 1) {
				auto field1_view = atlas::array::make_view <double , 1>( field1 );
				alreadyFilled = (field1_view(0) != 0.0); // Need a better way to test. What if filled data is 0.0?
			}
			else if (rank == 2) {
				auto field1_view = atlas::array::make_view <double , 2>( field1 );
				alreadyFilled = (field1_view(0,0) != 0.0); // Need a better way to test. What if filled data is 0.0?
			}
			if (alreadyFilled) {
				oops::Log::debug() << "Field '" << neededField <<
					"' is already populated in the fieldset. Vader doesn't need to work on it." << std::endl;
			}
			else {
				oops::Log::debug() << "Field '" << neededField <<
					"' is not the input fieldset. Recipe required." << std::endl;
			}
			//  // See if the Vader cookbook contains a recipe for the requested field.
			// oops::Log::debug() << "Output field '" << requestedField <<
			// 	"' is not the input fieldset. Recipe required." << std::endl;
			// auto recipe = cookbook.find(requestedField);
			// if (recipe != cookbook.end()) {
			// 	oops::Log::debug() << "Vader cookbook contains a recipe for '" << requestedField <<
			// 		"'. Ingredients are:" << std::endl;
			// 	bool haveAllIngredients = true;
			// 	for (auto ingredient : recipe->second) {
			// 		oops::Log::debug() << ingredient << std::endl;
			// 		haveAllIngredients = haveAllIngredients &&
			// 			std::find(fieldNamesIn.begin(), fieldNamesIn.end(), ingredient) != fieldNamesIn.end();
			// 	}
			// 	if (haveAllIngredients) {
			// 		oops::Log::debug() << "All ingredients for recipe are in the input fieldset." << std::endl;
			// 	}
			// 	else {
			// 		oops::Log::debug() << "Do not have all ingredients for recipe in the input fieldset." << std::endl;
			// 	}
			// }
			// else {
			// 	oops::Log::debug() << "Vader cookbook does not contain a recipe for '" << requestedField << "'" << std::endl;
			// }
		}
	}

	oops::Log::trace() << "leaving Vader::changeVar: " << std::endl;
}

}  // namespace vader

