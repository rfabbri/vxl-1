# dbrl/dbbas/CMakeLists.txt
# main package for basic operations common to all libraries
# All libraries are mutually independent, except for bxml <-- bdgl
include( ${DBRL_MODULE_PATH}/find_open_cl.cmake )
# xml<->vxl conversion 
#subdirs(bxml)

# uncertainty geometry library
#subdirs(bugl)

# Additions to vnl
#subdirs(bnl)

# Brown Graph Library
#subdirs(bgrl)
# Brown Templated Graph Library
#subdirs(bgrl2)

# Brown statistics library
subdirs(dbsta)

# Brown dynamic programming library
#subdirs(bdpg)

#3D mesh library 
#subdirs(bmsh3d)

#In memory relational database
#subdirs(brdb)

#Brown Imaging Library
#subdirs(bil)

if(OPENCL_FOUND)
	subdirs(dbocl)
endif(OPENCL_FOUND)

subdirs(dkdu)

if( BUILD_GEL )
  # spatial object algorithms
 # subdirs(bsol)

  # topology algorithms
 # subdirs(btol)

  # digital library algorithms
 # subdirs(bdgl)

  # basic vgui classes
  #subdirs(bgui)

  # coin3D incorporated vgui classes
 # subdirs(bgui3d)

  # staged vgl development area
 # subdirs(bvgl)

  #3D modeling library
  #subdirs(bwm)

  #Indexed Mesh Library
 # subdirs(imesh)
endif( BUILD_GEL )


