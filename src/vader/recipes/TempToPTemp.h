/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef TEMP_TO_PTEMP_RECIPE_H_
#define TEMP_TO_PTEMP_RECIPE_H_

#include <vector>

#include "eckit/config/Configuration.h"
#include "atlas/field/FieldSet.h"
#include "vader/recipe.h"

namespace vader {

// -----------------------------------------------------------------------------
/// Recipe base class

class TempToPTempRecipe : public Recipe {
   public:
      const static std::string Name;
      const static std::vector<std::string> Ingredients;

      explicit TempToPTempRecipe(const eckit::Configuration &);

      double p0() const { return p0_; } 
      double kappa() const { return kappa_; }

      // Recipe base class overrides
      std::string name() const override;
      std::vector<std::string> ingredients() const override;
      int execute(atlas::FieldSet *) override;

   private:
      double p0_;
      double kappa_;
};

} // namespace vader

#endif  // TEMP_TO_PTEMP_RECIPE_H_
