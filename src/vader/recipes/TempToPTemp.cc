/*
 * (C) Copyright 2021  UCAR.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <iostream>
#include <vector>
#include <math.h>

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "atlas/util/Metadata.h"
#include "oops/util/Logger.h"
#include "TempToPTemp.h"

namespace vader {

// Static attribute initialization
const std::string TempToPTempRecipe::Name = "t_to_pt";
const std::vector<std::string> TempToPTempRecipe::Ingredients = {"t", "ps"};
const double default_p0 = 1000.0;
const double default_kappa = 0.2857;

TempToPTempRecipe::TempToPTempRecipe(const eckit::Configuration & config) :
   p0_{config.getDouble("t_to_pt.p0", default_p0)},
   kappa_{config.getDouble("t_to_pt.kappa", default_kappa)} {

   oops::Log::trace() << 
      "TempToPTempRecipe::TempToPTempRecipe(config) constructor" << std::endl;

   oops::Log::debug() << "TempToPTempRecipe.p0_: " << p0_ << std::endl;
   oops::Log::debug() << "TempToPTempRecipe.kappa_: " << kappa_ << std::endl;
}

std::string TempToPTempRecipe::name() const {
   return TempToPTempRecipe::Name;
}

std::vector<std::string> TempToPTempRecipe::ingredients() const {
   return TempToPTempRecipe::Ingredients;
}

bool TempToPTempRecipe::execute(atlas::FieldSet *afieldset) {
   bool potential_temperature_filled = false;

   oops::Log::trace() << "entering TempToPTempRecipe::execute function" << std::endl;

   atlas::Field temperature = afieldset->field("t");
   atlas::Field pressure = afieldset->field("ps");
   atlas::Field potential_temperature = afieldset->field("pt");
   std::string t_units, ps_units;
   temperature.metadata().get("units", t_units);
   pressure.metadata().get("units", ps_units);
   auto temperature_view = atlas::array::make_view <double , 2>( temperature );
   auto pressure_view = atlas::array::make_view <double , 2>( pressure );
   auto potential_temperature_view = atlas::array::make_view <double , 2>( potential_temperature );

   oops::Log::debug() << "Temperature Size: " << temperature.size() << std::endl;
   oops::Log::debug() << "Temperature Rank: " << temperature.rank() << std::endl;
   oops::Log::debug() << "Temperature Levels: " << temperature.levels() << std::endl;
   oops::Log::debug() << "Temperature Units: " << t_units << std::endl;
   oops::Log::debug() << "Temperature shape: " << temperature.shape()[1] << "," << temperature.shape()[2] << std::endl;

   oops::Log::debug() << "Pot. Temperature Size: " << potential_temperature.size() << std::endl;
   oops::Log::debug() << "Pot. Temperature Rank: " << potential_temperature.rank() << std::endl;
   oops::Log::debug() << "Pot. Temperature Levels: " << potential_temperature.levels() << std::endl;
   oops::Log::debug() << "Pot. Temperature Units: " << ps_units << std::endl;
   oops::Log::debug() << "Pot. Temperature shape: " << potential_temperature.shape()[1] << "," << potential_temperature.shape()[2] << std::endl;

   oops::Log::debug() << "Pressure Size: " << pressure.size() << std::endl;
   oops::Log::debug() << "Pressure Rank: " << pressure.rank() << std::endl;
   oops::Log::debug() << "Pressure Levels: " << pressure.levels() << std::endl;
   oops::Log::debug() << "Pressure shape: " << pressure.shape()[1] << "," << pressure.shape()[2] << std::endl;

   int nlevels = temperature.levels();
   for (int level = 0; level < nlevels; ++level) {
      potential_temperature_view(level, 0) = temperature_view(level, 0) * pow(p0_ / pressure_view(1, 0), kappa_);
   }
   potential_temperature_filled = true;
   oops::Log::debug() << "Pot. Temperature 1st element: " << potential_temperature_view(1,0) << std::endl;
   oops::Log::debug() << "Temperature 1st element: " << temperature_view(1,0) << std::endl;
   oops::Log::debug() << "Pressure 1st element: " << pressure_view(1,0) << std::endl;

   oops::Log::trace() << "leaving t_to_pt execute function" << std::endl;

   return potential_temperature_filled;
}

}