/*
 * (C) Copyright 2021  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>
#include <vector>

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "oops/util/Logger.h"
#include "recipe.h"

namespace vader {

// -----------------------------------------------------------------------------
Recipe::Recipe() {}
// -----------------------------------------------------------------------------
Recipe::Recipe(std::vector<std::string> ingredients, std::function<int(atlas::Field *, atlas::FieldSet *)> execute)
          : ingredients{ingredients}, execute(execute) { }
// -----------------------------------------------------------------------------
Recipe::~Recipe() {}
// -----------------------------------------------------------------------------

// Implementation of vader "execute" functions for the instantiated recipes (Is this the right place for these?)

int ps_to_delp(atlas::Field *delp, atlas::FieldSet *ingredients) {
    // TODO: Implement conversion logic
    return 0;
}

int t_to_pt(atlas::Field *pt, atlas::FieldSet *ingredients) {
    // TODO: Implement conversion logic
    return 0;
}

}  // namespace vader

