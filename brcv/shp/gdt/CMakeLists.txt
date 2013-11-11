# This is brcv/shp/dbmsh3d/algo/CMakeLists.txt
# Brown 3D Mesh Algorithm Library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/shp)

SET(gdt_sources

                                     gdt_numerical.h

   gdt_interval.cxx                  gdt_interval.h
   gdt_welm.cxx                      gdt_welm.h
   gdt_interval_lsrc.cxx

   gdt_interval_section.cxx          gdt_interval_section.h

   gdt_shock.cxx                     gdt_shock.h
   gdt_shock_edge.cxx

   gdt_shock_section.cxx             gdt_shock_section.h

   gdt_vertex.cxx                    gdt_vertex.h
   gdt_edge.cxx                      gdt_edge.h
   gdt_mesh.cxx                      gdt_mesh.h

   gdt_path.cxx                      gdt_path.h

   gdt_solve_intersect.cxx           gdt_solve_intersect.h
   gdt_solve_shock_intersect.cxx     gdt_solve_shock_intersect.h

   gdt_manager.cxx                   gdt_manager.h
   gdt_prop_funs.cxx

   gdt_manager_i_based.cxx
   gdt_i_init_prop.cxx

   gdt_f_base.cxx
   gdt_f_init.cxx
   gdt_f_prop.cxx
   gdt_f_flow.cxx
   gdt_f_prop_funs.cxx
   gdt_f_retract_prop.cxx

   gdt_fs_init.cxx
   gdt_fs_prop.cxx
   gdt_fs_flow.cxx
   gdt_fs_retract_prop.cxx
   gdt_fs_shock_prop.cxx             gdt_fs_shock_prop.h

   gdt_ws_init.cxx
   gdt_ws_algo.cxx
   gdt_ws_algo_funs.cxx

   gdt_ws_prop_wa_2ndsrc.cxx
   gdt_ws_prop_wa_edge.cxx
   gdt_ws_prop_shk.cxx

   gdt_ws_adv_wa_2ndsrc.cxx
   gdt_ws_adv_wa_edge.cxx
   gdt_ws_adv_wa_vertex_bnd.cxx
   gdt_ws_adv_wa_vertex_interior.cxx
   gdt_ws_adv_shk_edge.cxx
   gdt_ws_adv_shk_junction.cxx
   gdt_ws_adv_shks_edge.cxx

)


ADD_LIBRARY( gdt  ${gdt_sources})

TARGET_LINK_LIBRARIES( gdt dbmsh3d )

SUBDIRS(pro)

# Visualization Library
SUBDIRS(vis)

SUBDIRS(cmdproc)


