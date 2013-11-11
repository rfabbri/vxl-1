# visualizer Application

#050427 Ming's Note

#1. Need to make the following modification of wxWidgets and recompile it.
#In C:\wxWindows-2.4.2\include\wx\msw\setup.h
#Change wxUSE_GLCANVAS to 1

#// Setting wxUSE_GLCANVAS to 1 enables OpenGL support. You need to have OpenGL
#// headers and libraries to be able to compile the library with wxUSE_GLCANVAS
#// set to 1. Note that for some compilers (notably Microsoft Visual C++) you
#// will need to manually add opengl32.lib and glu32.lib to the list of
#// libraries linked with your program if you use OpenGL.
#//
#// Default is 0.
#//
#// Recommended setting: 1 if you intend to use OpenGL, 0 otherwise
##define wxUSE_GLCANVAS       1


#INCLUDE( ${CMAKE_ROOT}/Modules/FindwxWindows.cmake )
INCLUDE( ${LEMS_CMAKE_DIR}/Modules/FindwxWindows.cmake )


IF (WXWINDOWS_FOUND)

IF (WIN32)

INCLUDE( ${CMAKE_ROOT}/Modules/USE_wxWindows.cmake )

ADD_DEFINITIONS(-D__WXMSW__)
#ADD_DEFINITIONS(-DWINVER=0x400)
#ADD_DEFINITIONS(-DwxUSE_GUI=1)

IF(WXWINDOWS_INCLUDE_PATH)
  INCLUDE_DIRECTORIES(${WXWINDOWS_INCLUDE_PATH})
ENDIF(WXWINDOWS_INCLUDE_PATH)

IF(CMAKE_WX_CXX_FLAGS)
  SET(CMAKE_CXX_FLAGS "${CMAKE_WX_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
ENDIF(CMAKE_WX_CXX_FLAGS)


#SET (USE_FREETYPE "YES")
#INCLUDE( ${LEMS_CMAKE_DIR}/Modules/UseSoWin.cmake )

#SET (USE_FTGL "YES")
#INCLUDE( ${LEMS_CMAKE_DIR}/Modules/UseSoWin.cmake )


INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/visualizer )

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )


SET( visualizer_sources

    test_include.cxx

    algo_arc.h
    algo_arctools.h
    algo_biarc.h
    algo_conic.h
    algo_esenergy.h                 algo_esenergy.cxx
    algo_euler.h                    algo_euler.cxx
    algo_eulerequation.h
    algo_eulergeom.h                algo_eulergeom.cxx
    algo_eulerspline.h              algo_eulerspline.cxx

    algo_biarc.cxx
    algo_fresnel.cxx
    algo_lookup.cxx

    base_common.h
    base_defines.h
    base_filedir.h                              base_filedir.cxx
    base_fuzzyboolean.h
    base_geometry_functions.h
    base_i386defines.h
    base_options.h                              base_options.cxx
    base_points.h
    base_sstream2.h                             base_sstream2.cxx
    base_common.h

    basegui_allbitmaps.h
    basegui_baseguigeometry.cxx
    basegui_baseguigeometry.h
    basegui_baseplugin.h
    basegui_basewindow.cxx
    basegui_basewindow.h
    basegui_bounds.h
    basegui_color.cxx
    basegui_color.h
    basegui_debugdisplayer.cxx
    basegui_debugdisplayer.h
    basegui_drawingwindow.cxx
    basegui_drawingwindow.h
    basegui_geomnodes.cxx
    basegui_geomnodes.h
    basegui_glfont.cxx
    basegui_glfont.h
    basegui_glgraphics.cxx
    basegui_glgraphics.h
    basegui_glvisitors.cxx
    basegui_glvisitors.h
    basegui_glwindow.cxx
    basegui_glwindow.h
    basegui_gtool.cxx
    basegui_gtool.h
    basegui_linetools.h
    basegui_mainguiwindow.cxx
    basegui_mainguiwindow.h
    basegui_messageprinter.h
    basegui_msgout.h
    basegui_nodes.cxx
    basegui_nodes.h
    basegui_pluginmanager.cxx
    basegui_pluginmanager.h
    basegui_pointtools.h
    basegui_printer.cxx
    basegui_printer.h
    basegui_rgbimage.cxx
    basegui_rgbimage.h
    basegui_scenedrawer.cxx
    basegui_scenedrawer.h
    basegui_scenegraph.cxx
    basegui_scenegraph.h
    basegui_selection.cxx
    basegui_selection.h
    basegui_style.cxx
    basegui_style.h
    basegui_tools.cxx
    basegui_tools.h
    basegui_view.h
    basegui_visitor.cxx
    basegui_visitor.h
    basegui_xform.h
    basegui_xpbitmaps.h
    basegui_xpdefs.h   
    basegui_xpgl.h
    basegui_xpglu.h          
    basegui_xpmath.h      
    
    command.h                        command.cxx
    commandoptions.h                 commandoptions.cxx

    drawtest/drawtest_window.cxx     drawtest/drawtest_window.h
    drawtest/drawtest_plugin.cxx

)

ADD_EXECUTABLE( visualizer WIN32  ${visualizer_sources} resource.rc )

# add the required libraries into this list
TARGET_LINK_LIBRARIES( visualizer  C:/wxWindows-2.4.2/lib/wxmswd vcl opengl32 glu32 )    #  

ENDIF (WIN32)

ENDIF (WXWINDOWS_FOUND)

