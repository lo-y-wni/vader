
/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <functional>
#include <memory>
#include <ostream>
#include <vector>

#include "atlas/array/MakeView.h"
#include "atlas/field/Field.h"
#include "atlas/field/FieldSet.h"
#include "atlas/functionspace.h"

#include "eckit/config/Configuration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/mpi/Comm.h"

#include "mo/common_varchange.h"
#include "mo/utils.h"

#include "oops/base/Variables.h"
#include "oops/util/Logger.h"

#include "vader/vadervariables.h"

// -----------------------------------------------------------------------------
namespace mo {
// -----------------------------------------------------------------------------

bool evalSatVaporPressure(atlas::FieldSet & fields)
{
  oops::Log::trace() << "[svp()] starting ..." << std::endl;

  // normalised T lambda function
  // which enforces upper and lower bounds on T
  auto normalisedT = [](const double tVal) {
    double t = (tVal - Constants::TLoBound)/Constants::Tinc;
    double t1 = (t < 0.0 ? 0.0 : t);
    double t2 = (t1 >= static_cast<double>(Constants::svpLookUpLength - 1)) ?
                       static_cast<double>(Constants::svpLookUpLength - 1) : t1;
    return t2;
  };

  // lambda function returning the index of a given temperature value;
  // avoids returning last index in table
  auto index = [](const double normalisedTVal) {
    std::size_t i = static_cast<std::size_t>(normalisedTVal);
    return (i == Constants::svpLookUpLength - 1 ? Constants::svpLookUpLength - 2 : i);
  };

  // weight lamba function
  auto weight = [index](const double normalisedTVal) {
    std::size_t i = index(normalisedTVal);
    return ( normalisedTVal -  static_cast<double>(i) );
  };

  // lambda function to interpolate values from table to given temperature
  auto interp = [](const double weight, const double v1, const double v2) {
     return (weight * v2 + (1 - weight) * v1);
  };

  // lambda function to determine the gradient of variable in table
  // with respect to temperature
  auto normalisedDiff = [](const double v1, const double v2) {
     return (v2 - v1)/ Constants::Tinc;
  };

  double w; // weight
  std::size_t indx; //index

  oops::Variables lookUpVars(std::vector<std::string>{"svp", "dlsvp", "svpW", "dlsvpW"});
  auto lookUpData = getLookUps(constantsFilePath, lookUpVars, Constants::svpLookUpLength);
  const std::vector<double> Lookup = lookUpData[0];  // supposed to be svp, but need checks

  // The check for the presence of required input fields will be performed by the Vader
  // algorithm when this code is in a Vader Recipe. At that time this check can be removed.
  if ( !fields.has_field(vader::VV_TS) ||
       !fields.has_field(vader::VV_SVP)) {
    return false;
  }

  if (fields[vader::VV_SVP].shape(0) != fields[vader::VV_TS].shape(0)
      || fields[vader::VV_SVP].levels() != fields[vader::VV_TS].levels()) {
    // svp field not compatible with air temperature field, cannot continue.
    return false;
  }

  auto tView  = atlas::array::make_view<double, 2>(fields[vader::VV_TS]);
  auto svpView = atlas::array::make_view<double, 2>(fields[vader::VV_SVP]);

  auto conf = atlas::util::Config("levels", fields[vader::VV_SVP].levels()) |
              atlas::util::Config("include_halo", true);

  // check this recipe to calculate svp is correct
  auto evaluateSVP = [&] (atlas::idx_t i, atlas::idx_t j) {
  indx = index(normalisedT(tView(i, j)));
  w = weight(normalisedT(tView(i, j)));
  svpView(i, j) = interp(w, Lookup.at(indx), Lookup.at(indx+1)); };

  auto fspace = fields[vader::VV_SVP].functionspace();

  parallelFor(fspace, evaluateSVP, conf);

  oops::Log::trace() << "[svp()] ... exit" << std::endl;

  return true;
}

bool evalSatSpecificHumidity(const atlas::Field & t, const atlas::Field & pbar,
                             const atlas::Field & svp, atlas::Field & qsat)
{
  oops::Log::trace() << "[getQsat()] starting ..." << std::endl;

  auto tView = atlas::array::make_view<double, 2>(t);
  auto pbarView = atlas::array::make_view<double, 2>(pbar);
  auto qsatView = atlas::array::make_view<double, 2>(qsat);
  auto svpView = atlas::array::make_view<double, 2>(svp);

  auto conf = atlas::util::Config("levels", qsat.levels()) |
              atlas::util::Config("include_halo", true);

  auto evaluateQsat = [&] (atlas::idx_t i, atlas::idx_t j) {
    // Converts from sat vapour pressure in pure water to pressure in air
    double fsubw = 1.0 + 1.0E-8*pbarView(i, j)*( 4.5 +
                      6.0e-4*( tView(i, j) - Constants::zerodegc )
                      *( tView(i, j) - Constants::zerodegc ) );
    qsatView(i, j) =  Constants::rd_over_rv * svpView (i, j) /
          (std::max (pbarView(i, j), svpView (i, j)) -
          (1.0 - Constants::rd_over_rv) * svpView (i, j)); };

  auto fspace = qsat.functionspace();

  parallelFor(fspace, evaluateQsat, conf);

  oops::Log::trace() << "[getQsat()] ... exit" << std::endl;

  return true;
}

}
