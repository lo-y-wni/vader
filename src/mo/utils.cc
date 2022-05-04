/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */


#include "oops/util/Logger.h"

#include "lfrictransforms/utils.h"



//--
// ++ Atlas Field, FieldSet ++


void checkFieldSetContent(const atlas::FieldSet & fields,
                          const std::vector<std::string> expected_fields) {
  for (auto ef : expected_fields) {
    if (!fields.has_field(ef)) {
      oops::Log::error() << "ERROR - data validation failed "
                            "(an expected field is missing)" << std::endl;
      throw std::runtime_error("data validation failed");
    }
  }
}
