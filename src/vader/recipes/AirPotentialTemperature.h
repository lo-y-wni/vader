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
#include "atlas/functionspace/FunctionSpace.h"
#include "oops/util/parameters/Parameter.h"
#include "oops/util/parameters/RequiredParameter.h"
#include "vader/RecipeBase.h"

namespace vader {

class AirPotentialTemperature_AParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(AirPotentialTemperature_AParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
  oops::Parameter<double> p0{"p0", "p-naught", -1.0, this};
  oops::Parameter<double> kappa{"kappa", "kappa", 0.2857, this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief AirPotentialTemperature_A class defines a recipe for potential temperature
 *
 *  \details This instantiation of RecipeBase produces potential temperature
 *           using temperature and surface pressure as inputs. The parameters
 *           p0 and kappa can be specified via the constructor configuration.
 *           If they are not, the code will attempt to provide default values.
 *           (See https://glossary.ametsoc.org/wiki/Potential_temperature)
 */
class AirPotentialTemperature_A : public RecipeBase {
 public:
    static const char Name[];
    static const std::vector<std::string> Ingredients;

    typedef AirPotentialTemperature_AParameters Parameters_;

    explicit AirPotentialTemperature_A(const Parameters_ &);

    // Recipe base class overrides
    std::string name() const override;
    std::string product() const override;
    std::vector<std::string> ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    bool executeNL(atlas::FieldSet &) override;

 private:
    double p0_;
    const double kappa_;
};

}  // namespace vader
