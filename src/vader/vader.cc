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
#include "oops/util/Timer.h"
#include "vader/vader/vader.h"
#include "vader/vader/cookbook.h"

namespace vader {

// -----------------------------------------------------------------------------
Vader::~Vader() {
    oops::Log::trace() << "Vader::~Vader done" << std::endl;
}
// -----------------------------------------------------------------------------
void Vader::createCookbook(std::unordered_map<std::string,
                                              std::vector<std::string>>
                                             definition,
                           const boost::optional<std::vector<RecipeParametersBase>> & allRecipeParams) {
    oops::Log::trace() << "entering Vader::createCookbook" << std::endl;
    std::vector<std::unique_ptr<RecipeBase>> recipes;
    for (auto defEntry : definition) {
        recipes.clear();
        for (auto recipeName : defEntry.second) {
            // There might not be any recipe parameters at all.
            // There might not be parameters for THIS recipe.
            // We must prepare for all eventualities.
            bool parametersFound = false;
            // boost::optional<RecipeParametersBase> recipeParams = boost::none;
            if (allRecipeParams != boost::none) {
                for (auto & singleRecipeParams : *allRecipeParams) {
                    if (singleRecipeParams.name.value() == recipeName) {
                        recipes.push_back(std::unique_ptr<RecipeBase>
                              (RecipeFactory::create(recipeName, singleRecipeParams)));
                        parametersFound = true;
                        break;
                    }
                }
            }
            if (!parametersFound) {
                auto emptyRecipeParams = RecipeFactory::createParameters(recipeName);
                        recipes.push_back(std::unique_ptr<RecipeBase>
                              (RecipeFactory::create(recipeName, *emptyRecipeParams)));
            }
        }
        cookbook_[defEntry.first] = std::move(recipes);
    }
    oops::Log::trace() << "leaving Vader::createCookbook" << std::endl;
}
// -----------------------------------------------------------------------------
Vader::Vader(const VaderParameters & parameters) {
    util::Timer timer(classname(), "Vader");
    std::unordered_map<std::string, std::vector<std::string>> definition =
        getDefaultCookbookDef();
    oops::Log::trace() << "entering Vader::Vader(parameters) " << std::endl;
    oops::Log::debug() << "Vader::Vader parameters = " << parameters << std::endl;

    // TODO::Configuration can alter the default cookbook here
    createCookbook(definition, parameters.recipeParams);
}
// -----------------------------------------------------------------------------
/*! \brief Change Variable
*
* \details **changeVar** is the only method of Vader that will be called
* externally. The caller passes an Atlas FieldSet that contains two kinds
* of fields:
* * Fields that have already been populated with values
* * Fields that have been allocated but need to be calculated and populated
* The already-populated fields serve as the ingredients for recipes which then
* populate fields. The names of the variables that still need to be
* populated are passed via the neededVars parameter. After this method is
* complete, Vader will have popluated all the variables it can based on
* the ingredients it was given and the recipes in its cookbook. The names of the
* variables it was able to populate will have been removed from the neededVars
* list. Any variable names remaining in neededVars remain unpopulated.
*
* \param[in,out] afieldset This is the FieldSet described above
* \param[in,out] neededVars Names of unpopulated Fields in afieldset
*
*/
void Vader::changeVar(atlas::FieldSet * afieldset,
                      oops::Variables & neededVars) const {
    util::Timer timer(classname(), "changeVar");
    oops::Log::trace() << "entering Vader::changeVar " << std::endl;
    oops::Log::debug() << "neededVars passed to Vader::changeVar: "
        << neededVars << std::endl;

    oops::Variables ingredients{};

    auto fieldSetFieldNames = afieldset->field_names();
    // Loop through all the requested fields in neededVars
    // Since neededVars can be modified by getVariable and getVariable calls
    // itself recursively, we make a copy of the list here before we start.
    std::vector<std::string> targetVariables{neededVars.variables()};

    for (auto targetVariable : targetVariables) {
        oops::Log::debug() <<
            "Vader::changeVar calling Vader::getVariable for: "
            << targetVariable << std::endl;
        getVariable(afieldset, neededVars, targetVariable);
    }
    oops::Log::debug() << "neededVars remaining after Vader::changeVar: " <<
        neededVars << std::endl;
    oops::Log::trace() << "leaving Vader::changeVar: " << std::endl;
}
// -----------------------------------------------------------------------------
/*! \brief Get Variable
*
* \details **getVariable** contains Vader's primary algorithm for attempting to
* populate an unpopulated field. It:
* * Checks the cookbook for recipes for the desired field (the targetVariable)
* * Checks each recipe to see if its required ingredients have been provided
* * If an ingredient is missing, recursively calls itself to attempt to get it
* * Executes the recipe if all ingredients are provided (or created)
* * If successful, removes the targetVariable from neededVars and returns 'true'
*
* \param[in,out] afieldset A fieldset containg both populated and unpopulated fields
* \param[in,out] neededVars Names of unpopulated Fields in afieldset
* \param[in] targetVariable variable name this instance is trying to populate
* \return boolean 'true' if it successfully populates targetVariable, else false
*
*/
bool Vader::getVariable(atlas::FieldSet * afieldset,
                        oops::Variables & neededVars,
                        const std::string targetVariable) const {
    bool variableFilled = false;

    oops::Log::trace() << "entering Vader::getVariable for variable: " <<
        targetVariable << std::endl;

    auto fieldSetFieldNames = afieldset->field_names();

    if (std::find(fieldSetFieldNames.begin(),
        fieldSetFieldNames.end(), targetVariable) == fieldSetFieldNames.end()) {
        oops::Log::debug() << "Field '" << targetVariable <<
            "' is not allocated the fieldset. Vader cannot make it."
            << std::endl;
        oops::Log::trace() << "leaving Vader::getVariable for variable: " <<
            targetVariable << std::endl;
        return false;
    }

    // Since this function is called recursively, make sure targetVariable is
    // still needed
    if (!neededVars.has(targetVariable)) {
        oops::Log::debug() << targetVariable <<
            " is no longer in the variable list neededVars." << std::endl;
        oops::Log::trace() << "leaving Vader::getVariable for variable: "
            << targetVariable << std::endl;
        return true;
    }

    auto recipeList = cookbook_.find(targetVariable);

    // If recipeList is found, recipeList->second is a vector of unique_ptr's
    // to Recipe objects that produce 'variableName'
    if ((recipeList != cookbook_.end()) && !recipeList->second.empty()) {
        oops::Log::debug() <<
            "Vader cookbook contains at least one recipe for '" <<
            targetVariable << "'" << std::endl;
        for (int i=0; i < recipeList->second.size(); ++i) {
            oops::Log::debug() <<
                "Checking to see if we have ingredients for recipe: " <<
                recipeList->second[i]->name() << std::endl;
            bool haveIngredient = false;
            for (auto ingredient : recipeList->second[i]->ingredients()) {
                if (ingredient == targetVariable) {
                    oops::Log::error() << "Error: Ingredient list for " <<
                        recipeList->second[i]->name() << " contains the target."
                        << std::endl;
                    // This could cause infinite recursion if we didn't check.
                    // TODO: infinite recursion probably still possible with
                    //       badly-constructed cookbook.
                    break;
                }
                haveIngredient =
                    (std::find(fieldSetFieldNames.begin(),
                               fieldSetFieldNames.end(),
                               ingredient) != fieldSetFieldNames.end()) &&
                    (!neededVars.has(ingredient));
                if (!haveIngredient) {
                    oops::Log::debug() << "ingredient " << ingredient <<
                        " not found. Recursively checking if Vader can make it."
                        << std::endl;
                    haveIngredient = getVariable(afieldset, neededVars,
                                                 ingredient);
                }
                oops::Log::debug() << "ingredient " << ingredient <<
                    (haveIngredient ? " is" : " is not") << " available." <<
                    std::endl;
                if (!haveIngredient) break;
            }
            if (haveIngredient) {
                oops::Log::debug() <<
                    "All ingredients are in the fieldset. Executing the recipe."
                    << std::endl;
                // Potentially a recipe might require set-up that should only be
                // performed the first time
                if (recipeList->second[i]->requiresSetup()) {
                    recipeList->second[i]->setup(afieldset);
                }
                variableFilled = recipeList->second[i]->execute(afieldset);
                if (variableFilled) {
                    neededVars -= targetVariable;
                    oops::Log::debug() << "Vader successfully executed recipe: "
                    << recipeList->second[i]->name() << std::endl;
                    break;
                } else {
                    oops::Log::debug() <<
                        "Vader tried but failed to execute recipe: "
                        << recipeList->second[i]->name() << std::endl;
                }
            } else {
                oops::Log::debug() <<
                    "Do not have all the ingredients for this recipe." <<
                    std::endl;
            }
        }
    } else {
        oops::Log::debug() << "Vader cookbook does not contain a recipe for: "
            << targetVariable << std::endl;
    }
    oops::Log::trace() << "leaving Vader::getVariable for variable: " <<
        targetVariable << std::endl;
    return variableFilled;
}

}  // namespace vader
