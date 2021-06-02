ecbuild_generate_config_headers( DESTINATION ${INSTALL_INCLUDE_DIR}/vader )

list( APPEND vader_src_files
vader/vader.h
vader/recipe.h
vader/recipe.cc
vader/RecipeBase.h
vader/RecipeBase.cc
vader/cookbook.h
vader/vader.cc
vader/recipes/TempToPTemp.h
vader/recipes/TempToPTemp.cc
vader/recipes/PressureToDelP.h
vader/recipes/PressureToDelP.cc
)

# if (VADER_BUILD_LANGUAGE_FORTRAN)
#   list( APPEND vader_src_files obsspace_mod.F90)
# endif()

# list( APPEND vader_fortran_interface_includes
# obsspace_interface.f
# )

include(GNUInstallDirs)
ecbuild_add_library( TARGET ${PROJECT_NAME}
                     SOURCES ${${PROJECT_NAME}_src_files}
                     INSTALL_HEADERS LISTED
                     HEADER_DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
                     LINKER_LANGUAGE CXX )

# target_link_libraries( ${PROJECT_NAME} PUBLIC NetCDF::NetCDF_C )
# target_link_libraries( ${PROJECT_NAME} PUBLIC fckit )
target_link_libraries( ${PROJECT_NAME} PUBLIC ${oops_LIBRARIES} ) #TODO: Change to "oops::oops" once oops adds namespace support

#Configure include directory layout for build-tree to match install-tree
set(BUILD_DIR_INCLUDE_PATH ${CMAKE_BINARY_DIR}/${PROJECT_NAME}/include)
add_custom_target(${PROJECT_NAME}_headers ALL
                    COMMAND ${CMAKE_COMMAND} -E make_directory ${BUILD_DIR_INCLUDE_PATH}
                    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR} ${BUILD_DIR_INCLUDE_PATH}/${PROJECT_NAME})
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_headers)
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${BUILD_DIR_INCLUDE_PATH}>)

