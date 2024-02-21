/*
 * (C) Crown Copyright 2022-2024 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <Eigen/Core>
#include <string>
#include <vector>

#include "atlas/array.h"
#include "atlas/field.h"

#include "mo/constants.h"
#include "mo/functions.h"

#include "oops/base/Variables.h"
#include "oops/util/Logger.h"

using atlas::array::make_view;

namespace mo {
namespace functions {

std::vector<double> getLookUp(const std::string & sVPFilePath,
                              const std::string & shortName,
                              const std::size_t lookupSize) {
  std::vector<double> values(lookupSize, 0);

  umGetLookUp_f90(static_cast<int>(sVPFilePath.size()),
                  sVPFilePath.c_str(),
                  static_cast<int>(shortName.size()),
                  shortName.c_str(),
                  static_cast<int>(lookupSize),
                  values[0]);

  return std::vector<double>(values.begin(), values.end());
}


std::vector<std::vector<double>> getLookUps(const std::string & sVPFilePath,
                                            const oops::Variables & vars,
                                            const std::size_t lookupSize) {
  std::vector<std::vector<double>> values(vars.size(), std::vector<double>(lookupSize));

  for (std::size_t i = 0; i < values.size(); ++i) {
    values[i] = getLookUp(sVPFilePath, vars[i], lookupSize);
  }

  return values;
}


Eigen::MatrixXd createMIOCoeff(const std::string mioFileName,
                               const std::string s) {
  oops::Log::trace() << "[createMIOCoeff] starting ..." << std::endl;
  Eigen::MatrixXd mioCoeff(static_cast<std::size_t>(constants::mioLevs),
                           static_cast<std::size_t>(constants::mioBins));

  std::vector<double> valuesvec(constants::mioLookUpLength, 0);

  umGetLookUp2D_f90(static_cast<int>(mioFileName.size()),
                    mioFileName.c_str(),
                    static_cast<int>(s.size()),
                    s.c_str(),
                    static_cast<int>(constants::mioBins),
                    static_cast<int>(constants::mioLevs),
                    valuesvec[0]);

  for (int j = 0; j < static_cast<int>(constants::mioLevs); ++j) {
    for (int i = 0; i < static_cast<int>(constants::mioBins); ++i) {
      // Fortran returns column major order, but C++ needs row major
      mioCoeff(j, i) = valuesvec[i * constants::mioLevs+j];
    }
  }
  oops::Log::trace() << "[createMIOCoeff] ... exit" << std::endl;
  return mioCoeff;
}

}  // namespace functions
}  // namespace mo
