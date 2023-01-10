/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "atlas/field/FieldSet.h"
#include "oops/base/Variables.h"
#include "oops/util/Printable.h"
#include "RecipeBase.h"
#include "VaderParameters.h"


namespace vader {

// ------------------------------------------------------------------------------------------------
/*! \brief Vader class to handle variable transformations
 *
 *  \details This class provides generic variable transformations via the
 *           changeVar method which is passed an atlas fieldset.
 *
 *           Throughout the Vader code, the primary variables are named using a
 *           metaphor involving ingredients, recipies, and a cookbook.
 *
 *           A 'recipe' is is an object that can produce a single output
 *           variable when provided with a list of required input variables.
 *           The input variables are referred to as the 'ingredients' to the
 *           recipe. The 'cookbook' is the container (a map) that
 *           contains the recipes to be attempted when specified output variable
 *           is desired. The cookbook can contain multiple recipes that produce
 *           the same output variable.
 */

class Vader  : public util::Printable {
 public:
    static const std::string classname() {return "Vader";}
    typedef  std::vector<std::pair<std::string,
                                   const std::unique_ptr<RecipeBase> & >> vaderPlanType;
    explicit Vader(const VaderParameters & parameters);
    Vader(const Vader &) = delete;
    Vader& operator=(const Vader &) = delete;
    ~Vader();

    std::vector<std::string> getPlanNames() const;
    std::vector<std::string> getPlanNames(vaderPlanType plan) const;

    /// Calculates as many variables in the list as possible
    oops::Variables changeVar(atlas::FieldSet &, oops::Variables &,
                              vaderPlanType plan = vaderPlanType()) const;
    oops::Variables changeVarTraj(atlas::FieldSet &, oops::Variables &);
    oops::Variables changeVarTL(atlas::FieldSet &, oops::Variables &) const;
    oops::Variables changeVarAD(atlas::FieldSet &, oops::Variables &) const;

 private:
    std::map<std::string, std::vector<std::unique_ptr<RecipeBase>>> cookbook_;
    vaderPlanType recipeExecutionPlan_;
    atlas::FieldSet trajectory_;
    std::map<std::string, std::vector<std::string>>
        getDefaultCookbookDef();

    void createCookbook(std::map<std::string, std::vector<std::string>>,
                        const std::vector<RecipeParametersWrapper> & allRecpParamWraps =
                              std::vector<RecipeParametersWrapper>());

    bool planVariable(const std::vector<std::string> &,
                      oops::Variables &,
                      const std::string,
                      bool,
                      oops::Variables &,
                      vaderPlanType &) const;

    void executePlanNL(atlas::FieldSet &, const vaderPlanType &) const;
    void executePlanTL(atlas::FieldSet &, const vaderPlanType &) const;
    void executePlanAD(atlas::FieldSet &, const vaderPlanType &) const;
    void print(std::ostream &) const;
};

}  // namespace vader
