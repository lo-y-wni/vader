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
#include "vader/vader/vadervariables.h"
#include "TempToPTemp.h"

namespace vader {
// -----------------------------------------------------------------------------

// Static attribute initialization
const std::string TempToPTemp::Name = "TempToPTemp";
const std::vector<std::string> TempToPTemp::Ingredients = {VV_T, VV_PS};
const double default_kappa = 0.2857;
const double default_Pa_p0 = 100000.0;
const double default_hPa_p0 = 1000.0;

// Register the maker
static RecipeMaker<TempToPTemp> makerTempToPTemp_(TempToPTemp::Name);

TempToPTemp::TempToPTemp(const eckit::Configuration & config) :
   RecipeBase{config}, config_{config.getSubConfiguration(TempToPTemp::Name)} {

   oops::Log::trace() << "TempToPTemp created" << std::endl;
   oops::Log::debug() << "TempToPTemp.config_: " << config_ << std::endl;
}

std::string TempToPTemp::name() const {
   return TempToPTemp::Name;
}

std::vector<std::string> TempToPTemp::ingredients() const {
   return TempToPTemp::Ingredients;
}

bool TempToPTemp::execute(atlas::FieldSet *afieldset) {
   bool potential_temperature_filled = false;

   oops::Log::trace() << "entering TempToPTemp::execute function" << std::endl;

   atlas::Field temperature = afieldset->field(VV_T);
   atlas::Field pressure = afieldset->field(VV_PS);
   atlas::Field potential_temperature = afieldset->field(VV_PT);
   std::string t_units, ps_units;
   double p0, kappa;

   temperature.metadata().get("units", t_units);
   pressure.metadata().get("units", ps_units);
   if (config_.has("p0")) {
      p0 = config_.getDouble("p0");
   }
   else if (ps_units == "Pa") {
      p0 = default_Pa_p0;
   }
   else if (ps_units == "hPa") {
      p0 = default_hPa_p0;
   }
   else {
      oops::Log::error() << "TempToPTemp::execute failed because p0 could not be determined." << std::endl;
      return false;
   }
   kappa = config_.getDouble("kappa", default_kappa);

   oops::Log::debug() << "TempToPTemp::execute: p0 value: " << p0 << std::endl;
   oops::Log::debug() << "TempToPTemp::execute: kappa value: " << kappa << std::endl;

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
   oops::Log::debug() << "Pot. Temperature shape: " << potential_temperature.shape()[1] << "," << potential_temperature.shape()[2] << std::endl;

   oops::Log::debug() << "Pressure Size: " << pressure.size() << std::endl;
   oops::Log::debug() << "Pressure Rank: " << pressure.rank() << std::endl;
   oops::Log::debug() << "Pressure Levels: " << pressure.levels() << std::endl;
   oops::Log::debug() << "Pressure Units: " << ps_units << std::endl;
   oops::Log::debug() << "Pressure shape: " << pressure.shape()[1] << "," << pressure.shape()[2] << std::endl;

   int nlevels = temperature.levels();
   for (int level = 0; level < nlevels; ++level) {
      potential_temperature_view(level, 0) = temperature_view(level, 0) * pow(p0 / pressure_view(1, 0), kappa);
   }
   potential_temperature_filled = true;
   oops::Log::debug() << "Pot. Temperature 1st element: " << potential_temperature_view(1,0) << std::endl;
   oops::Log::debug() << "Temperature 1st element: " << temperature_view(1,0) << std::endl;
   oops::Log::debug() << "Pressure 1st element: " << pressure_view(1,0) << std::endl;

   oops::Log::trace() << "leaving TempToPTemp execute function" << std::endl;

   return potential_temperature_filled;
}

}