/*
 * (C) Copyright 2022 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef SRC_VADER_VADERPARAMETERS_H_
#define SRC_VADER_VADERPARAMETERS_H_

#include <vector>

#include "oops/util/parameters/OptionalParameter.h"
#include "oops/util/parameters/Parameter.h"
#include "RecipeBase.h"

namespace vader {

class VaderParameters : public oops::Parameters {
  OOPS_CONCRETE_PARAMETERS(VaderParameters, Parameters)

 public:
  /// An optional `recipeParams` parameter, which contains an (optional)
  /// list of Parameter (wrappers), each of which contains parameters
  /// which configure one of the individual recipes.
  /// Recipies should be designed so that they can be used with a
  /// default configuration and no parameters.
  oops::OptionalParameter<std::vector<RecipeParametersWrapper>> recipeParams{
     "recipe parameters",
     "Parameters to configure individual recipe functionality",
     this};
};

}  // namespace vader

#endif  // SRC_VADER_VADERPARAMETERS_H_
