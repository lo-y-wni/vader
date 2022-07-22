/*
 * (C) Copyright 2021  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <algorithm>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "oops/util/Logger.h"
#include "oops/util/Timer.h"
#include "vader/cookbook.h"
#include "vader/vader.h"

namespace vader {

// ------------------------------------------------------------------------------------------------
Vader::~Vader() {
    oops::Log::trace() << "Vader::~Vader done" << std::endl;
}
// ------------------------------------------------------------------------------------------------
void Vader::createCookbook(std::unordered_map<std::string,
                                              std::vector<std::string>>
                                              definition,
                        const std::vector<RecipeParametersWrapper> &
                    allRecpParamWraps) {
    oops::Log::trace() << "entering Vader::createCookbook" << std::endl;
    std::vector<std::unique_ptr<RecipeBase>> recipes;
    for (auto defEntry : definition) {
        recipes.clear();
        for (auto recipeName : defEntry.second) {
            // There might not be any recipe parameters at all.
            // There might not be parameters for THIS recipe.
            // We must prepare for all eventualities.
            bool parametersFound = false;
            for (auto & singleRecpParamWrap : allRecpParamWraps) {
                if (singleRecpParamWrap.recipeParams.value().name.value()
                                                            == recipeName) {
                    recipes.push_back(std::unique_ptr<RecipeBase>
                        (RecipeFactory::create(recipeName,
                            singleRecpParamWrap.recipeParams)));
                    parametersFound = true;
                    break;
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
// ------------------------------------------------------------------------------------------------
Vader::Vader(const VaderParameters & parameters) {
    util::Timer timer(classname(), "Vader");
    // TODO(vahl): Parameters can alter the default cookbook here
    std::unordered_map<std::string, std::vector<std::string>> definition =
        getDefaultCookbookDef();
    oops::Log::trace() << "entering Vader::Vader(parameters) " << std::endl;
    oops::Log::debug() << "Vader::Vader parameters = " << parameters << std::endl;

    // Vader is designed to function without parameters. So VaderParameters
    // should not have any RequiredParameters.
    //
    // To simplify things for vader clients, they should declare vader Parameters with a
    // default construction of empty/default VaderParameters. i.e. their Parameters should contain:
    // oops::Parameter<vader::VaderParameters> vader{"vader", {}, this};
    //
    if (parameters.recipeParams.value() == boost::none) {
        createCookbook(definition);
    } else {
        createCookbook(definition, *parameters.recipeParams.value());
    }
}
// ------------------------------------------------------------------------------------------------
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
void Vader::changeVar(atlas::FieldSet & afieldset,
                      oops::Variables & neededVars) const {
    util::Timer timer(classname(), "changeVar");
    oops::Log::trace() << "entering Vader::changeVar " << std::endl;
    oops::Log::debug() << "neededVars passed to Vader::changeVar: " << neededVars << std::endl;

    oops::Variables ingredients{};

    auto fieldSetFieldNames = afieldset.field_names();
    // Loop through all the requested fields in neededVars
    // Since neededVars can be modified by planVariable and planVariable calls
    // itself recursively, we make a copy of the list here before we start.
    std::vector<std::string> targetVariables{neededVars.variables()};
    std::vector<std::pair<std::string, std::string>> plan;

    for (auto targetVariable : targetVariables) {
        oops::Log::debug() <<
            "Vader::changeVar calling Vader::planVariable for: "
            << targetVariable << std::endl;
        planVariable(afieldset, neededVars, targetVariable, plan);
    }
    executePlanNL(afieldset, plan);

    oops::Log::debug() << "neededVars remaining after Vader::changeVar: " << neededVars
        << std::endl;
    oops::Log::trace() << "leaving Vader::changeVar: " << std::endl;
}
// ------------------------------------------------------------------------------------------------
/*! \brief Plan Variable
*
* \details **planVariable** contains Vader's primary algorithm for attempting to
* populate an unpopulated field. It:
* * Checks the cookbook for recipes for the desired field (the targetVariable)
* * Checks each recipe to see if its required ingredients have been provided
* * If an ingredient is missing, recursively calls itself to attempt to get it
* * Adds the variable and recipe name to the "recipeExecutionPlan" if the recipe is viable.
* * If successful, removes the targetVariable from neededVars and returns 'true'
*
* \param[in,out] afieldset A fieldset containg both populated and unpopulated fields
* \param[in,out] neededVars Names of unpopulated Fields in afieldset
* \param[in] targetVariable variable name this instance is trying to populate
* \param[in,out] plan ordered list of viable recipes that will get exectued later
* \return boolean 'true' if it successfully creates a plan for targetVariable, else false
*
*/
bool Vader::planVariable(atlas::FieldSet & afieldset,
                         oops::Variables & neededVars,
                         const std::string targetVariable,
                         std::vector<std::pair<std::string, std::string>> & plan) const {
    bool variablePlanned = false;

    oops::Log::trace() << "entering Vader::planVariable for variable: " << targetVariable <<
        std::endl;

    auto fieldSetFieldNames = afieldset.field_names();

    if (std::find(fieldSetFieldNames.begin(),
        fieldSetFieldNames.end(), targetVariable) == fieldSetFieldNames.end()) {
        oops::Log::debug() << "Field '" << targetVariable <<
            "' is not allocated the fieldset. Vader cannot make it." << std::endl;
        oops::Log::trace() << "leaving Vader::planVariable for variable: " <<
            targetVariable << std::endl;
        return false;
    }

    // Since this function is called recursively, make sure targetVariable is
    // still needed
    if (!neededVars.has(targetVariable)) {
        oops::Log::debug() << targetVariable <<
            " is no longer in the variable list neededVars." << std::endl;
        oops::Log::trace() << "leaving Vader::planVariable for variable: "
            << targetVariable << std::endl;
        return true;
    }

    auto recipeList = cookbook_.find(targetVariable);

    // If recipeList is found, recipeList->second is a vector of unique_ptr's
    // to Recipe objects that produce 'variableName'
    if ((recipeList != cookbook_.end()) && !recipeList->second.empty()) {
        oops::Log::debug() <<
            "Vader cookbook contains at least one recipe for '" << targetVariable << "'" <<
            std::endl;
        for (int i=0; i < recipeList->second.size(); ++i) {
            oops::Log::debug() << "Checking to see if we have ingredients for recipe: " <<
                recipeList->second[i]->name() << std::endl;
            bool haveIngredient = false;
            for (auto ingredient : recipeList->second[i]->ingredients()) {
                if (ingredient == targetVariable) {
                    oops::Log::error() << "Error: Ingredient list for " <<
                        recipeList->second[i]->name() << " contains the target." << std::endl;
                    // This could cause infinite recursion if we didn't check.
                    // TODO(vahl): infinite recursion probably still possible
                    //       with badly-constructed cookbook.
                    break;
                }
                haveIngredient =
                    (std::find(fieldSetFieldNames.begin(), fieldSetFieldNames.end(), ingredient)
                        != fieldSetFieldNames.end()) && (!neededVars.has(ingredient));
                if (!haveIngredient) {
                    oops::Log::debug() << "ingredient " << ingredient <<
                        " not found. Recursively checking if Vader can make it." << std::endl;
                    haveIngredient = planVariable(afieldset, neededVars, ingredient, plan);
                }
                oops::Log::debug() << "ingredient " << ingredient <<
                    (haveIngredient ? " is" : " is not") << " available." << std::endl;
                if (!haveIngredient) break;
            }
            if (haveIngredient) {
                oops::Log::debug() <<
                    "All ingredients are in the fieldset. Adding recipe to recipeExecutionPlan." <<
                    std::endl;
                plan.push_back(std::pair<std::string, std::string> ({targetVariable,
                                                                  recipeList->second[i]->name()}));
                variablePlanned = true;
                neededVars -= targetVariable;
            } else {
                oops::Log::debug() << "Do not have all the ingredients for this recipe." <<
                    std::endl;
            }
        }
    } else {
        oops::Log::debug() << "Vader cookbook does not contain a recipe for: "
            << targetVariable << std::endl;
    }
    oops::Log::trace() << "leaving Vader::planVariable for variable: " << targetVariable <<
        std::endl;
    return variablePlanned;
}

/*! \brief Execute Plan (non-linear)
*
* \details **executePlanNL** calls, in order, the 'execute' (non-linear) method of the
* recipes specified in the recipeExecutionPlan that is passed in. (The recipeExecutionPlan is
* created through calls to planVariable.)
*
* \param[in,out] afieldset A fieldset containg both populated and unpopulated fields
* \param[in] recipeExecutionPlan ordered list of recipes that are to be exectued
*
*/
void Vader::executePlanNL(atlas::FieldSet & afieldset,
            const std::vector<std::pair<std::string, std::string>> & recipeExecutionPlan) const {
    oops::Log::trace() << "entering Vader::executePlanNL" <<  std::endl;
    // We must get the recipes specified in the recipeExecutionPlan out of the cookbook,
    // where they live
    for (auto varPlan : recipeExecutionPlan) {
        oops::Log::debug() << "Attempting to calculate variable " << varPlan.first <<
            " using recipe with name: " << varPlan.second << std::endl;
        ASSERT(afieldset.has_field(varPlan.first));
        auto recipeList = cookbook_.find(varPlan.first);
        size_t recipeIndex = 0;
        while (recipeIndex < recipeList->second.size() &&
               recipeList->second[recipeIndex]->name() != varPlan.second) recipeIndex++;
        ASSERT(recipeIndex < recipeList->second.size());
        for (auto ingredient :  recipeList->second[recipeIndex]->ingredients()) {
            ASSERT(afieldset.has_field(ingredient));
        }
        if (recipeList->second[recipeIndex]->requiresSetup()) {
            recipeList->second[recipeIndex]->setup(afieldset);
        }
        const bool recipeSuccess = recipeList->second[recipeIndex]->execute(afieldset);
        ASSERT(recipeSuccess);  // At least for now, we'll require the execution to be successful
    }
    oops::Log::trace() << "leaving Vader::executePlanNL" <<  std::endl;
}

}  // namespace vader
