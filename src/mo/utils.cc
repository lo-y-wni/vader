/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */


#include <string>
#include <vector>

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

}  // namespace mo
