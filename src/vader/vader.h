/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef SRC_VADER_VADER_H_
#define SRC_VADER_VADER_H_

#include <unordered_map>
#include <memory>
#include <vector>
#include <string>

#include "atlas/field/FieldSet.h"
#include "oops/base/Variables.h"
#include "VaderParameters.h"

#include "RecipeBase.h"

namespace vader {

// -----------------------------------------------------------------------------
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
 *           recipe. The 'cookbook' is the container (an unordered_map) that
 *           contains the recipes to be attempted when specified output variable
 *           is desired. The cookbook can contain multiple recipes that produce
 *           the same output variable.
 */

class Vader {
 public:
    static const std::string classname() {return "Vader";}
    explicit Vader(const VaderParameters & parameters);
    ~Vader();

    /// Calculates as many variables in the list as possible
    void changeVar(atlas::FieldSet * afieldset, oops::Variables &) const;

 private:
    std::unordered_map<std::string, std::vector<std::unique_ptr<RecipeBase>>>
        cookbook_;
    std::unordered_map<std::string, std::vector<std::string>>
        getDefaultCookbookDef();

    void createCookbook(std::unordered_map<std::string,
                        std::vector<std::string>>,
                        const boost::optional<std::vector<RecipeParametersBase>> &);
    bool getVariable(atlas::FieldSet * afieldset,
                     oops::Variables & neededVars,
                     const std::string targetVariable) const;
};

}  // namespace vader

#endif  // SRC_VADER_VADER_H_
