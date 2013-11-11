# this is dbrec3d_gui - uses QT


INCLUDE(${CMAKE_ROOT}/Modules/FindOpenGL.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/FindGLUT.cmake)
INCLUDE(${CMAKE_ROOT}/Modules/FindQt.cmake)
INCLUDE(${LEMS_CMAKE_DIR}/Modules/FindQGLViewer.cmake)
INCLUDE(${LEMS_CMAKE_DIR}/Modules/FindQWT.cmake)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dbrec_lib/dbrec3d )


IF(QT4_INSTALLED)
  MESSAGE("qt4 installed")
  IF(GLUT_FOUND)
    MESSAGE("GLUT installed")
    IF(QGLVIEWER_FOUND AND QWT_FOUND)
      MESSAGE("QGLViewer  and QWT installed")
      MESSAGE(${QGLVIEWER_INCLUDE_DIR})
      MESSAGE(${QWT_INCLUDE_DIR})
      
      #    ADD_DEFINITIONS(-DQT3_SUPPORT_WARNINGS ${QT_DEFINITIONS} ${QT_USE_QTOPENGL})
          
          
      INCLUDE_DIRECTORIES( ${QGLVIEWER_INCLUDE_DIR} )
      INCLUDE_DIRECTORIES( ${QGLVIEWER_INCLUDE_DIR}/QGLViewer )
      INCLUDE_DIRECTORIES( ${QWT_INCLUDE_DIR} )
      INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} ${QT_QT_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} 
                           ${QT_QTCORE_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})
      
      
      SET(dbrec3d_qt_sources
       dbrec3d_scene_model.cxx       dbrec3d_scene_model.h
       dbrec3d_scene_widgets.cxx     dbrec3d_scene_widgets.h
       dbrec3d_scene_viewer.cxx      dbrec3d_scene_viewer.h
       dbrec3d_histogram_widgets.cxx dbrec3d_histogram_widgets.h
       qwt_histogram.cxx             qwt_histogram.h
       dbrec3d_double_slider.cxx     dbrec3d_double_slider.h
       dbrec3d_line_plot.cxx         dbrec3d_line_plot.h
       #window.cxx  window.h
       #glwidget.cxx glwidget.h
       #dbrec3d_line.cxx       dbrec3d_line.h

      )

      QT_WRAP_CPP(dbrec3d_gui dbrec3d_qt_moc_files
          dbrec3d_scene_viewer.h
          dbrec3d_scene_widgets.h
          dbrec3d_histogram_widgets.h
          qwt_histogram.h
          dbrec3d_double_slider.h
          dbrec3d_line_plot.h
          #glwidget.h
          #window.h
          #dbrec3d_line.h
      )
      
      SET(dbrec3d_gui_sources ${dbrec3d_qt_sources} ${dbrec3d_qt_moc_files} )

      ADD_LIBRARY(dbrec3d_gui ${dbrec3d_gui_sources})

      TARGET_LINK_LIBRARIES( dbrec3d_gui dbrec3d boxm boct bvpl )
      TARGET_LINK_LIBRARIES( dbrec3d_gui ${QGLVIEWER_LIBRARY} )
      TARGET_LINK_LIBRARIES( dbrec3d_gui ${QWT_LIBRARY} )
      TARGET_LINK_LIBRARIES( dbrec3d_gui ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} ${QT_QTXML_LIBRARY})
      TARGET_LINK_LIBRARIES( dbrec3d_gui ${GLUT_LIBRARIES})
      TARGET_LINK_LIBRARIES( dbrec3d_gui ${OPENGL_LIBRARIES})
   
      SUBDIRS(vtk)

    ENDIF(QGLVIEWER_FOUND AND QWT_FOUND)
    
  ENDIF(GLUT_FOUND)
 
ENDIF(QT4_INSTALLED)



