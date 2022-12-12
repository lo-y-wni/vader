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
#include "atlas/functionspace/PointCloud.h"
#include "atlas/util/Point.h"
#include "oops/util/parameters/Parameter.h"
#include "oops/util/parameters/RequiredParameter.h"
#include "vader/RecipeBase.h"

namespace vader {

class TestRecipeParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(TestRecipeParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

// ------------------------------------------------------------------------------------------------
/*! \brief TestVarA_A class defines a recipe for TestVarA
 *
 *  \details This instantiation of RecipeBase produces TestVarA
 *           using TestVarB as input. Non-linear recipe only.
 */
class TestVarA_A : public RecipeBase {
 public:
    static const char Name[];
    static const std::vector<std::string> Ingredients;

    typedef TestRecipeParameters Parameters_;

    explicit TestVarA_A(const Parameters_ &);

    // Recipe base class overrides
    std::string name() const override;
    std::string product() const override {return "TestVarA";}
    std::vector<std::string> ingredients() const override {return {"TestVarB"};}
    size_t productLevels(const atlas::FieldSet &) const override {return 0;}
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const override
                                {return atlas::functionspace::PointCloud({atlas::PointXY({0.0})});}
    // bool hasTLAD() const override { return true; }
    bool executeNL(atlas::FieldSet &) override;
    // bool executeTL(atlas::FieldSet &, const atlas::FieldSet &) override;
    // bool executeAD(atlas::FieldSet &, const atlas::FieldSet &) override;

 private:
};

}  // namespace vader
