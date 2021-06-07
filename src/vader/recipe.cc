/*
 * (C) Copyright 2021  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>
#include <vector>

#include "recipe.h"

namespace vader {

// -----------------------------------------------------------------------------
Recipe::Recipe(const std::string name, const eckit::Configuration & conf)
            : recipe_{RecipeFactory::create(name, conf)} { }
// -----------------------------------------------------------------------------
std::string Recipe::name() const {
   return recipe_->name();
}
// -----------------------------------------------------------------------------
std::vector<std::string> Recipe::ingredients() const {
   return recipe_->ingredients();
}
// -----------------------------------------------------------------------------
bool Recipe::requiresSetup() const {
   return recipe_->requiresSetup();
}
// -----------------------------------------------------------------------------
bool Recipe::setup(atlas::FieldSet *afieldset) {
   return recipe_->setup(afieldset);
}
// -----------------------------------------------------------------------------
bool Recipe::execute(atlas::FieldSet *afieldset) {
   return recipe_->execute(afieldset);
}
// -----------------------------------------------------------------------------
void Recipe::print(std::ostream & os) const {
  os << *recipe_;
}

}  // namespace vader

