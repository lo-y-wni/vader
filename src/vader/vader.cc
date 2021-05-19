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

   oops::Variables ingredients{};

   auto fieldSetFieldNames = afieldset->field_names();
   // Loop through all the requested fields in the output fieldset
   for (auto targetVariable : neededVars.variables()) {
      if (std::find(fieldSetFieldNames.begin(), fieldSetFieldNames.end(), targetVariable) == fieldSetFieldNames.end()) {
         oops::Log::error() << "Field '" << targetVariable <<
            "' is not allocated the fieldset. Calling function needs to do this. Vader can't." << std::endl;
      }
      else {
         getVariable(afieldset, neededVars, targetVariable);
      }
   }
   oops::Log::debug() << "neededVars remaining after Vader::changeVar: " << neededVars << std::endl;
   oops::Log::trace() << "leaving Vader::changeVar: " << std::endl;
}
// -----------------------------------------------------------------------------
bool Vader::getVariable(atlas::FieldSet * afieldset, oops::Variables & neededVars, const std::string variableName) const {

   atlas::Field field1 = afieldset->field(variableName);
   bool variableFilled = false;

   oops::Log::trace() << "entering Vader::getVariable for variable: " << variableName << std::endl;

   // Look in the cookbook for a recipe
   auto recipeList = cookbook_.find(variableName);

   // recipeList->second is the cookbook vector of unique_ptr's to Recipe objects that produce 'variableName'
   // (Because of the compiler restrictions on unique_ptr's I don't know if we can conveniently rename this for clarity.)
   if ((recipeList != cookbook_.end()) && !recipeList->second.empty()) {
      oops::Log::debug() << "Vader cookbook contains at least one recipe for '" << variableName << std::endl;
      auto fieldSetFieldNames = afieldset->field_names();
      for (int i=0; i < recipeList->second.size(); ++i) {
            bool haveAllIngredients = true;
            for (auto ingredient : recipeList->second[i]->ingredients()) {
               oops::Log::debug() << "Checking if we have ingredient: " << ingredient << std::endl;
               bool haveThisIngredient = 
                  (std::find(fieldSetFieldNames.begin(), fieldSetFieldNames.end(), ingredient) != fieldSetFieldNames.end()) &&
                  (!neededVars.has(ingredient));
               if (!haveThisIngredient) {
                  // If we don't have an ingredient, see if we can make it from a recipe by calling this method recursively
                  haveThisIngredient = getVariable(afieldset, neededVars, ingredient);
               }
               haveAllIngredients = haveAllIngredients && haveThisIngredient;
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
            neededVars -= variableName;
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

