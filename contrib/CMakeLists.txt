# contrib/CMakeLists.txt
# Brown personal and subgroup development area.


# The starter_kit
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_STARTER_KIT "Build the contrib starter kit" YES)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_STARTER_KIT "Build the contrib starter kit" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_STARTER_KIT)
    SUBDIRS( starter_kit )
ENDIF(BUILD_CONTRIB_STARTER_KIT)


#########################################
#               Projects                #
#########################################

# Contrib Project: dbrec_lib
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_dbrec_lib "Build contrib/dbrec_lib" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_dbrec_lib "Build contrib/dbrec_lib" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_dbrec_lib)
    SUBDIRS( dbrec_lib )
ENDIF(BUILD_CONTRIB_dbrec_lib)

# Contrib Project: dbvxm
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_dbvxm "Build contrib/dbvxm" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_dbvxm "Build contrib/dbvxm" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_dbvxm)
    SUBDIRS( dbvxm )
ENDIF(BUILD_CONTRIB_dbvxm)

# Contrib Project: dbvxm_batch
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_dbvxm_batch "Build contrib/dbvxm_batch" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_dbvxm_batch "Build contrib/dbvxm_batch" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_dbvxm_batch)
    SUBDIRS( dbvxm_batch)
ENDIF(BUILD_CONTRIB_dbvxm_batch)

# Contrib Project: bvam
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_bvam "Build contrib/bvam" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_bvam "Build contrib/bvam" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_bvam)
    SUBDIRS( bvam )
ENDIF(BUILD_CONTRIB_bvam)

# Contrib Project: bvam_batch
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_bvam_batch "Build contrib/bvam_batch" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_bvam_batch "Build contrib/bvam_batch" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_bvam_batch)
    SUBDIRS( bvam_batch )
ENDIF(BUILD_CONTRIB_bvam_batch)

# Contrib Project: db3dapp
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_db3dapp "Build contrib/db3dapp" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_db3dapp "Build contrib/db3dapp" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_db3dapp)
    SUBDIRS( db3dapp )
ENDIF(BUILD_CONTRIB_db3dapp)

# Contrib Project: Gregistration
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_Gregistration "Build contrib/Gregistration" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_Gregistration "Build contrib/Gregistration" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_Gregistration)
    SUBDIRS( Gregistration )
ENDIF(BUILD_CONTRIB_Gregistration)


# Contrib Project: dbmsh3dapp
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_dbmsh3dapp "Build contrib/dbmsh3dapp" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_dbmsh3dapp "Build contrib/dbmsh3dapp" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_dbmsh3dapp)
    SUBDIRS( dbmsh3dapp )
ENDIF(BUILD_CONTRIB_dbmsh3dapp)


# Contrib Project: dbsk3dapp
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_dbsk3dapp "Build contrib/dbsk3dapp" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_dbsk3dapp "Build contrib/dbsk3dapp" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_dbsk3dapp)
    SUBDIRS( dbsk3dapp )
ENDIF(BUILD_CONTRIB_dbsk3dapp)


# Contrib Project: meshvis
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_meshvis "Build contrib/meshvis" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_meshvis "Build contrib/meshvis" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_meshvis)
    SUBDIRS( meshvis )
ENDIF(BUILD_CONTRIB_meshvis)

# Contrib Project: han
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_han "Build contrib/han" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_han "Build contrib/han" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_han)
    SUBDIRS( han )
ENDIF(BUILD_CONTRIB_han)

# Contrib Project: sadali
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_sadali "Build contrib/sadali" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_sadali "Build contrib/sadali" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_sadali)
    SUBDIRS( sadali )
ENDIF(BUILD_CONTRIB_sadali)

# Contrib Project: int_cedt3d
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_int_cedt3d "Build contrib/int_cedt3d" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_int_cedt3d "Build contrib/int_cedt3d" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_int_cedt3d)
    SUBDIRS( int_cedt3d )
ENDIF(BUILD_CONTRIB_int_cedt3d)

# Contrib Project: ozge
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_ozge "Build contrib/ozge" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_ozge "Build contrib/ozge" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_ozge)
    SUBDIRS( ozge )
ENDIF(BUILD_CONTRIB_ozge)

# Contrib Project: matching_gui
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_matching_gui "Build contrib/matching_gui" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_matching_gui "Build contrib/matching_gui" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_matching_gui)
    SUBDIRS( matching_gui )
ENDIF(BUILD_CONTRIB_matching_gui)

# Contrib Project: fatih
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_fatih "Build contrib/fatih" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_fatih "Build contrib/fatih" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_fatih)
    SUBDIRS( fatih )
ENDIF(BUILD_CONTRIB_fatih)


# Contrib Project: world_model
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_world_model "Build contrib/world_model" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_world_model "Build contrib/world_model" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_world_model)
    SUBDIRS( world_model )
ENDIF(BUILD_CONTRIB_world_model)

# Contrib Project: ozge
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_cluster_mpi "Build contrib/cluster_mpi" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_cluster_mpi "Build contrib/cluster_mpi" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_cluster_mpi)
    SUBDIRS( cluster_mpi )
ENDIF(BUILD_CONTRIB_cluster_mpi)


# Contrib Project: curve2d
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_curve2d "Build contrib/curve2d" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_curve2d "Build contrib/curve2d" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_curve2d)
    SUBDIRS( curve2d )
ENDIF(BUILD_CONTRIB_curve2d)

# Contrib Project: ntrinh
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_ntrinh "Build contrib/ntrinh" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_ntrinh "Build contrib/ntrinh" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_ntrinh)
    SUBDIRS( ntrinh )
ENDIF(BUILD_CONTRIB_ntrinh)

# Contrib Project: biotree
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_biotree "Build contrib/biotree" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_biotree "Build contrib/biotree" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_biotree)
    SUBDIRS( biotree )
ENDIF(BUILD_CONTRIB_biotree)

# Contrib Project: shock2d
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_shock2d "Build contrib/shock2d" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_shock2d "Build contrib/shock2d" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_shock2d)
    SUBDIRS( shock2d )
ENDIF(BUILD_CONTRIB_shock2d)

# Contrib Project: edge_det
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_edge_det "Build contrib/edge_det" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_edge_det "Build contrib/edge_det" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_edge_det)
    SUBDIRS( edge_det )
ENDIF(BUILD_CONTRIB_edge_det)

# Contrib Project: rfabbri
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_rfabbri "Build contrib/rfabbri" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_rfabbri "Build contrib/rfabbri" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_rfabbri)
    SUBDIRS( rfabbri )
ENDIF(BUILD_CONTRIB_rfabbri)

#contrib project: dbctrkapp
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_dbctrkapp "Build contrib/dbctrkapp" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_dbctrkapp "Build contrib/dbctrkapp" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_dbctrkapp)
    SUBDIRS( dbctrkapp)
ENDIF(BUILD_CONTRIB_dbctrkapp)

# Contrib Project: can
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_can "Build contrib/can" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_can "Build contrib/can" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_can)
    SUBDIRS( can )
ENDIF(BUILD_CONTRIB_can)

#contrib project: dbpz_hindsite_2
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_dbpz_hindsite_2 "Build contrib/dbpz_hindsite_2" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_dbpz_hindsite_2 "Build contrib/dbpz_hindsite_2" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_dbpz_hindsite_2)
    SUBDIRS( dbpz_hindsite_2 )
ENDIF(BUILD_CONTRIB_dbpz_hindsite_2)

#contrib project: dbpz_hindsite
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_dbpz_hindsite "Build contrib/dbpz_hindsite" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_dbpz_hindsite "Build contrib/dbpz_hindsite_2" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_dbpz_hindsite)
    SUBDIRS( dbpz_hindsite )
ENDIF(BUILD_CONTRIB_dbpz_hindsite)

# Contrib Project: mleotta
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_mleotta "Build contrib/mleotta" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_mleotta "Build contrib/mleotta" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_mleotta)
    SUBDIRS( mleotta )
ENDIF(BUILD_CONTRIB_mleotta)


# Contrib Project: visualizer
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_visualizer "Build contrib/visualizer" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_visualizer "Build contrib/visualizer" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_visualizer)
    SUBDIRS( visualizer )
ENDIF(BUILD_CONTRIB_visualizer)

## Contrib Project: shannon
#IF( DART_ROOT )
#  OPTION(BUILD_CONTRIB_shannon "Build contrib/shannon" NO)
#ELSE( DART_ROOT )
#  OPTION(BUILD_CONTRIB_shannon "Build contrib/shannon" NO)
#ENDIF( DART_ROOT )
#IF(BUILD_CONTRIB_shannon)
#    SUBDIRS( shannon )
#ENDIF(BUILD_CONTRIB_shannon)


# Contrib Project: medial_livewire
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_medial_livewire "Build contrib/medial_livewire" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_medial_livewire "Build contrib/medial_livewire" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_medial_livewire)
    SUBDIRS( medial_livewire )
ENDIF(BUILD_CONTRIB_medial_livewire)



# Contrib Project: vehicleseg
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_vehicleseg "Build contrib/vehicleseg" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_vehicleseg "Build contrib/vehicleseg" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_vehicleseg)
    SUBDIRS( vehicleseg)
ENDIF(BUILD_CONTRIB_vehicleseg)



# Contrib Project: itrack
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_itrack "Build contrib/itrack" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_itrack "Build contrib/itrack" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_itrack)
    SUBDIRS( itrack )
ENDIF(BUILD_CONTRIB_itrack)


# Contrib Project: geodesic_mesh
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_geodesic_mesh "Build contrib/geodesic_mesh" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_geodesic_mesh "Build contrib/geodesic_mesh" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_geodesic_mesh)
    SUBDIRS( geodesic_mesh )
ENDIF(BUILD_CONTRIB_geodesic_mesh)


# Contrib Project: polygon_triangulation
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_polygon_triangulation "Build contrib/polygon_triangulation" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_polygon_triangulation "Build contrib/polygon_triangulation" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_polygon_triangulation)
    SUBDIRS( polygon_triangulation )
ENDIF(BUILD_CONTRIB_polygon_triangulation)

# Contrib Project: levelset_seg
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_levelset_seg "Build contrib/levelset_seg" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_levelset_seg "Build contrib/levelset_seg" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_levelset_seg)
    SUBDIRS( levelset_seg )
ENDIF(BUILD_CONTRIB_levelset_seg)

# Contrib Project: dbacm3d_example
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_dbacm3d_example "Build contrib/dbacm3d_example" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_dbacm3d_example "Build contrib/dbacm3d_example" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_dbacm3d_example)
    SUBDIRS( dbacm3d_example )
ENDIF(BUILD_CONTRIB_dbacm3d_example)

# Contrib Project: tpollard
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_tpollard "Build contrib/tpollard" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_tpollard "Build contrib/tpollard" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_tpollard)
    SUBDIRS( tpollard )
ENDIF(BUILD_CONTRIB_tpollard)

# Contrib Project: manifold_extraction
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_manifold_extraction "Build contrib/manifold_extraction" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_manifold_extraction "Build contrib/manifold_extraction" NO)
ENDIF( DART_ROOT )

IF(BUILD_CONTRIB_manifold_extraction)
    SUBDIRS( manifold_extraction )
ENDIF(BUILD_CONTRIB_manifold_extraction)

# Contrib Project: knee_cartilage
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_knee_cartilage "Build contrib/knee_cartilage" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_knee_cartilage "Build contrib/knee_cartilage" NO)
ENDIF( DART_ROOT )

# Contrib Project: isa
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_isabel "Build contrib/isabel" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_isabel "Build contrib/isabel" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_isabel)
    SUBDIRS( isabel )
ENDIF(BUILD_CONTRIB_isabel)

IF(BUILD_CONTRIB_knee_cartilage)
    SUBDIRS( knee_cartilage )
ENDIF(BUILD_CONTRIB_knee_cartilage)

# Contrib Project: labelcontour
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_labelcontour "Build contrib/labelcontour" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_labelcontour "Build contrib/labelcontour" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_labelcontour)
    SUBDIRS( labelcontour )
ENDIF(BUILD_CONTRIB_labelcontour)

# Contrib Project: amir
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_amir "Build contrib/amir" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_amir "Build contrib/amir" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_amir)
    SUBDIRS( amir )
ENDIF(BUILD_CONTRIB_amir)

#SUBDIRS( top_down_segmentation )


# Contrib Project: yong
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_yong "Build contrib/yong" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_yong "Build contrib/yong" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_yong)
    SUBDIRS( yong )
ENDIF(BUILD_CONTRIB_yong)

# Contrib Project: spatemp
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_spatemp "Build contrib/spatemp" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_spatemp "Build contrib/spatemp" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_spatemp)
    SUBDIRS( spatemp)
ENDIF(BUILD_CONTRIB_spatemp)



# Contrib Project: dec
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_dec "Build contrib/dec" YES)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_dec "Build contrib/dec" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_dec)
    SUBDIRS( dec )
ENDIF(BUILD_CONTRIB_dec)


# Contrib Project: edubrito
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_edubrito "Build contrib/edubrito" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_edubrito "Build contrib/edubrito" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_edubrito)
    SUBDIRS( edubrito )
ENDIF(BUILD_CONTRIB_edubrito)

# Contrib Project: maruthi
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_maruthi "Build contrib/maruthi" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_maruthi "Build contrib/maruthi" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_maruthi)
    SUBDIRS( maruthi )
ENDIF(BUILD_CONTRIB_maruthi)

# Contrib Project: bwm_lidar
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_bwm_lidar "Build contrib/bwm_lidar" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_bwm_lidar "Build contrib/bwm_lidar" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_bwm_lidar)
    SUBDIRS( bwm_lidar )
ENDIF(BUILD_CONTRIB_bwm_lidar)

# Contrib Project: anil
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_anil "Build contrib/anil" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_anil "Build contrib/anil" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_anil)
    SUBDIRS( anil )
ENDIF(BUILD_CONTRIB_anil)

# Contrib Project: bm
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_bm "Build contrib/bm" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_bm "Build contrib/bm" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_bm)
  SUBDIRS( bm )
ENDIF(BUILD_CONTRIB_bm)

# Contrib Project: gould
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_gould "Build contrib/gould" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_gould "Build contrib/gould" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_gould)
  SUBDIRS( gould )
ENDIF(BUILD_CONTRIB_gould)

# Contrib Project: firat
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_firat "Build contrib/firat" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_firat "Build contrib/firat" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_firat)
    SUBDIRS( firat )
ENDIF(BUILD_CONTRIB_firat)

# Contrib Project: octi
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_octi "Build contrib/octi" NO)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_octi "Build contrib/octi" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_octi)
    SUBDIRS( octi )
ENDIF(BUILD_CONTRIB_octi)

# Contrib Project: Anant
# IF( DART_ROOT )
#   OPTION(BUILD_CONTRIB_Anant "Build contrib/Anant" NO)
# ELSE( DART_ROOT )
#   OPTION(BUILD_CONTRIB_Anant "Build contrib/Anant" NO)
# ENDIF( DART_ROOT )
# IF(BUILD_CONTRIB_Anant)
#     SUBDIRS( Anant )
# ENDIF(BUILD_CONTRIB_Anant)

# Contrib Project: mauro
IF( DART_ROOT )
  OPTION(BUILD_CONTRIB_mauro "Build contrib/mauro" YES)
ELSE( DART_ROOT )
  OPTION(BUILD_CONTRIB_mauro "Build contrib/mauro" NO)
ENDIF( DART_ROOT )
IF(BUILD_CONTRIB_mauro)
    SUBDIRS( mauro )
ENDIF(BUILD_CONTRIB_mauro)

