/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

#include "atlas/field/Field.h"
#include "atlas/field/FieldSet.h"

namespace mo {

/// \details Tangent linear approximation to the
///          transformation from virtual potential temperature (thetav) to
///          hydrostatically-balanced exner (hexner)
void thetavP2HexnerTL(atlas::FieldSet & fields);

/// \details Adjoint of the tangent linear approximation to the
///          transformation from virtual potential temperature (thetav) to
///          hydrostatically-balanced exner (hexner)
///          Note:: zeroing of hexnerHatView not done.
void thetavP2HexnerAD(atlas::FieldSet & fields);

/// \details Tangent linear approximation to the
///          transformation from hydrostatically-balanced exner (hexner)
///          to virtual potential temperature (thetav)
void hexner2ThetavTL(atlas::FieldSet & fields);

/// \details Adjoint of the tangent linear approximation to the
///          transformation from virtual potential temperature (thetav) to
///          hydrostatically-balanced exner (hexner)
///          Note:: zeroing of hexnerHatView not done.
void hexner2ThetavAD(atlas::FieldSet & fields);

/// \details Tangent linear approximation to create the scaled dry
///          density using exner and the dry virtual temperature
///
///          rho'_d = rho ( exner'       theta_vd' )
///                       (-------   -   --------- )
///                       ( exner        theta_vd  )
///          theta_vd and theta_vd' needs to be vertically interpolated
///          onto the rho_grid.
void thetavExner2RhoTL(atlas::FieldSet & fields);

/// \details Adjoint of Tangent linear approximation to create the scaled dry
///          density using exner and the dry virtual temperature
///
///          rho'_d = rho ( exner'       theta_vd' )
///                       (-------   -   --------- )
///                       ( exner        theta_vd  )
///          theta_vd and theta_vd' needs to be vertically interpolated
///          onto the rho_grid.
void thetavExner2RhoAD(atlas::FieldSet & fields);

/// \details Calculate qT increment from the sum of q, qcl and qcf increments
void qqclqcf2qtTL(atlas::FieldSet & incFields);

/// \details This is the adjoint of qqclqcf2qtTL
void qqclqcf2qtAD(atlas::FieldSet & hatFields);

/// \details This is the moisture incrementing operator (MIO) that determines the cloud condensate
///          increments from increments in total water and temperature
void qtTemperature2qqclqcfTL(const atlas::FieldSet & stateFields,
                        const atlas::FieldSet & ceffFieldSet,
                        atlas::FieldSet & incFields);

/// \details This is the adjoint of the MIO
void qtTemperature2qqclqcfAD(const atlas::FieldSet & stateFields,
                       const atlas::FieldSet & ceffFieldSet,
                       atlas::FieldSet & hatFields);
}  // namespace mo
