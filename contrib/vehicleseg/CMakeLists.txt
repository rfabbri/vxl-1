# contrib/vehicleseg/CMakeLists.txt
# Developement for : vehicleseg

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

# my bvis1 based GUI
SUBDIRS( vehgui )


# executable or dlls
SUBDIRS(exe)

