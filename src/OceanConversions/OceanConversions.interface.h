/*
 * (C) Copyright 2023 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#pragma once

namespace vader {

extern "C" {
// call Fortran code
  float gsw_rho_t_exact_f90(const float & sal,
                            const float & temp,
                            const float & pressure);
  float gsw_p_from_z_f90(const float & depth,
                         const float & latitude);
  float gsw_pt_from_t_f90(const float & sal,
                          const float & temp,
                          const float & pressure);
  float gsw_ct_from_t_f90(const float & sal,
                          const float & temp,
                          const float & pressure);
  float gsw_ct_from_pt_f90(const float & sal,
                           const float & temp);
  float gsw_t_from_ct_f90(const float & sal,
                          const float & temp,
                          const float & pressure);
  float gsw_sa_from_sp_f90(const float & sal,
                           const float & pressure,
                           const float & longitude,
                           const float & latitude);
}  // extern C

}  // namespace vader
