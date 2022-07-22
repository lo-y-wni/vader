/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef SRC_VADER_COOKBOOK_H_
#define SRC_VADER_COOKBOOK_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "oops/util/Logger.h"
#include "vader/vader.h"
#include "vader/vadervariables.h"

// Recipe headers
#include "recipes/PressureToDelP.h"
#include "recipes/TempToPTemp.h"

namespace vader
{

// ------------------------------------------------------------------------------------------------
// This defines the recipes used (and their priority) by default in Vader
// (Recipes can be added, removed, or rearranged in cookbook by specifying in
//  yaml.)
std::unordered_map<std::string, std::vector<std::string>> Vader::getDefaultCookbookDef() {
    return {
        // This defines the default Vader Cookbook
        // Key: name of the variable produced by all the recipes in the Value
        // Value: a vector of recipe names that will be searched, in order,
        //        by Vader for viability
        {VV_PT, {TempToPTemp::Name}},
        // TODO(vahl) get PressureToDelP recipe working
        /*{VV_DELP, {PressureToDelP::Name}}*/};
}

}  // namespace vader

#endif  // SRC_VADER_COOKBOOK_H_
