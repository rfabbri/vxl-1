# dbrl/dbbas/CMakeLists.txt
# main package for basic operations common to all libraries
# All libraries are mutually independent, except for bxml <-- bdgl
include( ${DBRL_MODULE_PATH}/find_open_cl.cmake )
# xml<->vxl conversion 
#add_subdirectory(bxml)

# uncertainty geometry library
#add_subdirectory(bugl)

# Additions to vnl
#add_subdirectory(bnl)

# Brown Graph Library
#add_subdirectory(bgrl)
# Brown Templated Graph Library
#add_subdirectory(bgrl2)

# Brown statistics library
add_subdirectory(dbsta)

# Brown dynamic programming library
#add_subdirectory(bdpg)

#3D mesh library 
#add_subdirectory(bmsh3d)

#In memory relational database
#add_subdirectory(brdb)

#Brown Imaging Library
#add_subdirectory(bil)

if(OPENCL_FOUND)
	add_subdirectory(dbocl)
endif(OPENCL_FOUND)

add_subdirectory(dkdu)

if( BUILD_GEL )
  # spatial object algorithms
 # add_subdirectory(bsol)

  # topology algorithms
 # add_subdirectory(btol)

  # digital library algorithms
 # add_subdirectory(bdgl)

  # basic vgui classes
  #add_subdirectory(bgui)

  # coin3D incorporated vgui classes
 # add_subdirectory(bgui3d)

  # staged vgl development area
 # add_subdirectory(bvgl)

  #3D modeling library
  #add_subdirectory(bwm)

  #Indexed Mesh Library
 # add_subdirectory(imesh)
endif( BUILD_GEL )


