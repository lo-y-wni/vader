/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef COOKBOOK_H_
#define COOKBOOK_H_

#include <vector>
#include <functional>

#include "atlas/field/FieldSet.h"

#include "recipe.h"
#include "vader.h"

namespace vader {

// -----------------------------------------------------------------------------
// List of implemented recipe "execute" methods (forward declarations)
// (Currently the implementation of these receipe 'execute' functions is in recipe.cc. 
//  Not sure if this is the right way to split these source files.)
int ps_to_delp(atlas::FieldSet *ingredients);
int t_to_pt(atlas::FieldSet *ingredients);

// -----------------------------------------------------------------------------
/// Create inidividual recipe objects
Recipe delp_recipe1{{"ps"}, ps_to_delp};

Recipe pt_recipe1{{"t", "ps"}, t_to_pt};

// Put inidividual recipes into vectors based on the variable produced.
// Often only one recipe per vector, but could be multiple.
std::vector<vader::Recipe> delp_recipes = {delp_recipe1};
std::vector<vader::Recipe> pt_recipes   = {pt_recipe1};

// Create the static cookbook used to search for recipes
const std::unordered_map<std::string, std::vector<vader::Recipe>> Vader::cookbook  {
	{ "delp", delp_recipes},
	{ "pt", pt_recipes}
};

} // namespace vader

#endif  // COOKBOOK_H_
