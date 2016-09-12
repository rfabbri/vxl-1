# contrib/lems_han/CMakeLists.txt
# Developement for : lems_han




# dll library for recognition of video objects in each frame
subdirs ( bcri_calib )

# my bvis based GUI
subdirs( gui )

# commented by Yong Zhao for Breyes1 moving operation. gui_flip does not exist.
# subdirs( gui_flip )

# commented by Yong Zhao for Breyes1 moving operation. pro does not exist.
# subdirs( pro )

# my command line executables
#subdirs( cmd )


subdirs ( dbcri )
subdirs ( dbcri_accu )
subdirs ( dbcri_accu_edge )
subdirs ( dbcri_whole )

# commented by Yong Zhao for Breyes1 moving operation. gui_flip does not exist.
#subdirs ( dbcri_whole_flip )

#subdirs (dbsel)
#subdirs (vis)

# commented by Yong Zhao for Breyes1 moving operation. gui_flip does not exist.
#subdirs (algo)

#subdirs (vorl)
subdirs (add_3D)
subdirs (add_3D_same_direction)

# commented by Yong Zhao for Breyes1 moving operation. gui_flip does not exist.
# subdirs (add_3D_reverse_direction)

#subdirs (add_3D_many_direction)

# commented by Yong Zhao for Breyes1 moving operation. gui_flip does not exist.
# subdirs( Z_height )

subdirs (tree_rec)
subdirs (tree_rec_rev)

# commented by Yong Zhao for Breyes1 moving operation. gui_flip does not exist.
#subdirs (CRHM)
