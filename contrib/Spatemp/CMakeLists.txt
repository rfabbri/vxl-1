# contrib/spatemp/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )



#pro
SUBDIRS(pro)


#vis
SUBDIRS(vis)

#gui
SUBDIRS(gui)




#lib
SUBDIRS(algo)

#graph cuts
SUBDIRS(gc)
