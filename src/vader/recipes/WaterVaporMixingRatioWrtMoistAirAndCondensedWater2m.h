/*
 * (C) Crown Copyright 2023 Met Office.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <map>
#include <string>
#include <vector>

#include "atlas/field/FieldSet.h"
#include "atlas/functionspace/FunctionSpace.h"
#include "oops/util/parameters/Parameter.h"
#include "oops/util/parameters/RequiredParameter.h"
#include "vader/RecipeBase.h"

namespace vader {

class WaterVaporMixingRatioWrtMoistAirAndCondensedWater2m_AParameters :
                                            public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(WaterVaporMixingRatioWrtMoistAirAndCondensedWater2m_AParameters,
                           RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief WaterVaporMixingRatioWrtMoistAirAndCondensedWater2m_A class defines a recipe for water
 *         vapor mixing ratio wrt moist air and condensed water at 2m
 *
 *  \details This instantiation of RecipeBase produces the water vapor mixing ratio wrt moist air
 *          and condensed water at 2m using the atmospheric specific humidity at the lowest level
 */
class WaterVaporMixingRatioWrtMoistAirAndCondensedWater2m_A : public RecipeBase {
 public:
    static const char Name[];
    static const oops::Variables Ingredients;

    typedef WaterVaporMixingRatioWrtMoistAirAndCondensedWater2m_AParameters Parameters_;

    WaterVaporMixingRatioWrtMoistAirAndCondensedWater2m_A(const Parameters_ &,
                                                          const VaderConfigVars &);

    // Recipe base class overrides
    std::string name() const override;
    oops::Variable product() const override;
    oops::Variables ingredients() const override;
    size_t productLevels(const atlas::FieldSet &) const override;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override;
    bool hasTLAD() const override { return true; }
    void executeNL(atlas::FieldSet &) override;
    void executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
    void executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;

 private:
};

}  // namespace vader
