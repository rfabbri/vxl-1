# contrib/CMakeLists.txt
# Brown personal and subgroup development area.


# The starter_kit
if( DART_ROOT )
  option(BUILD_CONTRIB_STARTER_KIT "Build the contrib starter kit" YES)
else( DART_ROOT )
  option(BUILD_CONTRIB_STARTER_KIT "Build the contrib starter kit" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_STARTER_KIT)
    add_subdirectory( starter_kit )
endif(BUILD_CONTRIB_STARTER_KIT)


#########################################
#               Projects                #
#########################################

# Contrib Project: dbrec_lib
if( DART_ROOT )
  option(BUILD_CONTRIB_dbrec_lib "Build contrib/dbrec_lib" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_dbrec_lib "Build contrib/dbrec_lib" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_dbrec_lib)
    add_subdirectory( dbrec_lib )
endif(BUILD_CONTRIB_dbrec_lib)

# Contrib Project: dbvxm
if( DART_ROOT )
  option(BUILD_CONTRIB_dbvxm "Build contrib/dbvxm" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_dbvxm "Build contrib/dbvxm" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_dbvxm)
    add_subdirectory( dbvxm )
endif(BUILD_CONTRIB_dbvxm)

# Contrib Project: dbvxm_batch
if( DART_ROOT )
  option(BUILD_CONTRIB_dbvxm_batch "Build contrib/dbvxm_batch" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_dbvxm_batch "Build contrib/dbvxm_batch" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_dbvxm_batch)
    add_subdirectory( dbvxm_batch)
endif(BUILD_CONTRIB_dbvxm_batch)

# Contrib Project: bvam
if( DART_ROOT )
  option(BUILD_CONTRIB_bvam "Build contrib/bvam" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_bvam "Build contrib/bvam" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_bvam)
    add_subdirectory( bvam )
endif(BUILD_CONTRIB_bvam)

# Contrib Project: bvam_batch
if( DART_ROOT )
  option(BUILD_CONTRIB_bvam_batch "Build contrib/bvam_batch" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_bvam_batch "Build contrib/bvam_batch" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_bvam_batch)
    add_subdirectory( bvam_batch )
endif(BUILD_CONTRIB_bvam_batch)

# Contrib Project: db3dapp
if( DART_ROOT )
  option(BUILD_CONTRIB_db3dapp "Build contrib/db3dapp" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_db3dapp "Build contrib/db3dapp" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_db3dapp)
    add_subdirectory( db3dapp )
endif(BUILD_CONTRIB_db3dapp)

# Contrib Project: Gregistration
if( DART_ROOT )
  option(BUILD_CONTRIB_Gregistration "Build contrib/Gregistration" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_Gregistration "Build contrib/Gregistration" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_Gregistration)
    add_subdirectory( Gregistration )
endif(BUILD_CONTRIB_Gregistration)


# Contrib Project: dbmsh3dapp
if( DART_ROOT )
  option(BUILD_CONTRIB_dbmsh3dapp "Build contrib/dbmsh3dapp" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_dbmsh3dapp "Build contrib/dbmsh3dapp" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_dbmsh3dapp)
    add_subdirectory( dbmsh3dapp )
endif(BUILD_CONTRIB_dbmsh3dapp)


# Contrib Project: dbsk3dapp
if( DART_ROOT )
  option(BUILD_CONTRIB_dbsk3dapp "Build contrib/dbsk3dapp" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_dbsk3dapp "Build contrib/dbsk3dapp" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_dbsk3dapp)
    add_subdirectory( dbsk3dapp )
endif(BUILD_CONTRIB_dbsk3dapp)


# Contrib Project: meshvis
if( DART_ROOT )
  option(BUILD_CONTRIB_meshvis "Build contrib/meshvis" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_meshvis "Build contrib/meshvis" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_meshvis)
    add_subdirectory( meshvis )
endif(BUILD_CONTRIB_meshvis)

# Contrib Project: han
if( DART_ROOT )
  option(BUILD_CONTRIB_han "Build contrib/han" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_han "Build contrib/han" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_han)
    add_subdirectory( han )
endif(BUILD_CONTRIB_han)

# Contrib Project: sadali
if( DART_ROOT )
  option(BUILD_CONTRIB_sadali "Build contrib/sadali" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_sadali "Build contrib/sadali" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_sadali)
    add_subdirectory( sadali )
endif(BUILD_CONTRIB_sadali)

# Contrib Project: int_cedt3d
if( DART_ROOT )
  option(BUILD_CONTRIB_int_cedt3d "Build contrib/int_cedt3d" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_int_cedt3d "Build contrib/int_cedt3d" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_int_cedt3d)
    add_subdirectory( int_cedt3d )
endif(BUILD_CONTRIB_int_cedt3d)

# Contrib Project: ozge
if( DART_ROOT )
  option(BUILD_CONTRIB_ozge "Build contrib/ozge" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_ozge "Build contrib/ozge" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_ozge)
    add_subdirectory( ozge )
endif(BUILD_CONTRIB_ozge)

# Contrib Project: matching_gui
if( DART_ROOT )
  option(BUILD_CONTRIB_matching_gui "Build contrib/matching_gui" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_matching_gui "Build contrib/matching_gui" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_matching_gui)
    add_subdirectory( matching_gui )
endif(BUILD_CONTRIB_matching_gui)

# Contrib Project: fatih
if( DART_ROOT )
  option(BUILD_CONTRIB_fatih "Build contrib/fatih" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_fatih "Build contrib/fatih" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_fatih)
    add_subdirectory( fatih )
endif(BUILD_CONTRIB_fatih)


# Contrib Project: world_model
if( DART_ROOT )
  option(BUILD_CONTRIB_world_model "Build contrib/world_model" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_world_model "Build contrib/world_model" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_world_model)
    add_subdirectory( world_model )
endif(BUILD_CONTRIB_world_model)

# Contrib Project: ozge
if( DART_ROOT )
  option(BUILD_CONTRIB_cluster_mpi "Build contrib/cluster_mpi" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_cluster_mpi "Build contrib/cluster_mpi" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_cluster_mpi)
    add_subdirectory( cluster_mpi )
endif(BUILD_CONTRIB_cluster_mpi)


# Contrib Project: curve2d
if( DART_ROOT )
  option(BUILD_CONTRIB_curve2d "Build contrib/curve2d" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_curve2d "Build contrib/curve2d" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_curve2d)
    add_subdirectory( curve2d )
endif(BUILD_CONTRIB_curve2d)

# Contrib Project: ntrinh
if( DART_ROOT )
  option(BUILD_CONTRIB_ntrinh "Build contrib/ntrinh" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_ntrinh "Build contrib/ntrinh" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_ntrinh)
    add_subdirectory( ntrinh )
endif(BUILD_CONTRIB_ntrinh)

# Contrib Project: biotree
if( DART_ROOT )
  option(BUILD_CONTRIB_biotree "Build contrib/biotree" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_biotree "Build contrib/biotree" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_biotree)
    add_subdirectory( biotree )
endif(BUILD_CONTRIB_biotree)

# Contrib Project: shock2d
if( DART_ROOT )
  option(BUILD_CONTRIB_shock2d "Build contrib/shock2d" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_shock2d "Build contrib/shock2d" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_shock2d)
    add_subdirectory( shock2d )
endif(BUILD_CONTRIB_shock2d)

# Contrib Project: edge_det
if( DART_ROOT )
  option(BUILD_CONTRIB_edge_det "Build contrib/edge_det" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_edge_det "Build contrib/edge_det" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_edge_det)
    add_subdirectory( edge_det )
endif(BUILD_CONTRIB_edge_det)

# Contrib Project: rfabbri
if( DART_ROOT )
  option(BUILD_CONTRIB_rfabbri "Build contrib/rfabbri" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_rfabbri "Build contrib/rfabbri" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_rfabbri)
    add_subdirectory( rfabbri )
endif(BUILD_CONTRIB_rfabbri)

#contrib project: dbctrkapp
if( DART_ROOT )
  option(BUILD_CONTRIB_dbctrkapp "Build contrib/dbctrkapp" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_dbctrkapp "Build contrib/dbctrkapp" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_dbctrkapp)
    add_subdirectory( dbctrkapp)
endif(BUILD_CONTRIB_dbctrkapp)

# Contrib Project: can
if( DART_ROOT )
  option(BUILD_CONTRIB_can "Build contrib/can" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_can "Build contrib/can" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_can)
    add_subdirectory( can )
endif(BUILD_CONTRIB_can)

#contrib project: dbpz_hindsite_2
if( DART_ROOT )
  option(BUILD_CONTRIB_dbpz_hindsite_2 "Build contrib/dbpz_hindsite_2" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_dbpz_hindsite_2 "Build contrib/dbpz_hindsite_2" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_dbpz_hindsite_2)
    add_subdirectory( dbpz_hindsite_2 )
endif(BUILD_CONTRIB_dbpz_hindsite_2)

#contrib project: dbpz_hindsite
if( DART_ROOT )
  option(BUILD_CONTRIB_dbpz_hindsite "Build contrib/dbpz_hindsite" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_dbpz_hindsite "Build contrib/dbpz_hindsite_2" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_dbpz_hindsite)
    add_subdirectory( dbpz_hindsite )
endif(BUILD_CONTRIB_dbpz_hindsite)

# Contrib Project: mleotta
if( DART_ROOT )
  option(BUILD_CONTRIB_mleotta "Build contrib/mleotta" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_mleotta "Build contrib/mleotta" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_mleotta)
    add_subdirectory( mleotta )
endif(BUILD_CONTRIB_mleotta)


# Contrib Project: visualizer
if( DART_ROOT )
  option(BUILD_CONTRIB_visualizer "Build contrib/visualizer" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_visualizer "Build contrib/visualizer" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_visualizer)
    add_subdirectory( visualizer )
endif(BUILD_CONTRIB_visualizer)

## Contrib Project: shannon
#if( DART_ROOT )
#  option(BUILD_CONTRIB_shannon "Build contrib/shannon" NO)
#else( DART_ROOT )
#  option(BUILD_CONTRIB_shannon "Build contrib/shannon" NO)
#endif( DART_ROOT )
#if(BUILD_CONTRIB_shannon)
#    add_subdirectory( shannon )
#endif(BUILD_CONTRIB_shannon)


# Contrib Project: medial_livewire
if( DART_ROOT )
  option(BUILD_CONTRIB_medial_livewire "Build contrib/medial_livewire" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_medial_livewire "Build contrib/medial_livewire" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_medial_livewire)
    add_subdirectory( medial_livewire )
endif(BUILD_CONTRIB_medial_livewire)



# Contrib Project: vehicleseg
if( DART_ROOT )
  option(BUILD_CONTRIB_vehicleseg "Build contrib/vehicleseg" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_vehicleseg "Build contrib/vehicleseg" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_vehicleseg)
    add_subdirectory( vehicleseg)
endif(BUILD_CONTRIB_vehicleseg)



# Contrib Project: itrack
if( DART_ROOT )
  option(BUILD_CONTRIB_itrack "Build contrib/itrack" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_itrack "Build contrib/itrack" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_itrack)
    add_subdirectory( itrack )
endif(BUILD_CONTRIB_itrack)


# Contrib Project: geodesic_mesh
if( DART_ROOT )
  option(BUILD_CONTRIB_geodesic_mesh "Build contrib/geodesic_mesh" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_geodesic_mesh "Build contrib/geodesic_mesh" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_geodesic_mesh)
    add_subdirectory( geodesic_mesh )
endif(BUILD_CONTRIB_geodesic_mesh)


# Contrib Project: polygon_triangulation
if( DART_ROOT )
  option(BUILD_CONTRIB_polygon_triangulation "Build contrib/polygon_triangulation" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_polygon_triangulation "Build contrib/polygon_triangulation" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_polygon_triangulation)
    add_subdirectory( polygon_triangulation )
endif(BUILD_CONTRIB_polygon_triangulation)

# Contrib Project: levelset_seg
if( DART_ROOT )
  option(BUILD_CONTRIB_levelset_seg "Build contrib/levelset_seg" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_levelset_seg "Build contrib/levelset_seg" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_levelset_seg)
    add_subdirectory( levelset_seg )
endif(BUILD_CONTRIB_levelset_seg)

# Contrib Project: dbacm3d_example
if( DART_ROOT )
  option(BUILD_CONTRIB_dbacm3d_example "Build contrib/dbacm3d_example" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_dbacm3d_example "Build contrib/dbacm3d_example" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_dbacm3d_example)
    add_subdirectory( dbacm3d_example )
endif(BUILD_CONTRIB_dbacm3d_example)

# Contrib Project: tpollard
if( DART_ROOT )
  option(BUILD_CONTRIB_tpollard "Build contrib/tpollard" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_tpollard "Build contrib/tpollard" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_tpollard)
    add_subdirectory( tpollard )
endif(BUILD_CONTRIB_tpollard)

# Contrib Project: manifold_extraction
if( DART_ROOT )
  option(BUILD_CONTRIB_manifold_extraction "Build contrib/manifold_extraction" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_manifold_extraction "Build contrib/manifold_extraction" NO)
endif( DART_ROOT )

if(BUILD_CONTRIB_manifold_extraction)
    add_subdirectory( manifold_extraction )
endif(BUILD_CONTRIB_manifold_extraction)

# Contrib Project: knee_cartilage
if( DART_ROOT )
  option(BUILD_CONTRIB_knee_cartilage "Build contrib/knee_cartilage" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_knee_cartilage "Build contrib/knee_cartilage" NO)
endif( DART_ROOT )

# Contrib Project: isa
if( DART_ROOT )
  option(BUILD_CONTRIB_isabel "Build contrib/isabel" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_isabel "Build contrib/isabel" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_isabel)
    add_subdirectory( isabel )
endif(BUILD_CONTRIB_isabel)

if(BUILD_CONTRIB_knee_cartilage)
    add_subdirectory( knee_cartilage )
endif(BUILD_CONTRIB_knee_cartilage)

# Contrib Project: labelcontour
if( DART_ROOT )
  option(BUILD_CONTRIB_labelcontour "Build contrib/labelcontour" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_labelcontour "Build contrib/labelcontour" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_labelcontour)
    add_subdirectory( labelcontour )
endif(BUILD_CONTRIB_labelcontour)

# Contrib Project: amir
if( DART_ROOT )
  option(BUILD_CONTRIB_amir "Build contrib/amir" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_amir "Build contrib/amir" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_amir)
    add_subdirectory( amir )
endif(BUILD_CONTRIB_amir)

#add_subdirectory( top_down_segmentation )


# Contrib Project: yong
if( DART_ROOT )
  option(BUILD_CONTRIB_yong "Build contrib/yong" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_yong "Build contrib/yong" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_yong)
    add_subdirectory( yong )
endif(BUILD_CONTRIB_yong)

# Contrib Project: spatemp
if( DART_ROOT )
  option(BUILD_CONTRIB_spatemp "Build contrib/spatemp" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_spatemp "Build contrib/spatemp" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_spatemp)
    add_subdirectory( spatemp)
endif(BUILD_CONTRIB_spatemp)



# Contrib Project: dec
if( DART_ROOT )
  option(BUILD_CONTRIB_dec "Build contrib/dec" YES)
else( DART_ROOT )
  option(BUILD_CONTRIB_dec "Build contrib/dec" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_dec)
    add_subdirectory( dec )
endif(BUILD_CONTRIB_dec)


# Contrib Project: edubrito
if( DART_ROOT )
  option(BUILD_CONTRIB_edubrito "Build contrib/edubrito" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_edubrito "Build contrib/edubrito" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_edubrito)
    add_subdirectory( edubrito )
endif(BUILD_CONTRIB_edubrito)

# Contrib Project: maruthi
if( DART_ROOT )
  option(BUILD_CONTRIB_maruthi "Build contrib/maruthi" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_maruthi "Build contrib/maruthi" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_maruthi)
    add_subdirectory( maruthi )
endif(BUILD_CONTRIB_maruthi)

# Contrib Project: bwm_lidar
if( DART_ROOT )
  option(BUILD_CONTRIB_bwm_lidar "Build contrib/bwm_lidar" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_bwm_lidar "Build contrib/bwm_lidar" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_bwm_lidar)
    add_subdirectory( bwm_lidar )
endif(BUILD_CONTRIB_bwm_lidar)

# Contrib Project: anil
if( DART_ROOT )
  option(BUILD_CONTRIB_anil "Build contrib/anil" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_anil "Build contrib/anil" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_anil)
    add_subdirectory( anil )
endif(BUILD_CONTRIB_anil)

# Contrib Project: bm
if( DART_ROOT )
  option(BUILD_CONTRIB_bm "Build contrib/bm" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_bm "Build contrib/bm" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_bm)
  add_subdirectory( bm )
endif(BUILD_CONTRIB_bm)

# Contrib Project: gould
if( DART_ROOT )
  option(BUILD_CONTRIB_gould "Build contrib/gould" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_gould "Build contrib/gould" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_gould)
  add_subdirectory( gould )
endif(BUILD_CONTRIB_gould)

# Contrib Project: firat
if( DART_ROOT )
  option(BUILD_CONTRIB_firat "Build contrib/firat" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_firat "Build contrib/firat" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_firat)
    add_subdirectory( firat )
endif(BUILD_CONTRIB_firat)

# Contrib Project: octi
if( DART_ROOT )
  option(BUILD_CONTRIB_octi "Build contrib/octi" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_octi "Build contrib/octi" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_octi)
    add_subdirectory( octi )
endif(BUILD_CONTRIB_octi)

# Contrib Project: Anant
# if( DART_ROOT )
#   option(BUILD_CONTRIB_Anant "Build contrib/Anant" NO)
# else( DART_ROOT )
#   option(BUILD_CONTRIB_Anant "Build contrib/Anant" NO)
# endif( DART_ROOT )
# if(BUILD_CONTRIB_Anant)
#     add_subdirectory( Anant )
# endif(BUILD_CONTRIB_Anant)

# Contrib Project: mauro
if( DART_ROOT )
  option(BUILD_CONTRIB_mauro "Build contrib/mauro" YES)
else( DART_ROOT )
  option(BUILD_CONTRIB_mauro "Build contrib/mauro" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_mauro)
    add_subdirectory( mauro )
endif(BUILD_CONTRIB_mauro)


# Contrib Project: yuliang
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_yuliang "Build contrib/yuliang" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_yuliang "Build contrib/yuliang" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_yuliang)
    SUBDIRS( yuliang )
ENDIF(BUILD_CONTRIB_yuliang)
