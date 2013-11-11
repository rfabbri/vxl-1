IF (VXL_VGUI_FOUND)
  INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
ENDIF (VXL_VGUI_FOUND)


SET( biotree_SOURCE_DIR ${lems_SOURCE_DIR}/contrib/biotree )

INCLUDE_DIRECTORIES( ${biotree_SOURCE_DIR} )

# basic utilities of biotree project
SUBDIRS( biob )

# CT simulation (bother Rahul with your questions)
SUBDIRS( biocts )

# Lung Modeling (bother Rahul with your questions)
SUBDIRS( biolung )

# gui
#SUBDIRS( gui )

# X-Ray Multiview Geometry
SUBDIRS( xmvg )

# Detection Library
SUBDIRS( det )

# Scan Library
SUBDIRS( xscan )

# Gui managers
IF(SIMVOLEON_FOUND)
  SUBDIRS( biov )
ENDIF(SIMVOLEON_FOUND)

# Executables
SUBDIRS( bin )

# Resources (Image and Splat)
SUBDIRS( resc )

# control processes
SUBDIRS( proc )

# can's CT-simulator
#SUBDIRS( biosim )

# filtering on 3D vol data
#SUBDIRS( vol3d )

#Biotree standard coordinate system
SUBDIRS( bscs )

SUBDIRS( geom )

# projected filters method
SUBDIRS( proj )

# optimized projected filters method
SUBDIRS( proj_opt )

# cone-beam CT simulator
SUBDIRS( biocbs )

# slice-based processing
SUBDIRS( slice )

SUBDIRS( roipick )

SUBDIRS( histogrampick )
