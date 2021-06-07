/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef COOKBOOK_H_
#define COOKBOOK_H_

#include <vector>
#include <unordered_map>

#include "oops/util/Logger.h"
#include "vader.h"
#include "vadervariables.h"

// Recipe headers
#include "recipes/TempToPTemp.h"
#include "recipes/PressureToDelP.h"

namespace vader {

// -----------------------------------------------------------------------------
// This defines the recipes used (and their priority) by default in Vader
// (Recipes can be added, removed, or rearranged in cookbook by specifying in yaml.)
std::unordered_map<std::string, std::vector<std::string>> Vader::getDefaultCookbookDef() {
   return {
      // This defines the default Vader Cookbook
      // The Key is the name of the variable produced by all the recipes in the Value
      // The Value is a vector of recipe names that will be searched, in order, by Vader for viability
        {VV_PT, {TempToPTemp::Name}}
      , {VV_DELP, {PressureToDelP::Name}}
   };
}

} // namespace vader

#endif  // COOKBOOK_H_
