/*
 * (C) Copyright 2021  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>
#include <map>
#include <vector>

#include "vader/recipe.h"

namespace vader
{

// ------------------------------------------------------------------------------------------------
Recipe::Recipe(const std::string name, const Parameters_ & paramWrapper,
               const VaderConfigVars & configVariables)
    : recipe_{RecipeFactory::create(name, paramWrapper.recipeParams, configVariables)} {}
// ------------------------------------------------------------------------------------------------
std::string Recipe::name() const
{
    return recipe_->name();
}
// ------------------------------------------------------------------------------------------------
const oops::Variable Recipe::product() const
{
    return recipe_->product();
}
// ------------------------------------------------------------------------------------------------
const oops::Variables Recipe::ingredients() const
{
    return recipe_->ingredients();
}
// ------------------------------------------------------------------------------------------------
size_t Recipe::productLevels(const atlas::FieldSet & afieldset) const
{
    return recipe_->productLevels(afieldset);
}
// ------------------------------------------------------------------------------------------------
atlas::FunctionSpace Recipe::productFunctionSpace(const atlas::FieldSet & afieldset) const
{
    return recipe_->productFunctionSpace(afieldset);
}
// ------------------------------------------------------------------------------------------------
bool Recipe::hasTLAD() const
{
    return recipe_->hasTLAD();
}
// ------------------------------------------------------------------------------------------------
void Recipe::executeNL(atlas::FieldSet & afieldset)
{
    recipe_->executeNL(afieldset);
}
// ------------------------------------------------------------------------------------------------
void Recipe::executeTL(atlas::FieldSet & afieldsetTL, const atlas::FieldSet & afieldsetTraj)
{
    recipe_->executeTL(afieldsetTL, afieldsetTraj);
}
// ------------------------------------------------------------------------------------------------
void Recipe::executeAD(atlas::FieldSet & afieldsetAD, const atlas::FieldSet & afieldsetTraj)
{
    recipe_->executeAD(afieldsetAD, afieldsetTraj);
}
// ------------------------------------------------------------------------------------------------
void Recipe::print(std::ostream &os) const
{
    os << *recipe_;
}

}  // namespace vader
