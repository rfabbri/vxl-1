if(UNIX)
    set (VTK_LIBRARY_PATH "/home/firat/vtk/release/lib/vtk-5.8/")
    set (VTK_INCLUDE_PATH "/home/firat/vtk/release/include/vtk-5.8")
    set (ANN_LIBRARY_PATH "/home/firat/ann/lib")
    set (ANN_INCLUDE_PATH "/home/firat/ann/include")
    set (BOOST_LIBRARY_PATH "/home/firat/boost/lib") 
    set (BOOST_INCLUDE_PATH "/home/firat/boost/include")
else(UNIX)
    
endif(UNIX)


add_subdirectory(algo)
add_subdirectory(gui)
add_subdirectory(pro)
add_subdirectory(storage)
add_subdirectory(sandbox)