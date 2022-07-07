/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include <Eigen/Core>
#include <string>
#include <vector>

#include "atlas/array.h"
#include "atlas/field.h"

#include "mo/utils.h"
#include "oops/base/Variables.h"
#include "oops/util/Logger.h"

//--

namespace mo {


// ++ Atlas Field, FieldSet ++

void checkFieldSetContent(const atlas::FieldSet & fields,
                          const std::vector<std::string> expected_fields) {
  for (auto& ef : expected_fields) {
    if (!fields.has(ef)) {
      oops::Log::error() << "ERROR - data validation failed "
                            "(an expected field is missing)" << std::endl;
      throw std::runtime_error("data validation failed");
    }
  }
}

// ++ I/O processing ++

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

void getMIOFields(const atlas::FieldSet & stateFields,
                  atlas::FieldSet & ceffFields) {
  const std::string mioFileName = "Data/parameters/MIO_coefficients.nc";
  const std::vector<std::string> fnames {"rht",
                                         "liquid_cloud_volume_fraction_in_atmosphere_layer",
                                         "ice_cloud_volume_fraction_in_atmosphere_layer"};
  checkFieldSetContent(stateFields, fnames);

  auto rhtView = atlas::array::make_view<double, 2>(stateFields["rht"]);
  auto clView = atlas::array::make_view<double, 2>
                (stateFields["liquid_cloud_volume_fraction_in_atmosphere_layer"]);
  auto cfView = atlas::array::make_view<double, 2>
                (stateFields["ice_cloud_volume_fraction_in_atmosphere_layer"]);

  const std::vector<std::string> fnames_out {"cleff", "cfeff"};
  checkFieldSetContent(ceffFields, fnames_out);
  auto cleffView = atlas::array::make_view<double, 2>(ceffFields["cleff"]);
  auto cfeffView = atlas::array::make_view<double, 2>(ceffFields["cfeff"]);

  Eigen::MatrixXd mioCoeffCl = createMIOCoeff(mioFileName, "qcl_coef");
  Eigen::MatrixXd mioCoeffCf = createMIOCoeff(mioFileName, "qcf_coef");

  for  (atlas::idx_t jn = 0; jn < stateFields["rht"].shape(0); ++jn) {
    for (int jl = 0; jl < stateFields["rht"].levels(); ++jl) {
      if (jl < Constants::mioLevs) {
        std::size_t ibin = (rhtView(jn, jl) > 100.0) ? Constants::mioBins - 1 :
                           static_cast<std::size_t>(floor(rhtView(jn, jl) / Constants::rHTBin));

        std::double_t ceffdenom = (1.0 -  clView(jn, jl) * cfView(jn, jl) );
        if (ceffdenom > Constants::tol) {
          std::double_t clcf = clView(jn, jl) * cfView(jn, jl);
          cleffView(jn, jl) = mioCoeffCl(jl, ibin) * (clView(jn, jl) - clcf) / ceffdenom;
          cfeffView(jn, jl) = mioCoeffCf(jl, ibin) * (cfView(jn, jl) - clcf) / ceffdenom;
        } else {
          cleffView(jn, jl) = 0.5;
          cfeffView(jn, jl) = 0.5;
        }
      } else {
        cleffView(jn, jl) = 0.0;
        cfeffView(jn, jl) = 0.0;
      }
    }
  }
}

Eigen::MatrixXd createMIOCoeff(const std::string mioFileName,
                               const std::string s)
{
    Eigen::MatrixXd mioCoeff(static_cast<std::size_t>(Constants::mioLevs),
                             static_cast<std::size_t>(Constants::mioBins));
    double valuesvec[Constants::mioLookUpLength];

    umGetLookUp2D_f90(static_cast<int>(mioFileName.size()),
                      mioFileName.c_str(),
                      static_cast<int>(s.size()),
                      s.c_str(),
                      static_cast<int>(Constants::mioBins),
                      static_cast<int>(Constants::mioLevs),
                      valuesvec[0]);

    for (int j = 0; j < Constants::mioLevs; ++j) {
        for (int i = 0; i < Constants::mioBins; ++i) {
            // Fortran returns column major order, but C++ needs row major
            mioCoeff(j, i) = valuesvec[i*Constants::mioLevs+j];
        }
    }

    return mioCoeff;
}

}  // namespace mo
