/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef VADER_H_
#define VADER_H_

#include <unordered_map>

#include "atlas/field/FieldSet.h"
#include "oops/base/Variables.h"

#include "recipe.h"

namespace vader {

// -----------------------------------------------------------------------------
/// Vader class

class Vader {
 public:
    explicit Vader();
    Vader(const eckit::Configuration & config);
    ~Vader();

    void changeVar(atlas::FieldSet * afieldset, const oops::Variables &) const;
    static Recipe * recipeFactory(std::string recipeName);
    static const std::unordered_map<std::string, std::vector<std::string>> defaultCookbookDefinition;
 private:
    std::unordered_map<std::string, std::vector<Recipe *>> cookbook_;

    std::unordered_map<std::string, std::vector<std::string>> getDefaultCookbookDef();
    void createCookbook(std::unordered_map<std::string, std::vector<std::string>> definition);

    int getVariable(atlas::FieldSet * afieldset, const std::string variableName) const;

};

} // namespace vader

#endif  // VADER_H_
