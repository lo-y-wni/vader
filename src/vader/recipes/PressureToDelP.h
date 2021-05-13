/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef PRESSURE_TO_DELP_RECIPE_H_
#define PRESSURE_TO_DELP_RECIPE_H_

#include <vector>

#include "atlas/field/FieldSet.h"
#include "vader/recipe.h"

namespace vader {

// -----------------------------------------------------------------------------
/// Recipe base class

class PressureToDelPRecipe : public Recipe {
   public:
      const static std::string Name;
      const static std::vector<std::string> Ingredients;

      explicit PressureToDelPRecipe(const eckit::Configuration &);

      std::string name() const override;
      std::vector<std::string> ingredients() const override;
      int execute(atlas::FieldSet *) override;

   private:
};

} // namespace vader

#endif  // PRESSURE_TO_DELP_RECIPE_H_
