# (C) Copyright 2017-2021 UCAR
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.

#
# VADER bundle
#

cmake_minimum_required( VERSION 3.12 FATAL_ERROR )

find_package( ecbuild 3.5 REQUIRED HINTS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../ecbuild)

project( vader-bundle VERSION 1.0.0 LANGUAGES C CXX Fortran )

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include( ecbuild_bundle )

# Default release mode
set( ECBUILD_DEFAULT_BUILD_TYPE Release)

# Enable MPI
set( ENABLE_MPI ON CACHE BOOL "Compile with MPI" )

# Initialize
ecbuild_bundle_initialize()

#ecbuild_bundle( PROJECT jedicmake GIT "https://github.com/JCSDA-internal/jedi-cmake.git" BRANCH develop UPDATE )
include( /opt/view/share/jedicmake/Functions/git_functions.cmake )

# ECMWF libs
# ----------
# Core JEDI repositories
ecbuild_bundle( PROJECT oops  GIT "https://github.com/JCSDA-internal/oops.git"  )
ecbuild_bundle( PROJECT vader GIT "https://github.com/JCSDA-internal/vader.git" )

# Build Doxygen documentation
option(BUILD_VADER_BUNDLE_DOC "Build documentation" OFF)
if(BUILD_VADER_BUNDLE_DOC)
   add_subdirectory( Documentation )
endif(BUILD_VADER_BUNDLE_DOC)

ecbuild_bundle_finalize()

include(cmake/cdash-integration.cmake)
include(CTest)
