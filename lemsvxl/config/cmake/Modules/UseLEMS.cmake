# lemsvxl/config/cmake/Modules/UseLEMS.cmake
# (also copied by CMake to the top-level of the lemsvxl build tree)
#
# This CMake file may be included by projects outside LEMS.  It
# configures them to make use of LEMS headers and libraries.  The file
# is written to work in one of two ways.
#
# The preferred way to use LEMS from an outside project with UseLEMS.cmake:
#
#  find_package(LEMS)
#  if(LEMS_FOUND)
#    include(${LEMS_CMAKE_DIR}/UseLEMS.cmake)
#  else()
#    message("LEMS_DIR should be set to the LEMS build directory.")
#  endif()
#
# Read lemsvxl/config/cmake/LEMSConfig.cmake for the list of variables
# provided.  The names have changed to reduce namespace pollution.
# The old names can be made available by placing this line before
# including UseLEMS.cmake:
#
# This UseLEMS.cmake no longer adds options and testing features automatically
# to projects including it unless this line appears before including it:
#
#  set(LEMS_PROVIDE_STANDARD_OPTIONS 1)
#
# For example, in order to enable full backward-compatibility while
# still using FIND_PACKAGE, use these lines:
#
#  find_package(LEMS)
#  if(LEMS_FOUND)
#    set(LEMS_PROVIDE_STANDARD_OPTIONS 1)
#    include(${LEMS_CMAKE_DIR}/UseLEMS.cmake)
#  else()
#    message("LEMS_DIR should be set to the LEMS build directory.")
#  endif()
#
# The old way to use LEMS from an outside project with UseLEMS.cmake is
# also supported for backward-compatibility:
#
#  set(LEMS_BINARY_PATH "" CACHE PATH "LEMS build directory (location of UseLEMS.cmake)")
#  if(LEMS_BINARY_PATH)
#    include(${LEMS_BINARY_PATH}/UseLEMS.cmake)
#  else()
#    message("LEMS_BINARY_PATH should be set to the LEMS build directory (location of UseLEMS.cmake)" )
#  endif()
#

# If this file has been included directly by a user project instead of
# through LEMS_USE_FILE from LEMSConfig.cmake, simulate old behavior.
if(NOT LEMS_CONFIG_CMAKE)
  if(LEMS_BINARY_PATH)

    # Let FIND_PACKAGE import the LEMSConfig.cmake module.
    set(LEMS_DIR ${LEMS_BINARY_PATH})
    find_package(LEMS)

    # Enable compatibility mode.
    set(LEMS_PROVIDE_STANDARD_OPTIONS 1)

  endif()
endif()

# LEMSConfig.cmake has now been included.  Use its settings.
if(LEMS_CONFIG_CMAKE)
  # Load the compiler settings used for LEMS.
  if(LEMS_BUILD_SETTINGS_FILE)
    option( LEMS_IMPORT_BUILD_SETTINGS "Import build settings (compiler flags, generator) from LEMS?" YES )
    mark_as_advanced( LEMS_IMPORT_BUILD_SETTINGS )
    if( LEMS_IMPORT_BUILD_SETTINGS )
      include(${CMAKE_ROOT}/Modules/CMakeImportBuildSettings.cmake)
      CMAKE_IMPORT_BUILD_SETTINGS(${LEMS_BUILD_SETTINGS_FILE})
    endif()
  endif()

  # Add link directories needed to use LEMS.
  link_directories(${LEMS_LIBRARY_DIR})

  # XXX for now, doxygen is not supported
  #  if(LEMS_CMAKE_DOXYGEN_DIR)
    # Allow use of LEMS's cmake/doxygen framework
    #    include(${LEMS_CMAKE_DOXYGEN_DIR}/doxygen.cmake)
    #  endif()

  if(LEMS_PROVIDE_STANDARD_OPTIONS)
    # Provide the standard set of LEMS CMake options to the project.
    include(${LEMS_CMAKE_DIR}/LEMSStandardOptions.cmake)
  endif()
endif()
