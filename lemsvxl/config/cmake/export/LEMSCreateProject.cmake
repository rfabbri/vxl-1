# This LEMSVXLCreateProject.cmake file handles the creation of files needed by
# other client projects that use LEMS VXL.  Nothing is built by this
# CMakeLists.txt file.  This CMakeLists.txt file must be processed by
# CMake after all the other CMakeLists.txt files in the LEMS tree,
# which is why the add_subdirectory(config/cmake/export) command is at the end
# of the top level CMakeLists.txt file.


# Save library dependencies.
set(LEMS_CMAKE_DOXYGEN_DIR  ${LEMS_ROOT_SOURCE_DIR}/config/cmake/doxygen)

get_property(LEMSTargets_MODULES GLOBAL PROPERTY LEMSTargets_MODULES)

set(LEMS_CONFIG_CMAKE_DIR "share/lemsvxl/cmake")
if(${CMAKE_VERSION} VERSION_LESS 2.8.12)
   set(INTERFACE_LINK_OPTION "")
else()
   set(INTERFACE_LINK_OPTION "EXPORT_LINK_INTERFACE_LIBRARIES")
endif()

if(LEMSTargets_MODULES)
  export(TARGETS
    ${LEMSTargets_MODULES}
    APPEND
    FILE "${CMAKE_CURRENT_BINARY_DIR}/LEMSTargets.cmake"
    ${INTERFACE_LINK_OPTION}
  )
  install(EXPORT ${LEMS_INSTALL_EXPORT_NAME} DESTINATION ${LEMS_CONFIG_CMAKE_DIR}
          COMPONENT Development)
endif()

# Create the LEMSConfig.cmake file for the build tree.
configure_file(${LEMS_CMAKE_DIR}/LEMSConfig.cmake.in
               ${PROJECT_BINARY_DIR}/LEMSConfig.cmake @ONLY)

configure_file(${LEMS_CMAKE_DIR}/LEMSConfig_export.cmake.in
               ${PROJECT_BINARY_DIR}/config/cmake/export/LEMSConfig.cmake
               @ONLY)

install(FILES
  ${PROJECT_BINARY_DIR}/config/cmake/export/LEMSConfig.cmake
  ${LEMS_CMAKE_DIR}/LEMSStandardOptions.cmake
  ${LEMS_CMAKE_DIR}/UseLEMS.cmake
  DESTINATION ${LEMS_CONFIG_CMAKE_DIR}
)
