/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <boost/noncopyable.hpp>

#include "atlas/field/FieldSet.h"
#include "atlas/functionspace/FunctionSpace.h"
#include "oops/util/Printable.h"
#include "vader/RecipeBase.h"

namespace oops {
  class Variable;
  class Variables;
}

namespace vader {

// ------------------------------------------------------------------------------------------------
/// Recipe class

class Recipe  : public util::Printable,
                private boost::noncopyable {
 public:
    typedef RecipeParametersWrapper Parameters_;

    Recipe(const std::string, const Parameters_ &, const VaderConfigVars &);

    std::string name() const;
    const oops::Variable product() const;
    const oops::Variables ingredients() const;
    size_t productLevels(const atlas::FieldSet &) const;
    atlas::FunctionSpace productFunctionSpace(const atlas::FieldSet &) const;
    bool hasTLAD() const;
    void executeNL(atlas::FieldSet &);
    void executeTL(atlas::FieldSet &, const atlas::FieldSet &);
    void executeAD(atlas::FieldSet &, const atlas::FieldSet &);

 private:
    void print(std::ostream &) const;
    std::unique_ptr<RecipeBase> recipe_;
};

}  // namespace vader
