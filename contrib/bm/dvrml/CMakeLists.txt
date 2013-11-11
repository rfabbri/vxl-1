# this is contrib/bm/dvrml/CMakeLists.txt

# this is a library to define a basic api for writing vrml files

project( dvrml )

INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
#MESSAGE( "VXL_CORE_INCLUDE_DIR = " ${VXL_CORE_INCLUDE_DIR} )

SET( dvrml_sources

	 dvrml.h							dvrml.cxx 
	 dvrml_appearance.h
	 dvrml_appearance_sptr.h
	 vbl_smart_ptr+dvrml_appearance-.cxx
     dvrml_shape.h	
	 dvrml_shape_sptr.h
	 vbl_smart_ptr+dvrml_shape-.cxx
	 dvrml_point.h						dvrml_point.cxx
	 dvrml_point_sptr.h
	 vbl_smart_ptr+dvrml_point-.cxx
	 dvrml_box.h						dvrml_box.cxx
	 dvrml_box_sptr.h
	 vbl_smart_ptr+dvrml_box-.cxx
	 dvrml_cone.h						dvrml_cone.cxx
	 dvrml_cone_sptr.h
	 vbl_smart_ptr+dvrml_cone-.cxx
	 dvrml_cylinder.h					dvrml_cylinder.cxx
	 dvrml_cylinder_sptr.h
	 vbl_smart_ptr+dvrml_cylinder-.cxx
	 dvrml_indexed_lineset.h			dvrml_indexed_lineset.cxx
	 dvrml_indexed_lineset_sptr.h
	 vbl_smart_ptr+dvrml_indexed_lineset-.cxx
	 )
	 
ADD_LIBRARY( dvrml ${dvrml_sources} )
TARGET_LINK_LIBRARIES( dvrml vul vbl vgl vnl )	 

IF( BUILD_TESTING )
	SUBDIRS( tests )
ENDIF( BUILD_TESTING )
		