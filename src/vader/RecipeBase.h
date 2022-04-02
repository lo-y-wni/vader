/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef SRC_VADER_RECIPEBASE_H_
#define SRC_VADER_RECIPEBASE_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <boost/noncopyable.hpp>

#include "eckit/config/Configuration.h"
#include "atlas/field/FieldSet.h"
#include "oops/base/Variables.h"
#include "oops/util/AssociativeContainers.h"
#include "oops/util/parameters/HasParameters_.h"
#include "oops/util/parameters/OptionalPolymorphicParameter.h"
#include "oops/util/parameters/Parameters.h"
#include "oops/util/parameters/RequiredParameter.h"
#include "oops/util/Printable.h"

namespace vader {

// static const char * recipeNameString = "recipe name";

// -----------------------------------------------------------------------------
class RecipeParametersBase : public oops::Parameters {
  OOPS_ABSTRACT_PARAMETERS(RecipeParametersBase, oops::Parameters)

 public:
  oops::RequiredParameter<std::string> name{
     "recipe name",
     this};
};

class GenericRecipeParameters : public RecipeParametersBase {
  OOPS_CONCRETE_PARAMETERS(GenericRecipeParameters, RecipeParametersBase)
 public:
};

// -----------------------------------------------------------------------------
/*! \brief RecipeBase class defines interface for individual variable
           transformations.
 *
 *  \details This pure virtual class provides the template for all concrete
 *           classes that calculate a variable using other variables
 *           (ingredients) as input.
 */

class RecipeBase : public util::Printable,
                   private boost::noncopyable {
 public:
  RecipeBase() {}
  virtual ~RecipeBase() {}

/// Name of the recipe
  virtual std::string name() const = 0;

/// Ingredients (list of variables required to setup and execute recipe)
  virtual std::vector<std::string> ingredients() const = 0;

/// Flag indicating whether the recipe requires setup.
  virtual bool requiresSetup() { return false; }
/// setup must return true on success, false on failure
  virtual bool setup(atlas::FieldSet *) { return true; }

/// Execute method performs the variable change
/// execute must return true on success, false on failure
  virtual bool execute(atlas::FieldSet *) = 0;

 private:
  virtual void print(std::ostream &) const;
};

// -----------------------------------------------------------------------------

/// Recipe Factory
class RecipeFactory {
 public:
  static RecipeBase * create(const std::string name,
                             const RecipeParametersBase &);
  static RecipeBase * create(const std::string name);
  static std::unique_ptr<RecipeParametersBase> createParameters(const std::string &);

  static std::vector<std::string> getMakerNames() {
    return oops::keys(getMakers());
  }

  virtual ~RecipeFactory() = default;
 protected:
  explicit RecipeFactory(const std::string &);
 private:
  virtual RecipeBase* make(const RecipeParametersBase &) = 0;
  virtual RecipeBase* make() = 0;

  virtual std::unique_ptr<RecipeParametersBase> makeParameters() const = 0;

  static std::map < std::string, RecipeFactory * > & getMakers() {
    static std::map < std::string, RecipeFactory * > makers_;
    return makers_;
  }
};

// -----------------------------------------------------------------------------

template<class T>
class RecipeMaker : public RecipeFactory {
  typedef oops::TParameters_IfAvailableElseFallbackType_t<T, GenericRecipeParameters>
    Parameters_;
  virtual RecipeBase * make(const RecipeParametersBase & params) override
    { return new T(params); }
  virtual RecipeBase * make() override { return new T(); }
 
   std::unique_ptr<RecipeParametersBase> makeParameters() const override {
    return std::make_unique<Parameters_>();
  }

 public:
  explicit RecipeMaker(const std::string & name) : RecipeFactory(name) {}
};

// -----------------------------------------------------------------------------
class RecipeParametersWrapper : public oops::Parameters {
  OOPS_CONCRETE_PARAMETERS(RecipeParametersWrapper, Parameters)

 public:
  oops::OptionalPolymorphicParameter<RecipeParametersBase, RecipeFactory> recipeParams{
     "recipe name",
     this};

};
// -----------------------------------------------------------------------------

}  // namespace vader

#endif  // SRC_VADER_RECIPEBASE_H_
