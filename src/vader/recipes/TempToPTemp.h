/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef SRC_VADER_RECIPES_TEMPTOPTEMP_H_
#define SRC_VADER_RECIPES_TEMPTOPTEMP_H_

#include <vector>
#include <string>

#include "eckit/config/Configuration.h"
#include "atlas/field/FieldSet.h"
#include "vader/vader/RecipeBase.h"

namespace vader {

// -----------------------------------------------------------------------------
/// Recipe base class

class TempToPTemp : public RecipeBase {
 public:
    static const std::string Name;
    static const std::vector<std::string> Ingredients;

    explicit TempToPTemp(const eckit::Configuration &);

    // Recipe base class overrides
    std::string name() const override;
    std::vector<std::string> ingredients() const override;
    bool execute(atlas::FieldSet *) override;

 private:
    double p0_;
    const double kappa_;
};

}  // namespace vader

#endif  // SRC_VADER_RECIPES_TEMPTOPTEMP_H_
