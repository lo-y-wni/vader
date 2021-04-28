/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef VADER_H_
#define VADER_H_


#include "atlas/field/FieldSet.h"
#include "oops/base/Variables.h"

namespace vader {

// -----------------------------------------------------------------------------
/// Vader class

class Vader {
 public:
  explicit Vader();
  ~Vader();

  void changeVar(atlas::FieldSet * afieldset, const oops::Variables &) const;

 private:
};

} // namespace vader

#endif  // VADER_H_
