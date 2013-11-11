INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )


ADD_LIBRARY( line_scene
             line_scene.h          line_scene.cxx )
TARGET_LINK_LIBRARIES( line_scene vcl vgl vnl vil )
SET( LINE_SCENE_INCLUDE_DIR ${line_scene_SOURCE_DIR} )

ADD_EXECUTABLE ( line_render line_render.cxx )
TARGET_LINK_LIBRARIES( line_render line_scene vcl vil vgl vnl )

ADD_EXECUTABLE ( line_extractor line_extractor.cxx )
TARGET_LINK_LIBRARIES( line_extractor line_scene vcl vil vgl vnl vul )



