include_directories( ${VXLCORE_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories(${VXL_MUL_INCLUDE_DIR} )


add_library( simple_scene
             simple_scene.h          simple_scene.cxx )
target_link_libraries( simple_scene vcl vgl vnl vil vpgl vpgl_algo )
set( SIMPLE_SCENE_INCLUDE_DIR ${simple_scene_SOURCE_DIR} )

add_executable ( simple_render simple_render.cxx )
target_link_libraries( simple_render simple_scene vcl vil vgl vnl vpgl vpgl_algo )

