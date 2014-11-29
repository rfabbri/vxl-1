# contrib/lems_ntrinh/CMakeLists.txt
# Developement for maruthi
# Main driver cmake file, simply subdirs into other directories

PROJECT( maruthi )


SET( CMAKE_VLFEAT_LIBS             CACHE PATH "Path of vlfeat libs" )
SET( CMAKE_VLFEAT_INCLUDES             CACHE PATH "Path of vlfeat includes" )

#GO TO SUBDIRS
#pb Edge Detector code
#SUBDIRS( pbEdgeDet )

#GO TO SUBDIRS
#en250_project final project
#Nhon changed this folder very drastically and the code at this point will
#not work with the new top down shock shap stuff
#Folder disabled (commented) at 11/04/09
#SUBDIRS( en250_project )

#GO TO SUBDIRS
#SUBDIRS( geometric_embedding )

#SUBDIRS( proximity_graph )

SUBDIRS( dbskfg )

SUBDIRS( con_graph )

#SUBDIRS(lbnn)
