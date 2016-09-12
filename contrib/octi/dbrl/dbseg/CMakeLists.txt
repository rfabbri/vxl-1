# dbrl/dbseg/CMakeLists.txt
# main package for segmentation-related code and applications
# Mutual dependencies: brip --> sdet --> (vpro,segv)

# float image operations
#subdirs(brip)

# segmentation feature detection library
#subdirs(sdet)

# video processors
#subdirs(vpro)

# tracking
#subdirs(strk)

# Markov Random Fields - This library is deprecated
#subdirs(bmrf)

# misc
#subdirs(bcons)

# segmentation view library
#subdirs(segv)

# segmentation application executables
#subdirs(sbin)

# background modeling
subdirs(dbbgm)

# background modeling python interface
subdirs(dbbgm_batch)
subdirs(bbkmdl)
#voxel world modeling
#subdirs(bvxm)

#voxel world modeling python interface
#subdirs(bvxm_batch)

# Lidar modeling (segmentation and meshing)
#subdirs(bmdl)

# Lidar modeling python interface
#subdirs(bmdl_batch)

# recognition library - uses background models of bbgm and/or bvxm
#subdirs(brec)

# recognition library - python interface
#subdirs(brec_batch)

# octree
#subdirs(boct)

#octree based modeling 
subdirs(dboxm)

#octree based modeling python interface
#subdirs(boxm_batch)

#voxel processing library
#subdirs(bvpl)

#voxel processing library python interface
#subdirs(bvpl_batch)