/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef SRC_VADER_RECIPE_H_
#define SRC_VADER_RECIPE_H_

#include <memory>
#include <vector>
#include <string>

#include <boost/noncopyable.hpp>

#include "oops/util/Printable.h"
#include "atlas/field/FieldSet.h"
#include "vader/RecipeBase.h"

namespace vader {

// -----------------------------------------------------------------------------
/// Recipe class

class Recipe  : public util::Printable,
                private boost::noncopyable {
 public:
    typedef RecipeParametersWrapper Parameters_;

    Recipe(const std::string, const Parameters_ &);

    std::string name() const;
    std::vector<std::string> ingredients() const;
    bool requiresSetup() const;
    bool setup(atlas::FieldSet *);
    bool execute(atlas::FieldSet *);

 private:
    void print(std::ostream &) const;
    std::unique_ptr<RecipeBase> recipe_;
};

}  // namespace vader

#endif  // SRC_VADER_RECIPE_H_
