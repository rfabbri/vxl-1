# contrib/lems_han/CMakeLists.txt
# Developement for : lems_han




# dll library for recognition of video objects in each frame
add_subdirectory( bcri_calib )

# my bvis based GUI
add_subdirectory( gui )

# commented by Yong Zhao for Breyes1 moving operation. gui_flip does not exist.
# add_subdirectory( gui_flip )

# commented by Yong Zhao for Breyes1 moving operation. pro does not exist.
# add_subdirectory( pro )

# my command line executables
#add_subdirectory( cmd )


add_subdirectory( dbcri )
add_subdirectory( dbcri_accu )
add_subdirectory( dbcri_accu_edge )
add_subdirectory( dbcri_whole )

# commented by Yong Zhao for Breyes1 moving operation. gui_flip does not exist.
#add_subdirectory( dbcri_whole_flip )

#add_subdirectory(dbsel)
#add_subdirectory(vis)

# commented by Yong Zhao for Breyes1 moving operation. gui_flip does not exist.
#add_subdirectory(algo)

#add_subdirectory(vorl)
add_subdirectory(add_3D)
add_subdirectory(add_3D_same_direction)

# commented by Yong Zhao for Breyes1 moving operation. gui_flip does not exist.
# add_subdirectory(add_3D_reverse_direction)

#add_subdirectory(add_3D_many_direction)

# commented by Yong Zhao for Breyes1 moving operation. gui_flip does not exist.
# add_subdirectory( Z_height )

add_subdirectory(tree_rec)
add_subdirectory(tree_rec_rev)

# commented by Yong Zhao for Breyes1 moving operation. gui_flip does not exist.
#add_subdirectory(CRHM)
