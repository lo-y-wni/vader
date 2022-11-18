/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <string>
#include <vector>

#include "atlas/field/FieldSet.h"
#include "atlas/functionspace/FunctionSpace.h"
#include "oops/util/parameters/Parameter.h"
#include "oops/util/parameters/RequiredParameter.h"
#include "vader/RecipeBase.h"

namespace vader {

class AirTemperature_A_Parameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(AirTemperature_A_Parameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief AirTemperature_A class defines a recipe for air_temperature
 *         from air_potential_temperature and dimensionless_exner_function
 *
 *         NL air_temperature = air_potential_temperature * dimensionless_exner_function
 */
class AirTemperature_A : public RecipeBase {
 public:
    static const char Name[];
    static const std::vector<std::string> Ingredients;

    typedef AirTemperature_A_Parameters Parameters_;

    explicit AirTemperature_A(const Parameters_ &);

    // Recipe base class overrides
    std::string name() const override;
    std::string product() const override;
    std::vector<std::string> ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    bool hasTLAD() const override { return true; }
    bool executeNL(atlas::FieldSet &) override;
    bool executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
    bool executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;

 private:
};

}  // namespace vader

