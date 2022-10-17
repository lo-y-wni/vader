/*
 * (C) Copyright 2021-2022 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <string>
#include <vector>

#include "atlas/field/FieldSet.h"
#include "vader/RecipeBase.h"

namespace vader
{

class PressureToDelPParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(PressureToDelPParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
     // No parameters for this recipe currently
};

// ------------------------------------------------------------------------------------------------
/// Recipe base class

class PressureToDelP : public RecipeBase
{
 public:
    static const char Name[];
    static const std::vector<std::string> Ingredients;

    typedef PressureToDelPParameters Parameters_;

    PressureToDelP();
    explicit PressureToDelP(const Parameters_ &);

    std::string name() const override;
    std::vector<std::string> ingredients() const override;
    bool executeNL(atlas::FieldSet &) override;

 private:
};
}  // namespace vader
