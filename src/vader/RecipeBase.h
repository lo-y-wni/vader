/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef SRC_VADER_RECIPEBASE_H_
#define SRC_VADER_RECIPEBASE_H_

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

#include <boost/noncopyable.hpp>

#include "eckit/config/Configuration.h"
#include "atlas/field/FieldSet.h"
#include "oops/base/Variables.h"
#include "oops/util/Printable.h"

namespace vader {

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
                             const eckit::Configuration &);
  virtual ~RecipeFactory() = default;
 protected:
  explicit RecipeFactory(const std::string &);
 private:
  virtual RecipeBase* make(const std::string, const eckit::Configuration &) = 0;
  static std::unordered_map < std::string, RecipeFactory * > & getMakers() {
    static std::unordered_map < std::string, RecipeFactory * > makers_;
    return makers_;
  }
};

// -----------------------------------------------------------------------------

template<class T>
class RecipeMaker : public RecipeFactory {
  virtual RecipeBase * make(const std::string name,
                            const eckit::Configuration & conf)
    { return new T(conf); }
 public:
  explicit RecipeMaker(const std::string & name) : RecipeFactory(name) {}
};

// -----------------------------------------------------------------------------

}  // namespace vader

#endif  // SRC_VADER_RECIPEBASE_H_
