/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "src/vader/RecipeBase.h"

#include <vector>
#include <unordered_map>

#include "eckit/config/Configuration.h"
#include "oops/util/abor1_cpp.h"
#include "oops/util/Logger.h"

namespace vader {

// -----------------------------------------------------------------------------

RecipeFactory::RecipeFactory(const std::string & name) {
  if (getMakers().find(name) != getMakers().end()) {
    oops::Log::error() << name << " already registered in vader::RecipeFactory."
      << std::endl;
    ABORT("Element already registered in vader::RecipeFactory.");
  }
  getMakers()[name] = this;
}

// -----------------------------------------------------------------------------

RecipeBase * RecipeFactory::create(const std::string name,
                                   const eckit::Configuration & conf) {
  oops::Log::trace() << "RecipeFactory::create starting for name: " << name <<
    std::endl;
  typename std::unordered_map<std::string, RecipeFactory*>::iterator jloc =
    getMakers().find(name);
  if (jloc == getMakers().end()) {
    oops::Log::error() << name << " does not exist in vader::RecipeFactory." <<
      std::endl;
    ABORT("Element does not exist in vader::RecipeFactory.");
  }
  RecipeBase * ptr = jloc->second->make(name, conf);
  oops::Log::trace() << "RecipeFactory::create finished for name: " << name <<
    std::endl;
  return ptr;
}

void RecipeBase::print(std::ostream & os) const {
  os << name();
}

// -----------------------------------------------------------------------------

}  // namespace vader
