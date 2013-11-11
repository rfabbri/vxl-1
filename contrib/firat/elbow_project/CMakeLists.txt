IF(UNIX)
    SET (VTK_LIBRARY_PATH "/home/firat/vtk/release/lib/vtk-5.8/")
    SET (VTK_INCLUDE_PATH "/home/firat/vtk/release/include/vtk-5.8")
    SET (ANN_LIBRARY_PATH "/home/firat/ann/lib")
    SET (ANN_INCLUDE_PATH "/home/firat/ann/include")
    SET (BOOST_LIBRARY_PATH "/home/firat/boost/lib") 
    SET (BOOST_INCLUDE_PATH "/home/firat/boost/include")
ELSE(UNIX)
    
ENDIF(UNIX)


subdirs(algo)
subdirs(gui)
subdirs(pro)
subdirs(storage)
subdirs(sandbox)