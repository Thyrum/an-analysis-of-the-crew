###############################################################################
# Top contributors (to current version):
#   Gereon Kremer, Aina Niemetz
#
# This file is part of the cvc5 project.
#
# Copyright (c) 2009-2021 by the authors listed in the file AUTHORS
# in the top-level source directory and their institutional affiliations.
# All rights reserved.  See the file COPYING in the top-level source
# directory for licensing information.
# #############################################################################
#
# Compile Kissat
# Kissat_INCLUDE_DIR - the Kissat include directory
# Kissat_LIBRARIES - Libraries needed to use Kissat
##

find_path(
	Kissat_INCLUDE_DIR
	NAMES kissat/kissat.h
)
find_library(
	Kissat_LIBRARIES
	NAMES kissat
)

set(Kissat_FOUND_SYSTEM FALSE)
if(Kissat_INCLUDE_DIR AND Kissat_LIBRARIES)
  set(Kissat_FOUND_SYSTEM TRUE)

  # Unfortunately it is not part of the headers
  find_library(Kissat_BINARY NAMES kissat)
  if(Kissat_BINARY)
    execute_process(
      COMMAND ${Kissat_BINARY} --version OUTPUT_VARIALE Kissat_VERSION
    )
  else()
    set(Kissat_VERSION "version unknown")
  endif()

	#check_system_version("Kissat")
endif()

if(NOT Kissat_FOUND_SYSTEM)
	include(ExternalProject)

	set(Kissat_VERSION "sc2021")
	set(Kissat_COMMIT "8174f940565b02716cf37acb04f0bad8dcd334ff")

	ExternalProject_Add(
		Kissat-EP
		BUILD_IN_SOURCE ON
		INSTALL_DIR ${DEPS_BASE}
		GIT_REPOSITORY https://github.com/arminbiere/kissat
		GIT_TAG ${Kissat_COMMIT}
		UPDATE_COMMAND ""
		CONFIGURE_COMMAND <SOURCE_DIR>/configure -fPIC --quiet
											CC=${CMAKE_C_COMPILER}
		INSTALL_COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/build/libkissat.a
										<INSTALL_DIR>/lib/libkissat.a
		COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/src/kissat.h
						<INSTALL_DIR>/include/kissat/kissat.h
		BUILD_BYPRODUCTS <INSTALL_DIR>/lib/libkissat.a
	)

	set(Kissat_INCLUDE_DIR "${DEPS_BASE}/include")
	set(Kissat_LIBRARIES "${DEPS_BASE}/lib/libkissat.a")
endif()

set(Kissat_FOUND TRUE)

add_library(Kissat STATIC IMPORTED GLOBAL)
set_target_properties(Kissat PROPERTIES IMPORTED_LOCATION "${Kissat_LIBRARIES}")
set_target_properties(
  Kissat PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${Kissat_INCLUDE_DIR}"
)

mark_as_advanced(Kissat_FOUND)
mark_as_advanced(Kissat_FOUND_SYSTEM)
mark_as_advanced(Kissat_INCLUDE_DIR)
mark_as_advanced(Kissat_LIBRARIES)

if(Kissat_FOUND_SYSTEM)
  message(STATUS "Found Kissat ${Kissat_VERSION}: ${Kissat_LIBRARIES}")
else()
  message(STATUS "Building Kissat ${Kissat_VERSION}: ${Kissat_LIBRARIES}")
  add_dependencies(Kissat Kissat-EP)
endif()
