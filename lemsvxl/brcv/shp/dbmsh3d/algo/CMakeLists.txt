# This is brcv/shp/dbmsh3d/algo/CMakeLists.txt
# Brown 3D Mesh Algorithm Library

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_RPL_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories(${VXD_B3PD_INCLUDE_DIR})


set( dbmsh3d_algo_sources

  dbmsh3d_fileio.cxx                dbmsh3d_fileio.h
  dbmsh3d_fileio_ply.cxx
  dbmsh3d_fileio_off.cxx
  dbmsh3d_fileio_obj.cxx
  dbmsh3d_fileio_wrl_iv.cxx
  dbmsh3d_fileio_nuages.cxx        
  dbmsh3d_iv_process_fileio.cxx

  dbmsh3d_sg3pi_algo.cxx            dbmsh3d_sg3pi_algo.h

  dbmsh3d_create_box.cxx
  dbmsh3d_create_paragutter.cxx

  dbmsh3d_pt_normal.cxx             dbmsh3d_pt_normal.h
  dbmsh3d_face_algo.cxx             dbmsh3d_face_algo.h
  dbmsh3d_sheet_algo.cxx            dbmsh3d_sheet_algo.h

  dbmsh3d_pt_set_bucket.cxx         dbmsh3d_pt_set_bucket.h

  dbmsh3d_mesh_bnd.cxx              dbmsh3d_mesh_bnd.h

  dbmsh3d_fmm.cxx                   dbmsh3d_fmm.h

  dbmsh3d_mesh_triangulate.cxx      dbmsh3d_mesh_triangulate.h
  dbmsh3d_ray_intersect.cxx         dbmsh3d_ray_intersect.h  
  dbmsh3d_pt_mesh_dist.cxx          dbmsh3d_pt_mesh_dist.h  

  dbmsh3d_xform.cxx                 dbmsh3d_xform.h    
  dbmsh3d_pt_set_correspondence.cxx dbmsh3d_pt_set_correspondence.h 
  
  dbmsh3d_mesh_algos.cxx            dbmsh3d_mesh_algos.h

  dbmsh3d_graph_sh_path.cxx         dbmsh3d_graph_sh_path.h
  
  dbmsh3d_bucketing.cxx             dbmsh3d_bucketing.h   dbmsh3d_bucketing_sptr.h
  
  dbmsh3d_scan_sim.cxx              dbmsh3d_scan_sim.h  
)


aux_source_directory(Templates dbmsh3d_algo_sources )

add_library( dbmsh3d_algo  ${dbmsh3d_algo_sources})

target_link_libraries( dbmsh3d_algo dbmsh3d rply mesh_tri ray_tri rsdl rgrl bgld_algo bnld bsol vgl_algo )

add_subdirectory(tests)


