/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef COOKBOOK_H_
#define COOKBOOK_H_

#include <vector>
#include <memory>
#include <unordered_map>

#include "oops/util/Logger.h"
#include "recipe.h"
#include "vader.h"

// Recipe headers
#include "recipes/TempToPTemp.h"
#include "recipes/PressureToDelP.h"

namespace vader {

// -----------------------------------------------------------------------------
// All implemented recipes need a corresponding line in this method
std::unique_ptr<Recipe> Vader::recipeFactory(std::string recipeName) {
	oops::Log::trace() << "entering Vader::recipeFactory for recipeName: " << recipeName << std::endl;

	if (recipeName == TempToPTempRecipe::Name) return std::unique_ptr<Recipe>(new TempToPTempRecipe());
	if (recipeName == PressureToDelPRecipe::Name) return std::unique_ptr<Recipe>(new PressureToDelPRecipe());

    // If we get here, we didn't find a matching recipe name.
	oops::Log::error() << "Vader::recipeFactory recieved unimplemented recipe name: " << recipeName << std::endl;
	return nullptr;
}

// -----------------------------------------------------------------------------
// This defines the recipes used (and their priority) by default in Vader
// (Recipes can be added, removed, or rearranged in cookbook by specifying in yaml.)
std::unordered_map<std::string, std::vector<std::string>> Vader::getDefaultCookbookDef() {
	return {
		// This defines default Vader Cookbook
        // The Key is the name of the variable produced by all the recipes in the Value
        // The Value is a vector of recipe names that will be searched, in order, by Vader for viability
		  {"pt", {TempToPTempRecipe::Name}}
		, {"delp", {PressureToDelPRecipe::Name}}
	};
}

} // namespace vader

#endif  // COOKBOOK_H_
