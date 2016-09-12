# contrib/lems_ntrinh/CMakeLists.txt
# Developement for maruthi
# Main driver cmake file, simply subdirs into other directories

project( maruthi )


set( CMAKE_VLFEAT_LIBS             CACHE PATH "Path of vlfeat libs" )
set( CMAKE_VLFEAT_INCLUDES             CACHE PATH "Path of vlfeat includes" )


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )

#GO TO SUBDIRS
#pb Edge Detector code
#subdirs( pbEdgeDet )

#GO TO SUBDIRS
#en250_project final project
#Nhon changed this folder very drastically and the code at this point will
#not work with the new top down shock shap stuff
#Folder disabled (commented) at 11/04/09
#subdirs( en250_project )

#GO TO SUBDIRS
#subdirs( geometric_embedding )

#subdirs( proximity_graph )

subdirs( dbskfg )

subdirs( con_graph )

subdirs( shape_align )
#subdirs(lbnn)
