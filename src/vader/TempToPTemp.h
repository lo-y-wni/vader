/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef TEMP_TO_PTEMP_RECIPE_H_
#define TEMP_TO_PTEMP_RECIPE_H_

#include <vector>

#include "atlas/field/FieldSet.h"
#include "recipe.h"

namespace vader {

// -----------------------------------------------------------------------------
/// Recipe base class

class TempToPTempRecipe : public Recipe {
 public:
   const static std::string Name;
   const static std::vector<std::string> Ingredients;

   explicit TempToPTempRecipe();
   // TempToPTempRecipe(std::vector<std::string> ingredients, std::function<int(atlas::FieldSet *)> execute);
   // ~TempToPTempRecipe();

   std::string name() override;
   std::vector<std::string> ingredients() override;
   int execute(atlas::FieldSet *) override;

 private:
};

} // namespace vader

#endif  // TEMP_TO_PTEMP_RECIPE_H_
