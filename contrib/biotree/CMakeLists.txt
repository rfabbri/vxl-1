if (VXL_VGUI_FOUND)
  include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
endif (VXL_VGUI_FOUND)


set( biotree_SOURCE_DIR ${lems_SOURCE_DIR}/contrib/biotree )

include_directories( ${biotree_SOURCE_DIR} )

# basic utilities of biotree project
subdirs( biob )

# CT simulation (bother Rahul with your questions)
subdirs( biocts )

# Lung Modeling (bother Rahul with your questions)
subdirs( biolung )

# gui
#subdirs( gui )

# X-Ray Multiview Geometry
subdirs( xmvg )

# Detection Library
subdirs( det )

# Scan Library
subdirs( xscan )

# Gui managers
if(SIMVOLEON_FOUND)
  subdirs( biov )
endif(SIMVOLEON_FOUND)

# Executables
subdirs( bin )

# Resources (Image and Splat)
subdirs( resc )

# control processes
subdirs( proc )

# can's CT-simulator
#subdirs( biosim )

# filtering on 3D vol data
#subdirs( vol3d )

#Biotree standard coordinate system
subdirs( bscs )

subdirs( geom )

# projected filters method
subdirs( proj )

# optimized projected filters method
subdirs( proj_opt )

# cone-beam CT simulator
subdirs( biocbs )

# slice-based processing
subdirs( slice )

subdirs( roipick )

subdirs( histogrampick )
