if (VXL_VGUI_FOUND)
  include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake )
endif (VXL_VGUI_FOUND)


set( biotree_SOURCE_DIR ${LEMS_SOURCE_DIR}/contrib/biotree )

include_directories( ${biotree_SOURCE_DIR} )

# basic utilities of biotree project
add_subdirectory( biob )

# CT simulation (bother Rahul with your questions)
add_subdirectory( biocts )

# Lung Modeling (bother Rahul with your questions)
add_subdirectory( biolung )

# gui
#add_subdirectory( gui )

# X-Ray Multiview Geometry
add_subdirectory( xmvg )

# Detection Library
add_subdirectory( det )

# Scan Library
add_subdirectory( xscan )

# Gui managers
if(SIMVOLEON_FOUND)
  add_subdirectory( biov )
endif(SIMVOLEON_FOUND)

# Executables
add_subdirectory( bin )

# Resources (Image and Splat)
add_subdirectory( resc )

# control processes
add_subdirectory( proc )

# can's CT-simulator
#add_subdirectory( biosim )

# filtering on 3D vol data
#add_subdirectory( vol3d )

#Biotree standard coordinate system
add_subdirectory( bscs )

add_subdirectory( geom )

# projected filters method
add_subdirectory( proj )

# optimized projected filters method
add_subdirectory( proj_opt )

# cone-beam CT simulator
add_subdirectory( biocbs )

# slice-based processing
add_subdirectory( slice )

add_subdirectory( roipick )

add_subdirectory( histogrampick )
