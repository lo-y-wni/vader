ecbuild_generate_config_headers( DESTINATION ${INSTALL_INCLUDE_DIR}/vader )

list( APPEND vader_src_files
vader/vader.h
vader/recipe.h
vader/recipe.cc
vader/RecipeBase.h
vader/RecipeBase.cc
vader/VaderConstructConfig.h
vader/VaderConstructConfig.cc
vader/VaderConfigVars.h
vader/VaderConfigVars.cc
vader/vader.cc
vader/VaderParameters.h
vader/recipes/AirPotentialTemperature_A.cc
vader/recipes/AirPotentialTemperature_B.cc
vader/recipes/AirPotentialTemperature.h
vader/recipes/AirPressure_A.cc
vader/recipes/AirPressureAtInterface_A.cc
vader/recipes/AirPressureAtInterface_B.cc
vader/recipes/AirPressureAtInterface.h
vader/recipes/AirPressure.h
vader/recipes/AirPressureThickness_A.cc
vader/recipes/AirPressureThickness.h
vader/recipes/AirPressureToKappa_A.cc
vader/recipes/AirPressureToKappa.h
vader/recipes/AirTemperature_A.cc
vader/recipes/AirTemperature_B.cc
vader/recipes/AirTemperature.h
vader/recipes/AirVirtualTemperature_A.cc
vader/recipes/AirVirtualTemperature.h
vader/recipes/LnAirPressureAtInterface_A.cc
vader/recipes/LnAirPressureAtInterface.h
vader/recipes/DryAirDensityLevelsMinusOne.h
vader/recipes/DryAirDensityLevelsMinusOne_A.cc
vader/recipes/HydrostaticExnerLevels.h
vader/recipes/HydrostaticExnerLevels_A.cc
vader/recipes/SurfaceAirPressure.h
vader/recipes/SurfaceAirPressure_A.cc
vader/recipes/TestRecipes.cc
vader/recipes/TestRecipes.h
vader/recipes/TotalWater.h
vader/recipes/TotalWater_A.cc
vader/recipes/uwind_at_10m_A.cc
vader/recipes/uwind_at_10m.h
vader/recipes/VirtualPotentialTemperature_A.cc
vader/recipes/VirtualPotentialTemperature_B.cc
vader/recipes/VirtualPotentialTemperature.h
vader/recipes/vwind_at_10m_A.cc
vader/recipes/vwind_at_10m.h
mo/common_varchange.h
mo/common_varchange.cc
mo/common_linearvarchange.h
mo/common_linearvarchange.cc
mo/constants.h
mo/functions.h
mo/functions.cc
mo/model2geovals_linearvarchange.h
mo/control2analysis_linearvarchange.h
mo/control2analysis_linearvarchange.cc
mo/control2analysis_varchange.h
mo/control2analysis_varchange.cc
mo/eval_air_pressure.h
mo/eval_air_pressure.cc
mo/eval_air_pressure_levels.h
mo/eval_air_pressure_levels.cc
mo/eval_air_temperature.h
mo/eval_air_temperature.cc
mo/eval_dry_air_density.h
mo/eval_dry_air_density.cc
mo/eval_exner.h
mo/eval_exner.cc
mo/eval_geostrophic_to_hydrostatic_pressure.h
mo/eval_geostrophic_to_hydrostatic_pressure.cc
mo/eval_hydrostatic_balance.h
mo/eval_hydrostatic_balance.cc
mo/eval_mio_fields.h
mo/eval_mio_fields.cc
mo/eval_moisture_incrementing_operator.h
mo/eval_moisture_incrementing_operator.cc
mo/eval_moisture_control.h
mo/eval_moisture_control.cc
mo/eval_sat_vapour_pressure.h
mo/eval_sat_vapour_pressure.cc
mo/eval_surface_pressure.h
mo/eval_surface_pressure.cc
mo/eval_surface_temperature.h
mo/eval_surface_temperature.cc
mo/eval_surface_wind.h
mo/eval_surface_wind.cc
mo/eval_total_relative_humidity.h
mo/eval_total_relative_humidity.cc
mo/eval_virtual_potential_temperature.h
mo/eval_virtual_potential_temperature.cc
mo/model2geovals_linearvarchange.h
mo/model2geovals_linearvarchange.cc
mo/model2geovals_varchange.h
mo/model2geovals_varchange.cc
mo/svp_interface.F90
)

include(GNUInstallDirs)
ecbuild_add_library( TARGET ${PROJECT_NAME}
                     SOURCES ${${PROJECT_NAME}_src_files}
                     INSTALL_HEADERS LISTED
                     HEADER_DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
                     LINKER_LANGUAGE CXX )

target_link_libraries( ${PROJECT_NAME} PUBLIC ${oops_LIBRARIES} ) #TODO: Change to "oops::oops" once oops adds namespace support

#Configure include directory layout for build-tree to match install-tree
set(BUILD_DIR_INCLUDE_PATH ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/include)
add_custom_target(${PROJECT_NAME}_headers ALL
                    COMMAND ${CMAKE_COMMAND} -E make_directory ${BUILD_DIR_INCLUDE_PATH}
                    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR} ${BUILD_DIR_INCLUDE_PATH}/${PROJECT_NAME})
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_headers)

## Include paths
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
                                      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
