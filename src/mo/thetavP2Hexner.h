/*
 * (C) Crown Copyright 2022 Met Office
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef SRC_MO_THETAVP2HEXNER_H_
#define SRC_MO_THETAVP2HEXNER_H_

#include "atlas/field/Field.h"

namespace atlas {
  class Field;
}

namespace unifiedmodel {

/// \details This is a straight inverse of thetavP2Hexner
///          Note that we need to revisit this for the top level for thetav.
void hexner2PThetav(const atlas::Field & rp,
                   const atlas::Field & hexner,
                   atlas::Field & p,
                   atlas::Field & thetav);

/// \details Tangent linear approximation to the
///          transformation from virtual potential temperature (thetav) to
///          hydrostatically-balanced exner (hexner)
void thetavP2HexnerTL(const atlas::Field & rp,
                     const atlas::Field & thetav,
                     const atlas::Field & p,
                     const atlas::Field & hexner,
                     const atlas::Field & thetavInc,
                     const atlas::Field & pInc,
                     atlas::Field & hexnerInc);

/// \details Adjoint of the tangent linear approximation to the
///          transformation from virtual potential temperature (thetav) to
///          hydrostatically-balanced exner (hexner)
///          Note:: zeroing of hexnerHatView not done.
void thetavP2HexnerAD(const atlas::Field & rp,
                     const atlas::Field & thetav,
                     const atlas::Field & p,
                     const atlas::Field & hexner,
                     atlas::Field & thetavHat,
                     atlas::Field & pHat,
                     atlas::Field & hexnerHat);

/// \details Tangent linear approximation to the
///          transformation from hydrostatically-balanced exner (hexner)
///          to virtual potential temperature (thetav)
void hexner2ThetavTL(const atlas::Field & rp,
                     const atlas::Field & thetav,
                     const atlas::Field & hexnerInc,
                     atlas::Field & thetavInc);

/// \details Adjoint of the tangent linear approximation to the
///          transformation from virtual potential temperature (thetav) to
///          hydrostatically-balanced exner (hexner)
///          Note:: zeroing of hexnerHatView not done.
void hexner2ThetavAD(const atlas::Field & rp,
                     const atlas::Field & thetav,
                     atlas::Field & thetavHat,
                     atlas::Field & hexnerHat);

/// \details Tangent linear approximation to create the scaled dry
///          density using exner and the dry virtual temperature
///
///          rho'_d = rho ( exner'       theta_vd' )
///                       (-------   -   --------- )
///                       ( exner        theta_vd  )
///          theta_vd and theta_vd' needs to be vertically interpolated
///          onto the rho_grid.
void thetavExner2RhoTL(const atlas::Field & rp,
                     const atlas::Field & rtheta,
                     const atlas::Field & exner,
                     const atlas::Field & thetav,
                     const atlas::Field & rhoRR,
                     const atlas::Field & exnerInc,
                     const atlas::Field & thetavInc,
                     atlas::Field & rhoRRInc);


/// \details Adjoint of Tangent linear approximation to create the scaled dry
///          density using exner and the dry virtual temperature
///
///          rho'_d = rho ( exner'       theta_vd' )
///                       (-------   -   --------- )
///                       ( exner        theta_vd  )
///          theta_vd and theta_vd' needs to be vertically interpolated
///          onto the rho_grid.
void thetavExner2RhoAD(const atlas::Field & rp,
                       const atlas::Field & rtheta,
                       const atlas::Field & exner,
                       const atlas::Field & thetav,
                       const atlas::Field & rhoRR,
                       atlas::Field & exnerHat,
                       atlas::Field & thetavHat,
                       atlas::Field & rhoRRHat);

/// \details Converting hExner to Exner
///          hExner is on rho levels
///          Exner is on p levels
///          Topmost level of exner is a copy of the level
///          below as the upper boundary condtion is
///          d(exner)/dz = 0.
///          Note: exnerHat not zeroed here!
void hExner2exnerTL(const atlas::Field & hexnerInc,
                    atlas::Field & exnerInc);


/// \details  Adjoint of hExner2exnerTL
///          hExner is on rho levels
///          Exner is on p levels
///          Note: exnerHat not zeroed here!
void hExner2exnerAD(atlas::Field & exnerHat,
                    atlas::Field & hexnerHat);


}  // namespace unifiedmodel

#endif  // SRC_MO_THETAVP2HEXNER_H_
