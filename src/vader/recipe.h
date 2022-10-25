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
#include "oops/util/Printable.h"
#include "vader/RecipeBase.h"

namespace vader {

// ------------------------------------------------------------------------------------------------
/// Recipe class

class Recipe  : public util::Printable,
                private boost::noncopyable {
 public:
    typedef RecipeParametersWrapper Parameters_;

    Recipe(const std::string, const Parameters_ &);

    std::string name() const;
    std::string product() const;
    std::vector<std::string> ingredients() const;
    bool requiresSetup() const;
    bool hasTLAD() const;
    bool setup(atlas::FieldSet &);
    bool executeNL(atlas::FieldSet &);
    bool executeTL(atlas::FieldSet &, const atlas::FieldSet &);
    bool executeAD(atlas::FieldSet &, const atlas::FieldSet &);

 private:
    void print(std::ostream &) const;
    std::unique_ptr<RecipeBase> recipe_;
};

}  // namespace vader
