/*
 * (C) Copyright 2021-2022 UCAR
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
#include "vader/RecipeBase.h"
#include "oops/util/parameters/Parameter.h"
#include "oops/util/parameters/RequiredParameter.h"

namespace vader {

class TempToPTempParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(TempToPTempParameters, RecipeParametersBase)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
  oops::Parameter<double> p0{"p0", "p-naught", 1000.0, this};
  oops::Parameter<double> kappa{"kappa", "kappa", 0.2857, this};
};

// -----------------------------------------------------------------------------
/*! \brief TempToPTemp class defines a recipe for potential temperature
 *
 *  \details This instantiation of RecipeBase produces potential temperature
 *           using temperature and surface pressure as inputs. The parameters
 *           p0 and kappa can be specified via the constructor configuration.
 *           If they are not, the code will attempt to provide default values.
 *           (See https://glossary.ametsoc.org/wiki/Potential_temperature)
 */
class TempToPTemp : public RecipeBase {
 public:
    static const std::string Name;
    static const std::vector<std::string> Ingredients;

    typedef TempToPTempParameters Parameters_;

    TempToPTemp();
    explicit TempToPTemp(const Parameters_ &);

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
