# contrib/CMakeLists.txt
# Brown personal and subgroup development area.


# The starter_kit
if( DART_ROOT )
  option(BUILD_CONTRIB_STARTER_KIT "Build the contrib starter kit" YES)
else( DART_ROOT )
  option(BUILD_CONTRIB_STARTER_KIT "Build the contrib starter kit" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_STARTER_KIT)
    subdirs( starter_kit )
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
    subdirs( dbrec_lib )
endif(BUILD_CONTRIB_dbrec_lib)

# Contrib Project: dbvxm
if( DART_ROOT )
  option(BUILD_CONTRIB_dbvxm "Build contrib/dbvxm" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_dbvxm "Build contrib/dbvxm" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_dbvxm)
    subdirs( dbvxm )
endif(BUILD_CONTRIB_dbvxm)

# Contrib Project: dbvxm_batch
if( DART_ROOT )
  option(BUILD_CONTRIB_dbvxm_batch "Build contrib/dbvxm_batch" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_dbvxm_batch "Build contrib/dbvxm_batch" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_dbvxm_batch)
    subdirs( dbvxm_batch)
endif(BUILD_CONTRIB_dbvxm_batch)

# Contrib Project: bvam
if( DART_ROOT )
  option(BUILD_CONTRIB_bvam "Build contrib/bvam" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_bvam "Build contrib/bvam" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_bvam)
    subdirs( bvam )
endif(BUILD_CONTRIB_bvam)

# Contrib Project: bvam_batch
if( DART_ROOT )
  option(BUILD_CONTRIB_bvam_batch "Build contrib/bvam_batch" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_bvam_batch "Build contrib/bvam_batch" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_bvam_batch)
    subdirs( bvam_batch )
endif(BUILD_CONTRIB_bvam_batch)

# Contrib Project: db3dapp
if( DART_ROOT )
  option(BUILD_CONTRIB_db3dapp "Build contrib/db3dapp" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_db3dapp "Build contrib/db3dapp" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_db3dapp)
    subdirs( db3dapp )
endif(BUILD_CONTRIB_db3dapp)

# Contrib Project: Gregistration
if( DART_ROOT )
  option(BUILD_CONTRIB_Gregistration "Build contrib/Gregistration" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_Gregistration "Build contrib/Gregistration" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_Gregistration)
    subdirs( Gregistration )
endif(BUILD_CONTRIB_Gregistration)


# Contrib Project: dbmsh3dapp
if( DART_ROOT )
  option(BUILD_CONTRIB_dbmsh3dapp "Build contrib/dbmsh3dapp" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_dbmsh3dapp "Build contrib/dbmsh3dapp" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_dbmsh3dapp)
    subdirs( dbmsh3dapp )
endif(BUILD_CONTRIB_dbmsh3dapp)


# Contrib Project: dbsk3dapp
if( DART_ROOT )
  option(BUILD_CONTRIB_dbsk3dapp "Build contrib/dbsk3dapp" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_dbsk3dapp "Build contrib/dbsk3dapp" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_dbsk3dapp)
    subdirs( dbsk3dapp )
endif(BUILD_CONTRIB_dbsk3dapp)


# Contrib Project: meshvis
if( DART_ROOT )
  option(BUILD_CONTRIB_meshvis "Build contrib/meshvis" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_meshvis "Build contrib/meshvis" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_meshvis)
    subdirs( meshvis )
endif(BUILD_CONTRIB_meshvis)

# Contrib Project: han
if( DART_ROOT )
  option(BUILD_CONTRIB_han "Build contrib/han" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_han "Build contrib/han" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_han)
    subdirs( han )
endif(BUILD_CONTRIB_han)

# Contrib Project: sadali
if( DART_ROOT )
  option(BUILD_CONTRIB_sadali "Build contrib/sadali" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_sadali "Build contrib/sadali" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_sadali)
    subdirs( sadali )
endif(BUILD_CONTRIB_sadali)

# Contrib Project: int_cedt3d
if( DART_ROOT )
  option(BUILD_CONTRIB_int_cedt3d "Build contrib/int_cedt3d" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_int_cedt3d "Build contrib/int_cedt3d" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_int_cedt3d)
    subdirs( int_cedt3d )
endif(BUILD_CONTRIB_int_cedt3d)

# Contrib Project: ozge
if( DART_ROOT )
  option(BUILD_CONTRIB_ozge "Build contrib/ozge" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_ozge "Build contrib/ozge" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_ozge)
    subdirs( ozge )
endif(BUILD_CONTRIB_ozge)

# Contrib Project: matching_gui
if( DART_ROOT )
  option(BUILD_CONTRIB_matching_gui "Build contrib/matching_gui" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_matching_gui "Build contrib/matching_gui" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_matching_gui)
    subdirs( matching_gui )
endif(BUILD_CONTRIB_matching_gui)

# Contrib Project: fatih
if( DART_ROOT )
  option(BUILD_CONTRIB_fatih "Build contrib/fatih" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_fatih "Build contrib/fatih" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_fatih)
    subdirs( fatih )
endif(BUILD_CONTRIB_fatih)


# Contrib Project: world_model
if( DART_ROOT )
  option(BUILD_CONTRIB_world_model "Build contrib/world_model" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_world_model "Build contrib/world_model" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_world_model)
    subdirs( world_model )
endif(BUILD_CONTRIB_world_model)

# Contrib Project: ozge
if( DART_ROOT )
  option(BUILD_CONTRIB_cluster_mpi "Build contrib/cluster_mpi" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_cluster_mpi "Build contrib/cluster_mpi" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_cluster_mpi)
    subdirs( cluster_mpi )
endif(BUILD_CONTRIB_cluster_mpi)


# Contrib Project: curve2d
if( DART_ROOT )
  option(BUILD_CONTRIB_curve2d "Build contrib/curve2d" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_curve2d "Build contrib/curve2d" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_curve2d)
    subdirs( curve2d )
endif(BUILD_CONTRIB_curve2d)

# Contrib Project: ntrinh
if( DART_ROOT )
  option(BUILD_CONTRIB_ntrinh "Build contrib/ntrinh" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_ntrinh "Build contrib/ntrinh" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_ntrinh)
    subdirs( ntrinh )
endif(BUILD_CONTRIB_ntrinh)

# Contrib Project: biotree
if( DART_ROOT )
  option(BUILD_CONTRIB_biotree "Build contrib/biotree" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_biotree "Build contrib/biotree" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_biotree)
    subdirs( biotree )
endif(BUILD_CONTRIB_biotree)

# Contrib Project: shock2d
if( DART_ROOT )
  option(BUILD_CONTRIB_shock2d "Build contrib/shock2d" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_shock2d "Build contrib/shock2d" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_shock2d)
    subdirs( shock2d )
endif(BUILD_CONTRIB_shock2d)

# Contrib Project: edge_det
if( DART_ROOT )
  option(BUILD_CONTRIB_edge_det "Build contrib/edge_det" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_edge_det "Build contrib/edge_det" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_edge_det)
    subdirs( edge_det )
endif(BUILD_CONTRIB_edge_det)

# Contrib Project: rfabbri
if( DART_ROOT )
  option(BUILD_CONTRIB_rfabbri "Build contrib/rfabbri" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_rfabbri "Build contrib/rfabbri" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_rfabbri)
    subdirs( rfabbri )
endif(BUILD_CONTRIB_rfabbri)

#contrib project: dbctrkapp
if( DART_ROOT )
  option(BUILD_CONTRIB_dbctrkapp "Build contrib/dbctrkapp" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_dbctrkapp "Build contrib/dbctrkapp" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_dbctrkapp)
    subdirs( dbctrkapp)
endif(BUILD_CONTRIB_dbctrkapp)

# Contrib Project: can
if( DART_ROOT )
  option(BUILD_CONTRIB_can "Build contrib/can" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_can "Build contrib/can" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_can)
    subdirs( can )
endif(BUILD_CONTRIB_can)

#contrib project: dbpz_hindsite_2
if( DART_ROOT )
  option(BUILD_CONTRIB_dbpz_hindsite_2 "Build contrib/dbpz_hindsite_2" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_dbpz_hindsite_2 "Build contrib/dbpz_hindsite_2" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_dbpz_hindsite_2)
    subdirs( dbpz_hindsite_2 )
endif(BUILD_CONTRIB_dbpz_hindsite_2)

#contrib project: dbpz_hindsite
if( DART_ROOT )
  option(BUILD_CONTRIB_dbpz_hindsite "Build contrib/dbpz_hindsite" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_dbpz_hindsite "Build contrib/dbpz_hindsite_2" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_dbpz_hindsite)
    subdirs( dbpz_hindsite )
endif(BUILD_CONTRIB_dbpz_hindsite)

# Contrib Project: mleotta
if( DART_ROOT )
  option(BUILD_CONTRIB_mleotta "Build contrib/mleotta" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_mleotta "Build contrib/mleotta" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_mleotta)
    subdirs( mleotta )
endif(BUILD_CONTRIB_mleotta)


# Contrib Project: visualizer
if( DART_ROOT )
  option(BUILD_CONTRIB_visualizer "Build contrib/visualizer" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_visualizer "Build contrib/visualizer" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_visualizer)
    subdirs( visualizer )
endif(BUILD_CONTRIB_visualizer)

## Contrib Project: shannon
#if( DART_ROOT )
#  option(BUILD_CONTRIB_shannon "Build contrib/shannon" NO)
#else( DART_ROOT )
#  option(BUILD_CONTRIB_shannon "Build contrib/shannon" NO)
#endif( DART_ROOT )
#if(BUILD_CONTRIB_shannon)
#    subdirs( shannon )
#endif(BUILD_CONTRIB_shannon)


# Contrib Project: medial_livewire
if( DART_ROOT )
  option(BUILD_CONTRIB_medial_livewire "Build contrib/medial_livewire" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_medial_livewire "Build contrib/medial_livewire" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_medial_livewire)
    subdirs( medial_livewire )
endif(BUILD_CONTRIB_medial_livewire)



# Contrib Project: vehicleseg
if( DART_ROOT )
  option(BUILD_CONTRIB_vehicleseg "Build contrib/vehicleseg" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_vehicleseg "Build contrib/vehicleseg" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_vehicleseg)
    subdirs( vehicleseg)
endif(BUILD_CONTRIB_vehicleseg)



# Contrib Project: itrack
if( DART_ROOT )
  option(BUILD_CONTRIB_itrack "Build contrib/itrack" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_itrack "Build contrib/itrack" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_itrack)
    subdirs( itrack )
endif(BUILD_CONTRIB_itrack)


# Contrib Project: geodesic_mesh
if( DART_ROOT )
  option(BUILD_CONTRIB_geodesic_mesh "Build contrib/geodesic_mesh" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_geodesic_mesh "Build contrib/geodesic_mesh" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_geodesic_mesh)
    subdirs( geodesic_mesh )
endif(BUILD_CONTRIB_geodesic_mesh)


# Contrib Project: polygon_triangulation
if( DART_ROOT )
  option(BUILD_CONTRIB_polygon_triangulation "Build contrib/polygon_triangulation" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_polygon_triangulation "Build contrib/polygon_triangulation" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_polygon_triangulation)
    subdirs( polygon_triangulation )
endif(BUILD_CONTRIB_polygon_triangulation)

# Contrib Project: levelset_seg
if( DART_ROOT )
  option(BUILD_CONTRIB_levelset_seg "Build contrib/levelset_seg" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_levelset_seg "Build contrib/levelset_seg" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_levelset_seg)
    subdirs( levelset_seg )
endif(BUILD_CONTRIB_levelset_seg)

# Contrib Project: dbacm3d_example
if( DART_ROOT )
  option(BUILD_CONTRIB_dbacm3d_example "Build contrib/dbacm3d_example" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_dbacm3d_example "Build contrib/dbacm3d_example" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_dbacm3d_example)
    subdirs( dbacm3d_example )
endif(BUILD_CONTRIB_dbacm3d_example)

# Contrib Project: tpollard
if( DART_ROOT )
  option(BUILD_CONTRIB_tpollard "Build contrib/tpollard" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_tpollard "Build contrib/tpollard" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_tpollard)
    subdirs( tpollard )
endif(BUILD_CONTRIB_tpollard)

# Contrib Project: manifold_extraction
if( DART_ROOT )
  option(BUILD_CONTRIB_manifold_extraction "Build contrib/manifold_extraction" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_manifold_extraction "Build contrib/manifold_extraction" NO)
endif( DART_ROOT )

if(BUILD_CONTRIB_manifold_extraction)
    subdirs( manifold_extraction )
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
    subdirs( isabel )
endif(BUILD_CONTRIB_isabel)

if(BUILD_CONTRIB_knee_cartilage)
    subdirs( knee_cartilage )
endif(BUILD_CONTRIB_knee_cartilage)

# Contrib Project: labelcontour
if( DART_ROOT )
  option(BUILD_CONTRIB_labelcontour "Build contrib/labelcontour" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_labelcontour "Build contrib/labelcontour" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_labelcontour)
    subdirs( labelcontour )
endif(BUILD_CONTRIB_labelcontour)

# Contrib Project: amir
if( DART_ROOT )
  option(BUILD_CONTRIB_amir "Build contrib/amir" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_amir "Build contrib/amir" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_amir)
    subdirs( amir )
endif(BUILD_CONTRIB_amir)

#subdirs( top_down_segmentation )


# Contrib Project: yong
if( DART_ROOT )
  option(BUILD_CONTRIB_yong "Build contrib/yong" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_yong "Build contrib/yong" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_yong)
    subdirs( yong )
endif(BUILD_CONTRIB_yong)

# Contrib Project: spatemp
if( DART_ROOT )
  option(BUILD_CONTRIB_spatemp "Build contrib/spatemp" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_spatemp "Build contrib/spatemp" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_spatemp)
    subdirs( spatemp)
endif(BUILD_CONTRIB_spatemp)



# Contrib Project: dec
if( DART_ROOT )
  option(BUILD_CONTRIB_dec "Build contrib/dec" YES)
else( DART_ROOT )
  option(BUILD_CONTRIB_dec "Build contrib/dec" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_dec)
    subdirs( dec )
endif(BUILD_CONTRIB_dec)


# Contrib Project: edubrito
if( DART_ROOT )
  option(BUILD_CONTRIB_edubrito "Build contrib/edubrito" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_edubrito "Build contrib/edubrito" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_edubrito)
    subdirs( edubrito )
endif(BUILD_CONTRIB_edubrito)

# Contrib Project: maruthi
if( DART_ROOT )
  option(BUILD_CONTRIB_maruthi "Build contrib/maruthi" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_maruthi "Build contrib/maruthi" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_maruthi)
    subdirs( maruthi )
endif(BUILD_CONTRIB_maruthi)

# Contrib Project: bwm_lidar
if( DART_ROOT )
  option(BUILD_CONTRIB_bwm_lidar "Build contrib/bwm_lidar" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_bwm_lidar "Build contrib/bwm_lidar" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_bwm_lidar)
    subdirs( bwm_lidar )
endif(BUILD_CONTRIB_bwm_lidar)

# Contrib Project: anil
if( DART_ROOT )
  option(BUILD_CONTRIB_anil "Build contrib/anil" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_anil "Build contrib/anil" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_anil)
    subdirs( anil )
endif(BUILD_CONTRIB_anil)

# Contrib Project: bm
if( DART_ROOT )
  option(BUILD_CONTRIB_bm "Build contrib/bm" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_bm "Build contrib/bm" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_bm)
  subdirs( bm )
endif(BUILD_CONTRIB_bm)

# Contrib Project: gould
if( DART_ROOT )
  option(BUILD_CONTRIB_gould "Build contrib/gould" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_gould "Build contrib/gould" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_gould)
  subdirs( gould )
endif(BUILD_CONTRIB_gould)

# Contrib Project: firat
if( DART_ROOT )
  option(BUILD_CONTRIB_firat "Build contrib/firat" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_firat "Build contrib/firat" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_firat)
    subdirs( firat )
endif(BUILD_CONTRIB_firat)

# Contrib Project: octi
if( DART_ROOT )
  option(BUILD_CONTRIB_octi "Build contrib/octi" NO)
else( DART_ROOT )
  option(BUILD_CONTRIB_octi "Build contrib/octi" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_octi)
    subdirs( octi )
endif(BUILD_CONTRIB_octi)

# Contrib Project: Anant
# if( DART_ROOT )
#   option(BUILD_CONTRIB_Anant "Build contrib/Anant" NO)
# else( DART_ROOT )
#   option(BUILD_CONTRIB_Anant "Build contrib/Anant" NO)
# endif( DART_ROOT )
# if(BUILD_CONTRIB_Anant)
#     subdirs( Anant )
# endif(BUILD_CONTRIB_Anant)

# Contrib Project: mauro
if( DART_ROOT )
  option(BUILD_CONTRIB_mauro "Build contrib/mauro" YES)
else( DART_ROOT )
  option(BUILD_CONTRIB_mauro "Build contrib/mauro" NO)
endif( DART_ROOT )
if(BUILD_CONTRIB_mauro)
    subdirs( mauro )
endif(BUILD_CONTRIB_mauro)

