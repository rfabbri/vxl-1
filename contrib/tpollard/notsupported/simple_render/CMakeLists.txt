INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )


ADD_LIBRARY( simple_scene
             simple_scene.h          simple_scene.cxx )
TARGET_LINK_LIBRARIES( simple_scene vcl vgl vnl vil vpgl vpgl_algo )
SET( SIMPLE_SCENE_INCLUDE_DIR ${simple_scene_SOURCE_DIR} )

ADD_EXECUTABLE ( simple_render simple_render.cxx )
TARGET_LINK_LIBRARIES( simple_render simple_scene vcl vil vgl vnl vpgl vpgl_algo )

