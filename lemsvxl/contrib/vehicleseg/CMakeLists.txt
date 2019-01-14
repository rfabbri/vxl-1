# contrib/vehicleseg/CMakeLists.txt
# Developement for : vehicleseg

include_directories( ${LEMS_SOURCE_DIR}/breye1 )

# my bvis1 based GUI
add_subdirectory( vehgui )


# executable or dlls
add_subdirectory(exe)

