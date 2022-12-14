ecbuild_generate_config_headers( DESTINATION ${INSTALL_INCLUDE_DIR}/vader )

list( APPEND vader_src_files
vader/vader.h
vader/recipe.h
vader/recipe.cc
vader/RecipeBase.h
vader/RecipeBase.cc
vader/vader.cc
vader/VaderParameters.h
vader/recipes/AirTemperature.h
vader/recipes/AirTemperature.cc
vader/recipes/PressureToDelP.h
vader/recipes/PressureToDelP.cc
vader/recipes/TempToPTemp.h
vader/recipes/TempToPTemp.cc
vader/recipes/TempToVTemp.h
vader/recipes/TempToVTemp.cc
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
