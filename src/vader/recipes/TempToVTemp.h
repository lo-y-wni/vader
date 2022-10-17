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
#include "oops/util/parameters/Parameter.h"
#include "oops/util/parameters/RequiredParameter.h"
#include "vader/RecipeBase.h"

namespace vader {

class TempToVTempParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(TempToVTempParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
  oops::Parameter<double> epsilon{"epsilon", "epsilon", 0.62196, this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief TempToVTemp class defines a recipe for virtual temperature
 *
 *  \details This instantiation of RecipeBase produces virtual temperature
 *           using temperature and specific humidity as inputs. The parameter
 *           epsilon can be specified via the constructor configuration.
 *           If it is not, the code will provide a default value.
 */
class TempToVTemp : public RecipeBase {
 public:
    static const char Name[];
    static const std::vector<std::string> Ingredients;

    typedef TempToVTempParameters Parameters_;

    TempToVTemp();
    explicit TempToVTemp(const Parameters_ &);

    // Recipe base class overrides
    std::string name() const override;
    std::vector<std::string> ingredients() const override;
    bool hasTLAD() const override { return true; }
    bool executeNL(atlas::FieldSet &) override;
    bool executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
    bool executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;

 private:
    const double epsilon_;
};

}  // namespace vader
