/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field.h"

#include "oops/util/Logger.h"


//--
// ++ Atlas Function Spaces ++

/// \brief procedure to call a functor for a given concrete implementation
///        of a function space type
template<typename Functor>
void executeFunc(const atlas::FunctionSpace & fspace, const Functor & functor) {
  if (atlas::functionspace::NodeColumns(fspace)) {
    functor(atlas::functionspace::CubedSphereNodeColumns(fspace));
  } else if (atlas::functionspace::CellColumns(fspace)) {
    functor(atlas::functionspace::CubedSphereCellColumns(fspace));
  } else {
    oops::Log::error() << "ERROR - a functor call failed "
                          "(function space type not allowed)" << std::endl;
    throw std::runtime_error("a functor call failed");
  }
}

/// \brief wrapper for 'parallel_for'
template<typename Functor>
void parallelFor(const atlas::FunctionSpace & fspace,
                 const Functor& functor,
                 const atlas::util::Config& conf = atlas::util::Config()) {
  executeFunc(fspace, [&](const auto& fspace){fspace.parallel_for(conf, functor);});
}
