# contrib/spatemp/CMakeLists.txt

include_directories( ${LEMS_SOURCE_DIR}/breye1 )



#pro
add_subdirectory(pro)


#vis
add_subdirectory(vis)

#gui
add_subdirectory(gui)




#lib
add_subdirectory(algo)

#graph cuts
add_subdirectory(gc)
