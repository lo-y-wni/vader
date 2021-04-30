/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef RECIPE_H_
#define RECIPE_H_

#include <vector>
#include <functional>

#include "atlas/field/FieldSet.h"

namespace vader {

// -----------------------------------------------------------------------------
/// Recipe class

class Recipe {
 public:
    explicit Recipe();
    Recipe(std::vector<std::string> ingredients, std::function<int(atlas::Field *, atlas::FieldSet *)> execute);
    ~Recipe();

    const std::vector<std::string> ingredients;
    const std::function<int(atlas::Field *, atlas::FieldSet *)> execute;

 private:
};

} // namespace vader

#endif  // RECIPE_H_
