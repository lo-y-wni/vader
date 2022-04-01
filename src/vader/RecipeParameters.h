/*
 * (C) Copyright 2022 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef VADER_RECIPEPARAMETERS_H_
#define VADER_RECIPEPARAMETERS_H_

#include <vector>

#include "oops/util/parameters/OptionalPolymorphicParameter.h"
#include "oops/util/parameters/RequiredParameter.h"

namespace vader {

class RecipeParameters : public oops::Parameters {
  OOPS_ABSTRACT_PARAMETERS(RecipeParameters, Parameters)

 public:
  oops::RequiredParameter<std::string> name{
     "name",
     "Factory name of recipe",
     this};

};

class RecipeParametersWrapper : public oops::Parameters {
  OOPS_CONCRETE_PARAMETERS(RecipeParametersWrapper, Parameters)

 public:
  oops::OptionalPolymorphicParameter<RecipeParameters, RecipeFactory> recipe{
     "name",
     this};

};

}  // namespace vader

#endif  // VADER_RECIPEPARAMETERS_H_