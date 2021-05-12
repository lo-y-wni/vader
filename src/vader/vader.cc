/*
 * (C) Copyright 2021  UCAR.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>
#include <vector>
#include <memory>
#include <utility>

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "oops/util/Logger.h"
#include "vader.h"
#include "cookbook.h"
#include "TempToPTemp.h"
#include "PressureToDelP.h"

namespace vader {

// -----------------------------------------------------------------------------
Vader::~Vader() {
	oops::Log::trace() << "Inside Vader destructor." << std::endl;
}
// -----------------------------------------------------------------------------

std::unordered_map<std::string, std::vector<std::string>> Vader::getDefaultCookbookDef() {
	return {
		// This defines default Vader Cookbook
		  {"pt", {TempToPTempRecipe::Name}}
		, {"delp", {PressureToDelP::Name}}
	};
}
// -----------------------------------------------------------------------------
void Vader::createCookbook(std::unordered_map<std::string, std::vector<std::string>> definition) {
	std::vector<std::unique_ptr<Recipe>> recipes;
	for (auto defEntry : definition ) {
		recipes.clear();
		for (auto recipeName : defEntry.second) {
			recipes.push_back(recipeFactory(recipeName));
			if (recipes.back() == nullptr) {
				recipes.pop_back();
			}
		}
		cookbook_[defEntry.first] = std::move(recipes);
	}
}
// -----------------------------------------------------------------------------
Vader::Vader() {
	createCookbook(getDefaultCookbookDef());
}
// -----------------------------------------------------------------------------
Vader::Vader(const eckit::Configuration & config) {
	std::unordered_map<std::string, std::vector<std::string>> definition = getDefaultCookbookDef();
	// Configuration can alter the default cookbook here
	createCookbook(definition);
}
// -----------------------------------------------------------------------------
std::unique_ptr<Recipe> Vader::recipeFactory(std::string recipeName) {
	oops::Log::trace() << "entering Vader::recipeFactory for recipeName: " << recipeName << std::endl;

	if (recipeName == TempToPTempRecipe::Name) return std::unique_ptr<Recipe>(new TempToPTempRecipe());
	if (recipeName == PressureToDelP::Name) return std::unique_ptr<Recipe>(new PressureToDelP());
	oops::Log::error() << "Vader::recipeFactory recieved unimplemented recipe name: " << recipeName << std::endl;
	return nullptr;
}
void Vader::changeVar(atlas::FieldSet * afieldset, const oops::Variables & vars) const {

	oops::Log::trace() << "entering Vader::changeVar " << std::endl;

	auto fieldSetFieldNames = afieldset->field_names();
	// Loop through all the requested fields in the output fieldset
	for (auto neededField : vars.variables()) {
		if (std::find(fieldSetFieldNames.begin(), fieldSetFieldNames.end(), neededField) == fieldSetFieldNames.end()) {
			oops::Log::error() << "Field '" << neededField <<
				"' is not allocated the fieldset. Calling function needs to do this. Vader can't." << std::endl;
		}
		else {
			int returnValue = getVariable(afieldset, neededField);
		}
	}

	oops::Log::trace() << "leaving Vader::changeVar: " << std::endl;
}

int Vader::getVariable(atlas::FieldSet * afieldset, const std::string variableName) const {

	atlas::Field field1 = afieldset->field(variableName);
	bool alreadyFilled;
	int returnValue = 1;

	oops::Log::trace() << "entering Vader::getVariable for variable: " << variableName << std::endl;

	auto rank = field1.rank();

	if (rank == 1) {
		auto field1_view = atlas::array::make_view <double , 1>( field1 );
		alreadyFilled = (field1_view(0) != 0.0); // Need a better way to test. What if filled data is 0.0?
	}
	else if (rank == 2) {
		auto field1_view = atlas::array::make_view <double , 2>( field1 );
		alreadyFilled = (field1_view(0, 0) != 0.0); // Need a better way to test. What if filled data is 0.0?
	}

	if (alreadyFilled) {
		oops::Log::debug() << "Field '" << variableName <<
			"' is already populated in the fieldset. Vader doesn't need to work on it." << std::endl;
		returnValue = 0;
	}
	else { // Look in the cookbook for a recipe
		oops::Log::debug() << "Field '" << variableName <<
			"' is not the input fieldset. Recipe required." << std::endl;
		auto recipeList = cookbook_.find(variableName);

		if ((recipeList != cookbook_.end()) && !recipeList->second.empty()) {
			oops::Log::debug() << "Vader cookbook contains at least one recipe for '" << variableName << std::endl;
			for (int i=0; i < recipeList->second.size(); ++i) {
				bool haveAllIngredients = true;
				for (auto ingredient : recipeList->second[i]->ingredients()) {
					oops::Log::debug() << "Checking if we have ingredient: " << ingredient << std::endl;
					auto fieldSetFieldNames = afieldset->field_names();
					haveAllIngredients = haveAllIngredients &&
						std::find(fieldSetFieldNames.begin(), fieldSetFieldNames.end(), ingredient) != fieldSetFieldNames.end();
					if (!haveAllIngredients) break;
				}
				if (haveAllIngredients) {
					oops::Log::debug() << "All ingredients for a recipe are in the fieldset. Executing the recipe." << std::endl;
					if (recipeList->second[i]->requiresSetup()) {
						recipeList->second[i]->setup(afieldset);
					}
					returnValue = recipeList->second[i]->execute(afieldset);
					break;
				}
				else {
					oops::Log::debug() << "Do not have all the ingredients for a recipe available in the fieldset." << std::endl;
				}
			}
			if (returnValue == 0) {
				oops::Log::debug() << "Vader successfully executed recipe for " << variableName << std::endl;
			}
			else {
				oops::Log::debug() << "Vader was unable to calculate " << variableName << std::endl;
			}
		}
		else {
			oops::Log::debug() << "Vader cookbook does not contain a recipe for '" << variableName << "'" << std::endl;
		}
	}

	oops::Log::trace() << "leaving Vader::getVariable for variable: " << variableName << std::endl;

	return returnValue;
}

}  // namespace vader

