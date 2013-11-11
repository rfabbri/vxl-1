# This is brcv/shp/dbsk3d/algo/CMakeLists.txt
# Brown Shock3D Algorithm Library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/shp)
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR} )

SET( dbsk3d_algo_sources

dbsk3d_mesh_bnd.cxx                 dbsk3d_mesh_bnd.h

dbsk3d_fs_algo.cxx                  dbsk3d_fs_algo.h
dbsk3d_fs_fileio.cxx                dbsk3d_fs_fileio.h
dbsk3d_fs_recovery_qhull.cxx        

dbsk3d_fs_xform_gap.cxx             dbsk3d_fs_xform.h
dbsk3d_fs_xform_splice.cxx

dbsk3d_fs_segre.cxx                 dbsk3d_fs_segre.h
dbsk3d_fs_segre_funs.cxx
dbsk3d_fs_segre_bkt_stitch.cxx      dbsk3d_fs_segre_bkt_stitch.h
dbsk3d_fs_inflow_prop.cxx           dbsk3d_fs_inflow_prop.h
dbsk3d_fs_regul.cxx                 dbsk3d_fs_regul.h

                      
dbsk3d_ms_hypg_build.cxx            dbsk3d_ms_hypg_build.h 
dbsk3d_ms_algos.cxx                 dbsk3d_ms_algos.h  
dbsk3d_ms_hypg_fileio.cxx           dbsk3d_ms_fileio.h 
dbsk3d_ms_recon.cxx                 dbsk3d_ms_recon.h

dbsk3d_ms_hypg_trans.cxx            dbsk3d_ms_hypg_trans.h
dbsk3d_ms_hypg_fmm.cxx              dbsk3d_ms_hypg_fmm.h

dbsk3d_ms_xform.cxx                 dbsk3d_ms_xform.h
dbsk3d_ms_xform_hybrid.cxx
dbsk3d_ms_xform_splice.cxx
dbsk3d_ms_xform_contract_a5.cxx
dbsk3d_ms_xform_contract_a12a3i.cxx
dbsk3d_ms_xform_contract_a15_c.cxx
dbsk3d_ms_xform_contract_a15_s.cxx
dbsk3d_ms_xform_contract_a14_s.cxx
dbsk3d_ms_xform_merge_a1a3ii_n_n.cxx
dbsk3d_ms_xform_merge_a12a3i_n_c.cxx
dbsk3d_ms_xform_merge_a14_c_c.cxx
dbsk3d_ms_xform_merge_a1a3ii_c_c.cxx
dbsk3d_ms_xform_merge_a1a5_n_c.cxx

dbsk3d_ms_hypg_vl.cxx

dbsk3d_sg_sa_algos.cxx              dbsk3d_sg_sa_algos.h
dbsk3d_sg_sa_trans.cxx
dbsk3d_sg_sa_fileio.cxx
dbsk3d_sg_sa_smooth.cxx

)


ADD_LIBRARY(dbsk3d_algo ${dbsk3d_algo_sources})

TARGET_LINK_LIBRARIES(dbsk3d_algo dbsk3d dbmsh3d_algo)

