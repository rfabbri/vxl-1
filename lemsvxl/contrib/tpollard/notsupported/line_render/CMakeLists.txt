include_directories( ${VXLCORE_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories(${VXL_MUL_INCLUDE_DIR} )


add_library( line_scene
             line_scene.h          line_scene.cxx )
target_link_libraries( line_scene vcl vgl vnl vil )
set( LINE_SCENE_INCLUDE_DIR ${line_scene_SOURCE_DIR} )

add_executable ( line_render line_render.cxx )
target_link_libraries( line_render line_scene vcl vil vgl vnl )

add_executable ( line_extractor line_extractor.cxx )
target_link_libraries( line_extractor line_scene vcl vil vgl vnl vul )



