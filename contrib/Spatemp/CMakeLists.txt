# contrib/spatemp/CMakeLists.txt

include_directories( ${lems_SOURCE_DIR}/breye1 )



#pro
subdirs(pro)


#vis
subdirs(vis)

#gui
subdirs(gui)




#lib
subdirs(algo)

#graph cuts
subdirs(gc)
