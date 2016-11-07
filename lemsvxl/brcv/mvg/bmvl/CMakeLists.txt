# brl/bmvl/CMakeLists.txt
# main package for multi-view related code, .e.g. 3-d reconstruction
# All libraries are mutually independent, except for bmvv <-- btom 
# Also, bcal and btom are independent of the rest of brl; brct depends on bbas and vsrl.

# camera calibration
add_subdirectory(bcal)


# 3-d reconstruction
add_subdirectory(brct)

# 3-d tomography
add_subdirectory(btom)


