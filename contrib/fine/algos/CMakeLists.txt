# contrib/seg/algos/CMakeLists.txt

IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )


SET(dbseg_algos_sources

    
    mean_shift/dbseg_mean_shift_ms.cpp                    mean_shift/dbseg_mean_shift_ms.h
    mean_shift/dbseg_mean_shift_msImageProcessor.cpp    mean_shift/dbseg_mean_shift_msImageProcessor.h
    mean_shift/dbseg_mean_shift_msSys.cpp                mean_shift/dbseg_mean_shift_msSys.h
    mean_shift/dbseg_mean_shift_RAList.cpp                mean_shift/dbseg_mean_shift_RAList.h
    mean_shift/dbseg_mean_shift_rlist.cpp                mean_shift/dbseg_mean_shift_rlist.h
                                                        mean_shift/dbseg_mean_shift_tdef.h
                                        


                                        
                                                        graph/dbseg_graph_convolve.h
    graph/dbseg_graph_disjoint-set.cxx                    graph/dbseg_graph_disjoint-set.h
                                                        graph/dbseg_graph_filter.h
                                                        graph/dbseg_graph_image.h
                                                        graph/dbseg_graph_imconv.h
                                                        graph/dbseg_graph_imutil.h
                                                        graph/dbseg_graph_misc.h
                                                        graph/dbseg_graph_pnmfile.h
                                                        graph/dbseg_graph_segment-graph.h
                                                        graph/dbseg_graph_segment-image.h


    jseg/dbseg_jseg_imgutil.c                            jseg/dbseg_jseg_imgutil.h
    jseg/dbseg_jseg_ioutil.c                            jseg/dbseg_jseg_ioutil.h
    jseg/dbseg_jseg_mathutil.c                            jseg/dbseg_jseg_mathutil.h
    jseg/dbseg_jseg_memutil.c                            jseg/dbseg_jseg_memutil.h
    jseg/dbseg_jseg_quan.c                                jseg/dbseg_jseg_quan.h
    jseg/dbseg_jseg_segment.c                            jseg/dbseg_jseg_segment.h
                                                        jseg/dbseg_jseg_xv.h
    jseg/dbseg_jseg_jfunc.c                    
    jseg/dbseg_jseg_xvgif.c    
    jseg/dbseg_jseg_reggrow.c                
    jseg/dbseg_jseg_xvmisc.c
    jseg/dbseg_jseg_xvgifwr.c
    jseg/dbseg_jseg_xv24to8.c
    #jseg/dbseg_jseg_cjpeg.c
    
    #ripple/dbseg_ripple_LinkedContour.cc                ripple/dbseg_ripple_LinkedContour.h
    #ripple/dbseg_ripple_ProbMap.cc                        ripple/dbseg_ripple_ProbMap.h
    #ripple/dbseg_ripple_RippleFilter.cc                    ripple/dbseg_ripple_RippleFilter.h
    
    
    #missing basic_utils.h
    #snakes/dbseg_snakes_matrix_lib.c                    snakes/dbseg_snakes_matrix_lib.h
    #                                                    snakes/dbseg_snakes_snakes.h
    #snakes/dbseg_snakes_snakes_top.c
    #snakes/dbseg_snakes_stop.c                            snakes/dbseg_snakes_stop.h
    #snakes/dbseg_snakes_test.c
    #snakes/dbseg_snakes_utils.c                            snakes/dbseg_snakes_utils.h
    
    #subpixel/dbseg_subpixel_curvature.c
    #subpixel/dbseg_subpixel_deform_SP_bubbles.c
    #subpixel/dbseg_subpixel_eno_utils.c
    #subpixel/dbseg_subpixel_flux.c
    #subpixel/dbseg_subpixel_force.c
    #subpixel/dbseg_subpixel_force_old.c
    #subpixel/dbseg_subpixel_initialize.c
    #subpixel/dbseg_subpixel_initialize_raphael.c
    #subpixel/dbseg_subpixel_initialize_utils.c
    #subpixel/dbseg_subpixel_inter_bubble_shocks.c
    #subpixel/dbseg_subpixel_inter_bubble_shocks_utils.c
    #subpixel/dbseg_subpixel_level_set.c
    ##subpixel/dbseg_subpixel_merge_main.c
    #subpixel/dbseg_subpixel_merge_regions.c
    #subpixel/dbseg_subpixel_normal_interp.c
    #subpixel/dbseg_subpixel_read_parameters.c
    #subpixel/dbseg_subpixel_region_utils.c
    #subpixel/dbseg_subpixel_save_results.c
    #subpixel/dbseg_subpixel_shock_to_cedt_boundary.c
    #subpixel/dbseg_subpixel_boundary_fill.c
    #subpixel/dbseg_subpixel_snake_force.c
    #                                                    subpixel/dbseg_subpixel_subpixel_bubbles.h
    #subpixel/dbseg_subpixel_utils.c
    #subpixel/dbseg_subpixel_1d_eno_types.h
    #subpixel/dbseg_subpixel_3dUtils.h
    #subpixel/dbseg_subpixel_boundary_from_geno.h
    #subpixel/dbseg_subpixel_bubbles.h
    #subpixel/dbseg_subpixel_cedt.h
    #subpixel/dbseg_subpixel_cedt3d.h
    #subpixel/dbseg_subpixel_complex.h
    #subpixel/dbseg_subpixel_contour_tracer.h
    #subpixel/dbseg_subpixel_curve-match.h
    #subpixel/dbseg_subpixel_distance_transform.h
    #subpixel/dbseg_subpixel_draw_engine.h
    #subpixel/dbseg_subpixel_fast_marching.h
    #subpixel/dbseg_subpixel_fresnel_solution.h
    #subpixel/dbseg_subpixel_func.h
    #subpixel/dbseg_subpixel_geno-types.h
    #subpixel/dbseg_subpixel_geno.h
    #subpixel/dbseg_subpixel_graphics.h
    #subpixel/dbseg_subpixel_image_processing.h
    #subpixel/dbseg_subpixel_imageTools.h
    #subpixel/dbseg_subpixel_morph.h
    #subpixel/dbseg_subpixel_morph_sub.h
    #subpixel/dbseg_subpixel_non-uniform-eno.h
    #subpixel/dbseg_subpixel_nrutil.h
    #subpixel/dbseg_subpixel_numeric.h
    #subpixel/dbseg_subpixel_postscript.h
    #subpixel/dbseg_subpixel_postscriptN.h
    #subpixel/dbseg_subpixel_rawimageio.h
    #subpixel/dbseg_subpixel_redt.h
    #subpixel/dbseg_subpixel_seeded_reg_grow.h
    #subpixel/dbseg_subpixel_shocks_from_cedt.h
    #subpixel/dbseg_subpixel_SP_Shocks_Cedt_types.h
    #subpixel/dbseg_subpixel_SPCedtGeno.h
    #subpixel/dbseg_subpixel_statistics.h
    #subpixel/dbseg_subpixel_tbs-utils.h
    #subpixel/dbseg_subpixel_twod_dpoint.h
    #subpixel/dbseg_subpixel_utils.h
    #subpixel/dbseg_subpixel_wave_propagation.h
    
    
    #swgraph/dbseg_swgraph_FrameMap.cpp                        swgraph/dbseg_swgraph_FrameMap.h
    #swgraph/dbseg_swgraph_Functions.cpp                        swgraph/dbseg_swgraph_Functions.h
    #swgraph/dbseg_swgraph_gauss.cpp                            swgraph/dbseg_swgraph_gauss.h
    #swgraph/dbseg_swgraph_graph.cpp                            swgraph/dbseg_swgraph_graph.h
    #swgraph/dbseg_swgraph_histogram.cpp                        swgraph/dbseg_swgraph_histogram.h
    #swgraph/dbseg_swgraph_ImgFunctions.cpp                    swgraph/dbseg_swgraph_ImgFunctions.h
    #                                                        swgraph/dbseg_swgraph_irmm_LinkList.h
    #                                                        swgraph/dbseg_swgraph_irmm_Raster.h
    #                                                        swgraph/dbseg_swgraph_irmm_RasterColor.h
    #                                                        swgraph/dbseg_swgraph_Matrix.h
    #swgraph/dbseg_swgraph_mcPublic.cpp                        swgraph/dbseg_swgraph_mcPublic.h
    #swgraph/dbseg_swgraph_Point.cpp                            swgraph/dbseg_swgraph_Point.h
    #swgraph/dbseg_swgraph_pointattributes.cpp                swgraph/dbseg_swgraph_PointAttributes.h
    #swgraph/dbseg_swgraph_Sampler.cpp                        swgraph/dbseg_swgraph_Sampler.h
    #swgraph/dbseg_swgraph_SimpleModel.cpp                    swgraph/dbseg_swgraph_SimpleModel.h
    #                                                        swgraph/dbseg_swgraph_stl_util.h
    #                                                        swgraph/dbseg_swgraph_SW_Framework.h
    #swgraph/dbseg_swgraph_SwendsenWang.cpp                    swgraph/dbseg_swgraph_SwendsenWang.h
    #swgraph/dbseg_swgraph_SwendsenWangGraph.cpp                swgraph/dbseg_swgraph_SwendsenWangGraph.h
    #swgraph/dbseg_swgraph_UnionFind.cpp                        swgraph/dbseg_swgraph_UnionFind.h
    #                                                        swgraph/dbseg_swgraph_Vector.h
    
    srgtek/dbseg_srgtek_contour_routines.c                    srgtek/dbseg_srgtek_contour_routines.h
    #srgtek/dbseg_srgtek_contours.c
    srgtek/dbseg_srgtek_find_contours.c                        srgtek/dbseg_srgtek_find_contours.h
    srgtek/dbseg_srgtek_grow.c                                srgtek/dbseg_srgtek_grow.h
    #srgtek/dbseg_srgtek_mean.c
    #srgtek/dbseg_srgtek_seeded_rg.c
    srgtek/dbseg_srgtek_SeedFill.c
    srgtek/dbseg_srgtek_utils.c                                srgtek/dbseg_srgtek_utils.h
    
    
    
    #                                                        levelset/dbseg_levelset_cimg_dependent.h
    #                                                        levelset/dbseg_levelset_colordefs.h
    #                                                        levelset/dbseg_levelset_Filters.h
    #                                                        levelset/dbseg_levelset_image_utils.h
    #                                                        levelset/dbseg_levelset_level_set.h
    #                                                        levelset/dbseg_levelset_LevelSetFunction.h
    #                                                        levelset/dbseg_levelset_LevelSetFunction_implementation.h
    #                                                        levelset/dbseg_levelset_NormalDistributionForce.h
    #                                                        levelset/dbseg_levelset_ParzenDistributionForce.h
    #                                                        levelset/dbseg_levelset_Region.h
    #                                                        levelset/dbseg_levelset_simple_tools.h
    #                                                        levelset/dbseg_levelset_simple_tools_templates.h
    #                                                        levelset/dbseg_levelset_SingleRegionAlgorithm.h
    #                                                        levelset/dbseg_levelset_SingleRegionAlgorithm_implementation.h
    #                                                        levelset/dbseg_levelset_errormacros.h
    #                                                        levelset/dbseg_levelset_Image.h
    #                                                        levelset/dbseg_levelset_Mask.h
    #                                                        levelset/dbseg_levelset_ForceGenerator.h
    #                                                        levelset/dbseg_levelset_Histogram.h
    #                                                        levelset/dbseg_levelset_MultiRegionAlgorithm.h
    #                                                        levelset/dbseg_levelset_BorderCondition.h
    #levelset/dbseg_levelset_lambda.hpp
    
    
    
    
    #levelset2/dbseg_levelset2_BMatrix.cpp                    levelset2/dbseg_levelset2_BMatrix.h
    #levelset2/dbseg_levelset2_BPoint.cpp                    levelset2/dbseg_levelset2_BPoint.h
    #levelset2/dbseg_levelset2_BVector.cpp                    levelset2/dbseg_levelset2_BVector.h
    #levelset2/dbseg_levelset2_Desc.cpp                        levelset2/dbseg_levelset2_Desc.h
    #levelset2/dbseg_levelset2_gnuplot_i.c                    levelset2/dbseg_levelset2_gnuplot_i.h
    #levelset2/dbseg_levelset2_ImageObj.cpp                    levelset2/dbseg_levelset2_ImageObj.h
    #                                                        levelset2/dbseg_levelset2_Mh.h
    #levelset2/dbseg_levelset2_RGBImg.cpp                    levelset2/dbseg_levelset2_RGBImg.h
    #levelset2/dbseg_levelset2_Util.cpp                        levelset2/dbseg_levelset2_Util.h
    
    
)
ADD_LIBRARY(dbseg_algos ${dbseg_algos_sources})
TARGET_LINK_LIBRARIES(dbseg_algos)

ENDIF (VXL_VGUI_FOUND)


