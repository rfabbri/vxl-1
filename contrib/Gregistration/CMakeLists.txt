# contrib/pointmatching2d/CMakeLists.txt
# Developement for :2D fetaure registration

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )


#main library
SUBDIRS(georegister)

#pro
SUBDIRS(pro)


#vis
SUBDIRS(vis)

#gui
SUBDIRS(gui)


#dt
SUBDIRS(exe)

#dt
SUBDIRS(dt)

#cmd
SUBDIRS(cmd)
