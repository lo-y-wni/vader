/*
 * (C) Copyright 2021 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#ifndef SRC_VADER_VADERVARIABLES_H_
#define SRC_VADER_VADERVARIABLES_H_

#include <string>

namespace vader {

const char VV_SVP[]   = "svp"; // saturation vapor pressure;
const char VV_TV[]   = "virtual_temperature";
const char VV_TS[]   = "air_temperature";
const char VV_PT[]   = "potential_temperature";
const char VV_T[]    = "temperature";
const char VV_MIXR[] = "humidity_mixing_ratio";  // g/kg
const char VV_Q[]    = "specific_humidity";      // kg/kg
const char VV_U[]    = "eastward_wind";
const char VV_V[]    = "northward_wind";
const char VV_PRS[]  = "air_pressure";
const char VV_PRSI[] = "air_pressure_levels";
const char VV_DELP[] = "air_pressure_thickness";
const char VV_PS[]   = "surface_pressure";
const char VV_Z[]    = "geopotential_height";
const char VV_ZM[]   = "geometric_height";
const char VV_ZI[]   = "geopotential_height_levels";
const char VV_SFC_Z[]= "surface_geopotential_height";
const char VV_OZ[]   = "mole_fraction_of_ozone_in_air";
const char VV_CO2[]  = "mole_fraction_of_carbon_dioxide_in_air";
const char VV_CLW[]  = "mass_content_of_cloud_liquid_water_in_atmosphere_layer";
const char VV_CLI[]  = "mass_content_of_cloud_ice_in_atmosphere_layer";
const char VV_CLR[]  = "mass_content_of_rain_in_atmosphere_layer";
const char VV_CLS[]  = "mass_content_of_snow_in_atmosphere_layer";
const char VV_CLG[]  = "mass_content_of_graupel_in_atmosphere_layer";
const char VV_CLH[]  = "mass_content_of_hail_in_atmosphere_layer";
const char VV_CLWEFR[] = "effective_radius_of_cloud_liquid_water_particle";
const char VV_CLIEFR[] = "effective_radius_of_cloud_ice_particle";
const char VV_CLREFR[] = "effective_radius_of_rain_particle";
const char VV_CLSEFR[] = "effective_radius_of_snow_particle";
const char VV_CLGEFR[] = "effective_radius_of_graupel_particle";
const char VV_CLHEFR[] = "effective_radius_of_hail_particle";
const char VV_CLDFRAC[]= "cloud_area_fraction_in_atmosphere_layer";
const char VV_SFC_P2M[] = "air_pressure_at_two_meters_above_surface";    // (Pa)
const char VV_SFC_Q2M[] = "specific_humidity_at_two_meters_above_surface";
const char VV_SFC_T2M[] = "surface_temperature";  // (K)
const char VV_SFC_TSKIN[] = "skin_temperature";   // (K)
const char VV_SFC_WFRAC[] = "water_area_fraction";
const char VV_SFC_LFRAC[] = "land_area_fraction";
const char VV_SFC_IFRAC[] = "ice_area_fraction";
const char VV_SFC_SFRAC[] = "surface_snow_area_fraction";
const char VV_SFC_WTMP[]  = "surface_temperature_where_sea";
const char VV_SFC_LTMP[]  = "surface_temperature_where_land";
const char VV_SFC_ITMP[]  = "surface_temperature_where_ice";
const char VV_SFC_STMP[]  = "surface_temperature_where_snow";
const char VV_SFC_SDEPTH[]  = "surface_snow_thickness";
const char VV_SFC_VEGFRAC[] = "vegetation_area_fraction";
const char VV_SFC_WSPEED[]  = "surface_wind_speed";
const char VV_SFC_WDIR[]    = "surface_wind_from_direction";
const char VV_SFC_U10[]     = "uwind_at_10m";
const char VV_SFC_V10[]     = "vwind_at_10m";
const char VV_SFC_U[]       = "surface_eastward_wind";
const char VV_SFC_V[]       = "surface_northward_wind";
const char VV_SFC_LAI[]     = "leaf_area_index";
const char VV_SFC_SOILM[]   = "volume_fraction_of_condensed_water_in_soil";
const char VV_SFC_SOILT[]   = "soil_temperature";
const char VV_SFC_LANDTYP[] = "land_type_index";
const char VV_SFC_VEGTYP[]  = "vegetation_type_index";
const char VV_SFC_SOILTYP[] = "soil_type";
const char VV_GEOMZ[]       = "height";
const char VV_SFC_GEOMZ[]   = "surface_altitude";
const char VV_SFC_ROUGH[]   = "surface_roughness_length";
const char VV_SFC_T[]       = "surface_temperature";
const char VV_SFC_FACT10[]  = "wind_reduction_factor_at_10m";
const char VV_SFC_EMISS[]   = "surface_emissivity";
const char VV_SFC_SSS[]     = "sea_surface_salinity";
const char VV_OPT_DEPTH[]   = "optical_thickness_of_atmosphere_layer";
const char VV_RADIANCE[]    = "toa_outgoing_radiance_per_unit_wavenumber";
const char VV_TB[]          = "brightness_temperature";
const char VV_TB_CLR[]      = "brightness_temperature_assuming_clear_sky";
const char VV_TOTAL_TRANSMIT[]= "toa_total_transmittance";
const char VV_LVL_TRANSMIT[]= "transmittances_of_atmosphere_layer";
const char VV_LVL_WEIGHTFUNC[]= "weightingfunction_of_atmosphere_layer";
const char VV_PMAXLEV_WEIGHTFUNC[]= "pressure_level_at_peak_of_weightingfunction";
const char VV_TSAVG5[]    = "average_surface_temperature_within_field_of_view";
const char VV_SEA_FRIC_VEL[]= "friction_velocity_over_water";
const char VV_REFL[]        = "equivalent_reflectivity_factor";
const char VV_W[]           = "upward_air_velocity";
const char VV_RH[]          = "relative_humidity";
const char VV_WATER_TYPE_RTTOV[] = "water_type";
const char VV_SURF_TYPE_RTTOV[] = "surface_type";
const char VV_SFC_LANDMASK[]   = "landmask";
const char VV_SFC_SEAICEFRAC[] = "seaice_fraction";

const char VV_SEAICEFRAC[]      = "sea_ice_category_area_fraction";
const char VV_SEAICETHICK[]     = "sea_ice_category_thickness";
const char VV_SEAICESNOWTHICK[] = "sea_ice_category_snow_thickness";
const char VV_OCN_CHL[]     = "mass_concentration_of_chlorophyll_in_sea_water";
const char VV_ABS_TOPO[]        = "sea_surface_height_above_geoid";
const char VV_OCN_POT_TEMP[]    = "sea_water_potential_temperature";
const char VV_OCN_CON_TEMP[]    = "sea_water_conservative_temperature";
const char VV_OCN_ABS_SALT[]    = "sea_water_absolute_salinity";
const char VV_OCN_PRA_SALT[]    = "sea_water_practical_salinity";
const char VV_OCN_SALT[]        = "sea_water_salinity";
const char VV_OCN_LAY_THICK[]   = "sea_water_cell_thickness";
const char VV_OCN_SST[]         = "sea_surface_temperature";
const char VV_SEA_TD[]          = "sea_surface_foundation_temperature";
const char VV_LATENT_VAP[]      = "latent_heat_vaporization";
const char VV_SW_RAD[]          = "net_downwelling_shortwave_radiation";
const char VV_LATENT_HEAT[]     = "upward_latent_heat_flux_in_air";
const char VV_SENS_HEAT[]       = "upward_sensible_heat_flux_in_air";
const char VV_LW_RAD[]          = "net_downwelling_longwave_radiation";

const char VV_DU001[] = "mass_fraction_of_dust001_in_air";
const char VV_DU002[] = "mass_fraction_of_dust002_in_air";
const char VV_DU003[] = "mass_fraction_of_dust003_in_air";
const char VV_DU004[] = "mass_fraction_of_dust004_in_air";
const char VV_DU005[] = "mass_fraction_of_dust005_in_air";
const char VV_SS001[] = "mass_fraction_of_sea_salt001_in_air";
const char VV_SS002[] = "mass_fraction_of_sea_salt002_in_air";
const char VV_SS003[] = "mass_fraction_of_sea_salt003_in_air";
const char VV_SS004[] = "mass_fraction_of_sea_salt004_in_air";
const char VV_SS005[] = "mass_fraction_of_sea_salt005_in_air";
const char VV_BCPHOBIC[] = "mass_fraction_of_hydrophobic_black_carbon_in_air";
const char VV_BCPHILIC[] = "mass_fraction_of_hydrophilic_black_carbon_in_air";
const char VV_OCPHOBIC[] = "mass_fraction_of_hydrophobic_organic_carbon_in_air";
const char VV_OCPHILIC[] = "mass_fraction_of_hydrophilic_organic_carbon_in_air";
const char VV_SULFATE[] = "mass_fraction_of_sulfate_in_air";
const char VV_NO3AN1[] = "mass_fraction_of_nitrate001_in_air";
const char VV_NO3AN2[] = "mass_fraction_of_nitrate002_in_air";
const char VV_NO3AN3[] = "mass_fraction_of_nitrate003_in_air";
const char VV_EXT1[] = "volume_extinction_in_air_due_to_aerosol_particles_lambda1";
const char VV_EXT2[] = "volume_extinction_in_air_due_to_aerosol_particles_lambda2";
const char VV_EXT3[] = "volume_extinction_in_air_due_to_aerosol_particles_lambda3";
const char VV_AIRDENS[] = "moist_air_density";

}  // namespace vader

#endif  // SRC_VADER_VADERVARIABLES_H_
