/*
 * (C) Copyright 2021-2022  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <math.h>
#include <iostream>
#include <vector>

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "atlas/util/Metadata.h"
#include "oops/util/Logger.h"
#include "vader/recipes/AirPressureAtInterface.h"

namespace vader {

// -------------------------------------------------------------------------------------------------

// Static attribute initialization
const char AirPressureAtInterface_A::Name[] = "AirPressureAtInterface_A";
const std::vector<std::string> AirPressureAtInterface_A::Ingredients = {"surface_pressure"};

// -------------------------------------------------------------------------------------------------

// Register the maker
static RecipeMaker<AirPressureAtInterface_A>
       makerAirPressureAtInterface_A_(AirPressureAtInterface_A::Name);

// -------------------------------------------------------------------------------------------------

AirPressureAtInterface_A::AirPressureAtInterface_A(
                                                 const AirPressureAtInterface_AParameters &params) {
    oops::Log::trace() << "AirPressureAtInterface_A::AirPressureAtInterface_A Starting"
                       << std::endl;
    oops::Log::trace() << "AirPressureAtInterface_A::AirPressureAtInterface_A Done"
                       << std::endl;
}

// -------------------------------------------------------------------------------------------------

std::string AirPressureAtInterface_A::name() const {
    return AirPressureAtInterface_A::Name;
}

// -------------------------------------------------------------------------------------------------

std::string AirPressureAtInterface_A::product() const {
    return "air_pressure_levels";
}

// -------------------------------------------------------------------------------------------------

std::vector<std::string> AirPressureAtInterface_A::ingredients() const {
    return AirPressureAtInterface_A::Ingredients;
}

// -------------------------------------------------------------------------------------------------

size_t AirPressureAtInterface_A::productLevels(const atlas::FieldSet & afieldset) const {
    atlas::Field ps = afieldset.field("surface_pressure");
    ASSERT_MSG(ps.metadata().has("nLevels"), "In Vader::AirPressureAtInterface_A::productLevels "
               "ps must contain nLevels in its metadata");
    int nLevels;
    ps.metadata().get("nLevels", nLevels);
    return nLevels + 1;
}

// -------------------------------------------------------------------------------------------------

atlas::FunctionSpace AirPressureAtInterface_A::productFunctionSpace(const atlas::FieldSet
& afieldset) const {
    return afieldset.field("surface_pressure").functionspace();
}

// -------------------------------------------------------------------------------------------------

bool AirPressureAtInterface_A::executeNL(atlas::FieldSet & afieldset) {
    //
    oops::Log::trace() << "AirPressureAtInterface_A::executeNL Starting" << std::endl;

    // Get the fields
    atlas::Field ps = afieldset.field("surface_pressure");
    atlas::Field prsi = afieldset.field("air_pressure_levels");

    // Get the units
    std::string prsi_units, ps_units;
    ps.metadata().get("units", ps_units);
    prsi.metadata().get("units", prsi_units);

    // Assert that the units match
    ASSERT_MSG(prsi_units == ps_units, "In Vader::AirPressureAtInterface_A::executeNL the units "
               "for pressure " + prsi_units + "do not match the surface pressure units" + ps_units);

    // Extract ak/bk from the metadata
    ASSERT_MSG(ps.metadata().has("ak"), "In Vader::AirPressureAtInterface_A::executeNL ps must "
               "contain ak in its metadata");
    ASSERT_MSG(ps.metadata().has("bk"), "In Vader::AirPressureAtInterface_A::executeNL ps must "
               "contain bk in its metadata");
    std::vector<double> ak, bk;
    ps.metadata().get("ak", ak);
    ps.metadata().get("bk", bk);

    // Get number of levels
    ASSERT_MSG(ps.metadata().has("nLevels"), "In Vader::AirPressureAtInterface_A::productLevels "
               "ps must contain nLevels in its metadata");
    int nLevels;
    ps.metadata().get("nLevels", nLevels);

    // Get the grid size
    const int gridSize = ps.shape(0);

    // Set the array views to manipulate the data
    auto prsi_view = atlas::array::make_view<double, 2>(prsi);
    auto ps_view = atlas::array::make_view<double, 2>(ps);

    // Compute pressure thickness from pressure at the levels
    for (int level = 0; level < nLevels + 1; ++level) {
        for ( size_t jNode = 0; jNode < gridSize ; ++jNode ) {
            prsi_view(jNode, level) = ak[level] + bk[level] * ps_view(jNode, 0);
        }
    }

    // Return
    oops::Log::trace() << "AirPressureAtInterface_A::executeNL Done" << std::endl;
    return true;
}

// -------------------------------------------------------------------------------------------------

}  // namespace vader

// -------------------------------------------------------------------------------------------------
