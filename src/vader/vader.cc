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

   // Configuration can alter the default cookbook here
   createCookbook(definition, config);
}
// -----------------------------------------------------------------------------
void Vader::changeVar(atlas::FieldSet * afieldset, oops::Variables & neededVars) const {

   oops::Log::trace() << "entering Vader::changeVar " << std::endl;
   oops::Log::debug() << "neededVars passed to Vader::changeVar: " << neededVars << std::endl;

   oops::Variables foundVariables;

   auto fieldSetFieldNames = afieldset->field_names();
   // Loop through all the requested fields in the output fieldset
   for (auto neededField : neededVars.variables()) {
      if (std::find(fieldSetFieldNames.begin(), fieldSetFieldNames.end(), neededField) == fieldSetFieldNames.end()) {
         oops::Log::error() << "Field '" << neededField <<
            "' is not allocated the fieldset. Calling function needs to do this. Vader can't." << std::endl;
      }
      else {
         if (getVariable(afieldset, neededField)) {
            foundVariables.push_back(neededField);
         }
      }
   }
   neededVars -= foundVariables;
   oops::Log::debug() << "neededVars remaining after Vader::changeVar: " << neededVars << std::endl;
    oops::Log::trace() << "leaving Vader::changeVar: " << std::endl;
}
// -----------------------------------------------------------------------------
bool Vader::getVariable(atlas::FieldSet * afieldset, const std::string variableName) const {

   atlas::Field field1 = afieldset->field(variableName);
   bool variableFilled = false;

   oops::Log::trace() << "entering Vader::getVariable for variable: " << variableName << std::endl;

   // Look in the cookbook for a recipe
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
               // Potentially a recipe might require set-up that should only be performed the first time
               if (recipeList->second[i]->requiresSetup()) {
                  recipeList->second[i]->setup(afieldset);
               }
               variableFilled = recipeList->second[i]->execute(afieldset);
               break;
            }
            else {
               oops::Log::debug() << "Do not have all the ingredients for a recipe available in the fieldset." << std::endl;
            }
      }
      if (variableFilled) {
            oops::Log::debug() << "Vader successfully executed recipe for " << variableName << std::endl;
      }
      else {
            oops::Log::debug() << "Vader tried but failed to successfully execute recipe for " << variableName << std::endl;
      }
   }
   else {
      oops::Log::debug() << "Vader cookbook does not contain a recipe for '" << variableName << "'" << std::endl;
   }

   oops::Log::trace() << "leaving Vader::getVariable for variable: " << variableName << std::endl;

   return variableFilled;
}

}  // namespace vader

