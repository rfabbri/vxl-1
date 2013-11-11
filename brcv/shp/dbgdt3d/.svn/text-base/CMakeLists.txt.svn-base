# This is brcv/shp/dbgdt3d/algo/CMakeLists.txt
# Debug Brown Geodesic Distance Transform 3D Library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/shp)

SET(dbgdt3d_sources

                                     dbgdt3d_numerical.h

   dbgdt3d_interval.cxx                  dbgdt3d_interval.h
   dbgdt3d_welm.cxx                      dbgdt3d_welm.h
   dbgdt3d_interval_lsrc.cxx

   dbgdt3d_interval_section.cxx          dbgdt3d_interval_section.h

   dbgdt3d_shock.cxx                     dbgdt3d_shock.h
   dbgdt3d_shock_edge.cxx

   dbgdt3d_shock_section.cxx             dbgdt3d_shock_section.h

   dbgdt3d_vertex.cxx                    dbgdt3d_vertex.h
   dbgdt3d_edge.cxx                      dbgdt3d_edge.h
   dbgdt3d_mesh.cxx                      dbgdt3d_mesh.h

   dbgdt3d_path.cxx                      dbgdt3d_path.h

   dbgdt3d_solve_intersect.cxx           dbgdt3d_solve_intersect.h
   dbgdt3d_solve_shock_intersect.cxx     dbgdt3d_solve_shock_intersect.h

   dbgdt3d_manager.cxx                   dbgdt3d_manager.h
   dbgdt3d_prop_funs.cxx

   dbgdt3d_manager_i_based.cxx
   dbgdt3d_i_init_prop.cxx

   dbgdt3d_f_base.cxx
   dbgdt3d_f_init.cxx
   dbgdt3d_f_prop.cxx
   dbgdt3d_f_flow.cxx
   dbgdt3d_f_prop_funs.cxx
   dbgdt3d_f_retract_prop.cxx

   dbgdt3d_fs_init.cxx
   dbgdt3d_fs_prop.cxx
   dbgdt3d_fs_flow.cxx
   dbgdt3d_fs_retract_prop.cxx
   dbgdt3d_fs_shock_prop.cxx             dbgdt3d_fs_shock_prop.h

   dbgdt3d_ws_init.cxx
   dbgdt3d_ws_algo.cxx
   dbgdt3d_ws_algo_funs.cxx

   dbgdt3d_ws_prop_wa_2ndsrc.cxx
   dbgdt3d_ws_prop_wa_edge.cxx
   dbgdt3d_ws_prop_shk.cxx

   dbgdt3d_ws_adv_wa_2ndsrc.cxx
   dbgdt3d_ws_adv_wa_edge.cxx
   dbgdt3d_ws_adv_wa_vertex_bnd.cxx
   dbgdt3d_ws_adv_wa_vertex_interior.cxx
   dbgdt3d_ws_adv_shk_edge.cxx
   dbgdt3d_ws_adv_shk_junction.cxx
   dbgdt3d_ws_adv_shks_edge.cxx

)


ADD_LIBRARY( dbgdt3d  ${dbgdt3d_sources})

TARGET_LINK_LIBRARIES( dbgdt3d dbmsh3d )

SUBDIRS(pro)

# Visualization Library
SUBDIRS(vis)

SUBDIRS(cmdproc)


