# contrib/pointmatching2d/CMakeLists.txt
# Developement for :2D fetaure registration

include_directories( ${lems_SOURCE_DIR}/breye1 )


#main library
subdirs(georegister)

#pro
subdirs(pro)


#vis
subdirs(vis)

#gui
subdirs(gui)


#dt
subdirs(exe)

#dt
subdirs(dt)

#cmd
subdirs(cmd)
