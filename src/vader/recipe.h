/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef RECIPE_H_
#define RECIPE_H_

#include <vector>

#include "atlas/field/FieldSet.h"

namespace vader {

// -----------------------------------------------------------------------------
/// Recipe base class

class Recipe {
   public:
      // explicit Recipe();
      // Recipe(std::vector<std::string> ingredients, std::function<int(atlas::FieldSet *)> execute);
      virtual ~Recipe() { };

      virtual std::string name() = 0; //Derived classes should have a static class name property that is also returned by this function
      virtual std::vector<std::string> ingredients() = 0;
      virtual bool requiresSetup() { return false; }
      virtual int setup(atlas::FieldSet *) { return 0; }
      virtual int execute(atlas::FieldSet *) = 0;

   private:
};

} // namespace vader

#endif  // RECIPE_H_
