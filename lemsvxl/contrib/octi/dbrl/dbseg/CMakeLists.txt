# dbrl/dbseg/CMakeLists.txt
# main package for segmentation-related code and applications
# Mutual dependencies: brip --> sdet --> (vpro,segv)

# float image operations
#add_subdirectory(brip)

# segmentation feature detection library
#add_subdirectory(sdet)

# video processors
#add_subdirectory(vpro)

# tracking
#add_subdirectory(strk)

# Markov Random Fields - This library is deprecated
#add_subdirectory(bmrf)

# misc
#add_subdirectory(bcons)

# segmentation view library
#add_subdirectory(segv)

# segmentation application executables
#add_subdirectory(sbin)

# background modeling
add_subdirectory(dbbgm)

# background modeling python interface
add_subdirectory(dbbgm_batch)
add_subdirectory(bbkmdl)
#voxel world modeling
#add_subdirectory(bvxm)

#voxel world modeling python interface
#add_subdirectory(bvxm_batch)

# Lidar modeling (segmentation and meshing)
#add_subdirectory(bmdl)

# Lidar modeling python interface
#add_subdirectory(bmdl_batch)

# recognition library - uses background models of bbgm and/or bvxm
#add_subdirectory(brec)

# recognition library - python interface
#add_subdirectory(brec_batch)

# octree
#add_subdirectory(boct)

#octree based modeling 
add_subdirectory(dboxm)

#octree based modeling python interface
#add_subdirectory(boxm_batch)

#voxel processing library
#add_subdirectory(bvpl)

#voxel processing library python interface
#add_subdirectory(bvpl_batch)