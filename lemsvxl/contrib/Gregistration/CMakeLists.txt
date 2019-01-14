# contrib/pointmatching2d/CMakeLists.txt
# Developement for :2D fetaure registration

include_directories( ${LEMS_SOURCE_DIR}/breye1 )


#main library
add_subdirectory(georegister)

#pro
add_subdirectory(pro)


#vis
add_subdirectory(vis)

#gui
add_subdirectory(gui)


#dt
add_subdirectory(exe)

#dt
add_subdirectory(dt)

#cmd
add_subdirectory(cmd)
