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

#include "RecipeBase.h"

namespace vader {

// -----------------------------------------------------------------------------
/// Vader class

class Vader {
 public:
    explicit Vader(const eckit::Configuration & config);
    ~Vader();

    void changeVar(atlas::FieldSet * afieldset, oops::Variables &) const;

 private:
    std::unordered_map<std::string, std::vector<std::unique_ptr<RecipeBase>>>
        cookbook_;
    std::unordered_map<std::string, std::vector<std::string>>
        getDefaultCookbookDef();

    void createCookbook(std::unordered_map<std::string,
                        std::vector<std::string>>,
                        const eckit::Configuration &);
    bool getVariable(atlas::FieldSet * afieldset,
                     oops::Variables & neededVars,
                     const std::string targetVariable) const;
};

}  // namespace vader

#endif  // SRC_VADER_VADER_H_
