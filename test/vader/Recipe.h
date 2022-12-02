/*
 * (C) Copyright 2022 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <netcdf.h>

#include <string>
#include <vector>

#define ECKIT_TESTING_SELF_REGISTER_CASES 0

#include "atlas/field.h"
#include "atlas/functionspace.h"
#include "atlas/grid.h"
#include "atlas/meshgenerator.h"

#include "eckit/config/LocalConfiguration.h"

#include "oops/runs/Test.h"
#include "oops/util/Logger.h"
#include "test/TestEnvironment.h"

#include "vader/RecipeBase.h"
#include "Utils.h"

namespace vader {
namespace test {

/// \brief Top-level options taken by the Recipe test.
class RecipeTestParameters : public oops::Parameters {
  OOPS_CONCRETE_PARAMETERS(RecipeTestParameters, Parameters)

 public:
  oops::RequiredParameter<vader::RecipeParametersWrapper> recipe{
        "recipe", "recipe parameters", this};
  oops::RequiredParameter<eckit::LocalConfiguration> grid{
        "trajectory grid", "trajectory grid description", this};
  oops::RequiredParameter<std::string> filename{"trajectory filename",
        "filename of existing netcdf file with trajectory ingredients", this};
  oops::RequiredParameter<double> tolerance{"adjoint test tolerance",
        "adjoint test tolerance", this};
};

// -----------------------------------------------------------------------------
/// \brief Tests adjoint of the recipe.
void testRecipeAdjoint() {
  RecipeTestParameters params;
  params.validateAndDeserialize(::test::TestEnvironment::config());

  // create recipe
  const auto & recipeParams = params.recipe.value().recipeParams.value();
  RecipeBase* recipe = RecipeFactory::create(recipeParams.name, recipeParams);
  const std::vector<std::string> ingredientVars = recipe->ingredients();
  const std::string productVar = recipe->product();
  oops::Log::info() << "Testing vader recipe (K): " << recipe->name() << std::endl;
  oops::Log::info() << " Ingredients (dx): " << ingredientVars << std::endl;
  oops::Log::info() << " Product (dy): " << productVar << std::endl;

  // set up grid and functionspace based on description in the yaml
  const atlas::StructuredGrid grid(params.grid.value());
  const atlas::functionspace::StructuredColumns fs(grid);

  // Open NetCDF file and read all the ingredients for the trajectory
  atlas::FieldSet traj;
  int ncid, retval;
  const std::string & filename = params.filename;
  oops::Log::info() << "Reading trajectory from file: " << filename << std::endl;
  if ((retval = nc_open(filename.c_str(), NC_NOWRITE, &ncid))) ERR(retval);
  std::vector<size_t> ingredientLevels(ingredientVars.size(), 0);
  for (size_t jvar = 0; jvar < ingredientVars.size(); ++jvar) {
    addFieldFromFile(traj, ingredientVars[jvar], fs, grid,
                     ingredientLevels[jvar], ncid);
  }

  // allocate field for the product in the trajectory (usually done in vader,
  // but here the recipes are tested outside of vader infrastructure)
  const size_t productLevels = recipe->productLevels(traj);
  addZeroField(traj, productVar, fs, productLevels);
  // run NL to set trajectory
  recipe->executeNL(traj);

  // Testing whether (dx, K^T dy) == (K dx, dy)
  // Allocating dxin to contain randomized dx (for the ingredient variables)
  // and Kdx (for the product variable) that will be filled in recipe.executeTL.
  atlas::FieldSet dxin;
  for (size_t jvar = 0; jvar < ingredientVars.size(); ++jvar) {
    addRandomField(dxin, ingredientVars[jvar], fs, ingredientLevels[jvar]);
  }
  addZeroField(dxin, productVar, fs, productLevels);
  // fill the product variable with Kdx
  recipe->executeTL(dxin, traj);

  // Allocating dxout to contain randomized dy (for the product variable)
  // and K^Tdy (for the ingredients variables) that will be updated after
  // recipe.executeAD call.
  atlas::FieldSet dxout;
  addRandomField(dxout, productVar, fs, productLevels);
  for (size_t jvar = 0; jvar < ingredientVars.size(); ++jvar) {
    addZeroField(dxout, ingredientVars[jvar], fs, ingredientLevels[jvar]);
  }
  // after calling recipe->executeAD, dy (the product variable) will be
  // zeroed out, so the copy is needed.
  atlas::Field dy(productVar, dxout.field(productVar).datatype(),
                  dxout.field(productVar).shape());
  auto from_view = atlas::array::make_view<double, 2>(dxout.field(productVar));
  auto to_view = atlas::array::make_view<double, 2>(dy);
  to_view.assign(from_view);
  // fill the ingredients variables with K^T dxout
  recipe->executeAD(dxout, traj);

  double zz1 = 0;
  // Compute (dx, K^T dy)
  for (const auto & ingredientVar : ingredientVars) {
    zz1 += dotProduct(dxin[ingredientVar], dxout[ingredientVar]);
  }
  // Compute (Kdx, dy)
  double zz2 = dotProduct(dxin[productVar], dy);

  oops::Log::info() << "<dx,KTdy>=" << zz1 << std::endl;
  oops::Log::info() << "<Kdx,dy>=" << zz2 << std::endl;
  oops::Log::info() << "<dx,KTdy>-<Kdx,dy>/<dx,KTdy>="
                    << (zz1-zz2)/zz1 << std::endl;
  oops::Log::info() << "<dx,KTdy>-<Kdx,dy>/<Kdx,dy>="
                    << (zz1-zz2)/zz2 << std::endl;
  const double tol = params.tolerance;
  EXPECT(oops::is_close(zz1, zz2, tol));
}

// -----------------------------------------------------------------------------

class Recipe : public oops::Test {
 public:
  Recipe() {}
  virtual ~Recipe() {}
 private:
  std::string testid() const override {return "vader::test::Recipe";}

  void register_tests() const override {
    std::vector<eckit::testing::Test>& ts = eckit::testing::specification();

    ts.emplace_back(CASE("vader/Recipe/testRecipeAdjoint")
      { testRecipeAdjoint(); });
  }

  void clear() const override {}
};

// -----------------------------------------------------------------------------

}  // namespace test
}  // namespace vader
