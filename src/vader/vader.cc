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
#include <unordered_map>
#include <algorithm>

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "oops/util/Logger.h"
#include "vader.h"
#include "cookbook.h"

namespace vader {

// -----------------------------------------------------------------------------
Vader::~Vader() {
   oops::Log::trace() << "Vader::~Vader done" << std::endl;
}
// -----------------------------------------------------------------------------
void Vader::createCookbook(std::unordered_map<std::string, std::vector<std::string>> definition, 
      const eckit::Configuration & config) {
   std::vector<std::unique_ptr<Recipe>> recipes;
   for (auto defEntry : definition ) {
      recipes.clear();
      for (auto recipeName : defEntry.second) {
         recipes.push_back(recipeFactory(recipeName, config));
         if (recipes.back() == nullptr) {
            recipes.pop_back();
         }
      }
      cookbook_[defEntry.first] = std::move(recipes);
   }
}
// -----------------------------------------------------------------------------
Vader::Vader(const eckit::Configuration & config) {
   std::unordered_map<std::string, std::vector<std::string>> definition = getDefaultCookbookDef();
   oops::Log::trace() << "entering Vader::Vader(config) " << std::endl;
   oops::Log::debug() << "Vader::Vader config = " << config << std::endl;

   // TODO::Configuration can alter the default cookbook here
   createCookbook(definition, config);
}
// -----------------------------------------------------------------------------
void Vader::changeVar(atlas::FieldSet * afieldset, oops::Variables & neededVars) const {

   oops::Log::trace() << "entering Vader::changeVar " << std::endl;
   oops::Log::debug() << "neededVars passed to Vader::changeVar: " << neededVars << std::endl;

   oops::Variables ingredients{};

   auto fieldSetFieldNames = afieldset->field_names();
   // Loop through all the requested fields in neededVars
   // Since neededVars can be modified by getVariable and getVariable calls itself recursively,
   // we make a copy of the list here before we start.
   std::vector<std::string> targetVariables{neededVars.variables()};

   for (auto targetVariable : targetVariables) {
      oops::Log::debug() << "Vader::changeVar calling Vader::getVariable for: " << targetVariable << std::endl;
      getVariable(afieldset, neededVars, targetVariable);
   }
   oops::Log::debug() << "neededVars remaining after Vader::changeVar: " << neededVars << std::endl;
   oops::Log::trace() << "leaving Vader::changeVar: " << std::endl;
}
// -----------------------------------------------------------------------------
bool Vader::getVariable(atlas::FieldSet * afieldset, oops::Variables & neededVars, const std::string targetVariable) const {

   bool variableFilled = false;

   oops::Log::trace() << "entering Vader::getVariable for variable: " << targetVariable << std::endl;

   auto fieldSetFieldNames = afieldset->field_names();

   if (std::find(fieldSetFieldNames.begin(), fieldSetFieldNames.end(), targetVariable) == fieldSetFieldNames.end()) {
      oops::Log::debug() << "Field '" << targetVariable <<
         "' is not allocated the fieldset. Vader cannot make it." << std::endl;
      oops::Log::trace() << "leaving Vader::getVariable for variable: " << targetVariable << std::endl;
      return false;
   }

   // Since this function is called recursively, make sure targetVariable is still needed
   if (!neededVars.has(targetVariable)) {
      oops::Log::debug() << targetVariable << " is no longer in the variable list neededVars." << std::endl;
      oops::Log::trace() << "leaving Vader::getVariable for variable: " << targetVariable << std::endl;
      return true;
   }

   auto recipeList = cookbook_.find(targetVariable);

   // If recipeList is found, recipeList->second is a vector of unique_ptr's to Recipe objects that produce 'variableName'
   if ((recipeList != cookbook_.end()) && !recipeList->second.empty()) {
      oops::Log::debug() << "Vader cookbook contains at least one recipe for '" << targetVariable << "'" << std::endl;
      for (int i=0; i < recipeList->second.size(); ++i) {
         oops::Log::debug() << "Checking fieldset to see if we have ingredients for recipe: " << recipeList->second[i]->name() << std::endl;
         bool haveIngredient = false;
         for (auto ingredient : recipeList->second[i]->ingredients()) {
            haveIngredient = 
               (std::find(fieldSetFieldNames.begin(), fieldSetFieldNames.end(), ingredient) != fieldSetFieldNames.end()) &&
               (!neededVars.has(ingredient));
            if (!haveIngredient) {
               oops::Log::debug() << "ingredient " << ingredient << " not found. Recursively checking if Vader can make it." << std::endl;
               haveIngredient = getVariable(afieldset, neededVars, ingredient);
            }
            oops::Log::debug() << "ingredient " << ingredient << (haveIngredient ? " is" : " is not") << " available." << std::endl;
            if (!haveIngredient) break;
         }
         if (haveIngredient) {
            oops::Log::debug() << "All ingredients are in the fieldset. Executing the recipe." << std::endl;
            // Potentially a recipe might require set-up that should only be performed the first time
            if (recipeList->second[i]->requiresSetup()) {
               recipeList->second[i]->setup(afieldset);
            }
            variableFilled = recipeList->second[i]->execute(afieldset);
            if (variableFilled) {
               neededVars -= targetVariable;
               oops::Log::debug() << "Vader successfully executed recipe: " << recipeList->second[i]->name() << std::endl;
               break;
            }
            else {
               oops::Log::debug() << "Vader tried but failed to execute recipe: " << recipeList->second[i]->name() << std::endl;
            }
         }
         else {
            oops::Log::debug() << "Do not have all the ingredients for this recipe." << std::endl;
         }
      }
   }
   else {
      oops::Log::debug() << "Vader cookbook does not contain a recipe for: " << targetVariable << std::endl;
   }
   oops::Log::trace() << "leaving Vader::getVariable for variable: " << targetVariable << std::endl;
   return variableFilled;
}

}  // namespace vader

