/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef RECIPE_H_
#define RECIPE_H_

#include <memory>
#include <vector>

#include <boost/noncopyable.hpp>

#include "oops/util/Printable.h"
#include "atlas/field/FieldSet.h"
#include "vader/vader/RecipeBase.h"

namespace vader {

// -----------------------------------------------------------------------------
/// Recipe base class

class Recipe  : public util::Printable,
                private boost::noncopyable {
   public:
      Recipe(const std::string, const eckit::Configuration &);

      std::string name() const;
      std::vector<std::string> ingredients() const;
      bool requiresSetup() const;
      bool setup(atlas::FieldSet *);
      bool execute(atlas::FieldSet *);

   private:
  void print(std::ostream &) const;
  std::unique_ptr<RecipeBase> recipe_;
};

} // namespace vader

#endif  // RECIPE_H_
