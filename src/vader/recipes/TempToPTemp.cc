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
const std::vector<std::string> TempToPTemp::Ingredients = {VV_TS, VV_PS};
const double default_kappa = 0.2857;
const double p0_not_in_config = -1.0;
const double default_Pa_p0 = 100000.0;
const double default_hPa_p0 = 1000.0;

// Register the maker
static RecipeMaker<TempToPTemp> makerTempToPTemp_(TempToPTemp::Name);

TempToPTemp::TempToPTemp(const eckit::Configuration & config) :
   p0_{config.getDouble(TempToPTemp::Name + ".p0", p0_not_in_config)},
   kappa_{config.getDouble(TempToPTemp::Name + ".kappa", default_kappa)} {

   oops::Log::trace() << "TempToPTemp::TempToPTemp" << std::endl;
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

   atlas::Field temperature = afieldset->field(VV_TS);
   atlas::Field pressure = afieldset->field(VV_PS);
   atlas::Field potential_temperature = afieldset->field(VV_PT);
   std::string t_units, ps_units;

   temperature.metadata().get("units", t_units);
   pressure.metadata().get("units", ps_units);
   if (p0_ == p0_not_in_config) {
      // If p0 not specified in config, determine it from pressure units
      if (ps_units == "Pa") {
         p0_ = default_Pa_p0;
      }
      else if (ps_units == "hPa") {
         p0_ = default_hPa_p0;
      }
      else {
         oops::Log::error() << "TempToPTemp::execute failed because p0 could not be determined." << std::endl;
         return false;
      }
   }

   oops::Log::debug() << "TempToPTemp::execute: p0 value: " << p0_ << std::endl;
   oops::Log::debug() << "TempToPTemp::execute: kappa value: " << kappa_ << std::endl;

   auto temperature_view = atlas::array::make_view <double , 2>( temperature );
   auto pressure_view = atlas::array::make_view <double , 2>( pressure );
   auto potential_temperature_view = atlas::array::make_view <double , 2>( potential_temperature );

   int nlevels = temperature.levels();
   for (int level = 0; level < nlevels; ++level) {
      potential_temperature_view(level, 0) = temperature_view(level, 0) * pow(p0_ / pressure_view(1, 0), kappa_);
   }
   potential_temperature_filled = true;

   oops::Log::trace() << "leaving TempToPTemp::execute function" << std::endl;

   return potential_temperature_filled;
}

}