# contrib/vehicleseg/CMakeLists.txt
# Developement for : vehicleseg

include_directories( ${lems_SOURCE_DIR}/breye1 )

# my bvis1 based GUI
subdirs( vehgui )


# executable or dlls
subdirs(exe)

