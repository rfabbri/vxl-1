# this is dbrec3d_gui - uses QT


include(${CMAKE_ROOT}/Modules/FindOpenGL.cmake)
include(${CMAKE_ROOT}/Modules/FindGLUT.cmake)
include(${CMAKE_ROOT}/Modules/FindQt.cmake)
include(${LEMS_CMAKE_DIR}/Modules/FindQGLViewer.cmake)
include(${LEMS_CMAKE_DIR}/Modules/FindQWT.cmake)
include_directories( ${lems_SOURCE_DIR}/contrib/dbrec_lib/dbrec3d )


if(QT4_INSTALLED)
  message("qt4 installed")
  if(GLUT_FOUND)
    message("GLUT installed")
    if(QGLVIEWER_FOUND AND QWT_FOUND)
      message("QGLViewer  and QWT installed")
      message(${QGLVIEWER_INCLUDE_DIR})
      message(${QWT_INCLUDE_DIR})
      
      #    add_definitions(-DQT3_SUPPORT_WARNINGS ${QT_DEFINITIONS} ${QT_USE_QTOPENGL})
          
          
      include_directories( ${QGLVIEWER_INCLUDE_DIR} )
      include_directories( ${QGLVIEWER_INCLUDE_DIR}/QGLViewer )
      include_directories( ${QWT_INCLUDE_DIR} )
      include_directories( ${QT_INCLUDE_DIR} ${QT_QT_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} 
                           ${QT_QTCORE_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})
      
      
      set(dbrec3d_qt_sources
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

      qt_wrap_cpp(dbrec3d_gui dbrec3d_qt_moc_files
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
      
      set(dbrec3d_gui_sources ${dbrec3d_qt_sources} ${dbrec3d_qt_moc_files} )

      add_library(dbrec3d_gui ${dbrec3d_gui_sources})

      target_link_libraries( dbrec3d_gui dbrec3d boxm boct bvpl )
      target_link_libraries( dbrec3d_gui ${QGLVIEWER_LIBRARY} )
      target_link_libraries( dbrec3d_gui ${QWT_LIBRARY} )
      target_link_libraries( dbrec3d_gui ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} ${QT_QTXML_LIBRARY})
      target_link_libraries( dbrec3d_gui ${GLUT_LIBRARIES})
      target_link_libraries( dbrec3d_gui ${OPENGL_LIBRARIES})
   
      add_subdirectory(vtk)

    endif(QGLVIEWER_FOUND AND QWT_FOUND)
    
  endif(GLUT_FOUND)
 
endif(QT4_INSTALLED)



