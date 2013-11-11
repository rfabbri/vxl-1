#include "dbrec_register.h"
#include "dbrec_processes.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include <dbrec/dbrec_part_sptr.h>
#include <dbrec/dbrec_part_context_sptr.h>
#include <dbrec/dbrec_visitor_sptr.h>
#include <dbrec/dbrec_part_selector.h>

void dbrec_register::register_datatype()
{
  REGISTER_DATATYPE( dbrec_hierarchy_sptr );
  REGISTER_DATATYPE( dbrec_context_factory_sptr );
  REGISTER_DATATYPE( dbrec_visitor_sptr );
  REGISTER_DATATYPE( dbrec_part_selection_measure_sptr );
}

void dbrec_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_create_hierarchy_process, "dbrecCreateHierarchyProcess");
  
  //: processes related to new objects in dbrec
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_image_create_random_hierarchy_process, "dbrecImgCreateRndHierarchyProcess");
  REG_PROCESS_FUNC_CONS_INIT(bprb_func_process, bprb_batch_process_manager, dbrec_image_parse_with_fg_process, "dbrecImgParseWithFGProcess");
  REG_PROCESS_FUNC_CONS_INIT(bprb_func_process, bprb_batch_process_manager, dbrec_image_parse_process, "dbrecImgParseProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_image_weibull_model_learner_init_process, "dbrecImgWeibullLearnerInitProcess");
  REG_PROCESS_FUNC_CONS_INIT(bprb_func_process, bprb_batch_process_manager, dbrec_image_weibull_model_update_process, "dbrecImgWeibullLearnerUpdateProcess"); 
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_image_weibull_model_learner_print_process, "dbrecImgWeibullLearnerPrintProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_image_hierarchy_factory_process, "dbrecImgHierarchyFactoryProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_image_hierarchy_factory_rotated_process, "dbrecImgHierarchyFactoryRotatedProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_visualize_hierarchy_process, "dbrecHierarchyVisualizeProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_visualize_sampled_class_process, "dbrecHierarchyVisualizeSampledClass");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_context_factory_get_map_process, "dbrecContextFactoryGetMapProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_context_factory_write_maps_process, "dbrecContextFactoryWriteMapsProcess");
  
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_construct_bg_op_models_process, "dbrecConstructBGOpModelsProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_write_hierarchy_xml_process, "dbrecWriteHierarchyXMLProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_parse_hierarchy_xml_process, "dbrecParseHierarchyXMLProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_classify_image_process, "dbrecClassifyImageProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_hierarchy_learner_init_process, "dbrecImgHierarchyLearnerInitProcess");
  REG_PROCESS_FUNC_CONS_INIT(bprb_func_process, bprb_batch_process_manager, dbrec_hierarchy_learner_update_process, "dbrecImgHierarchyLearnerUpdateProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_hierarchy_learner_construct_process, "dbrecImgHierarchyLearnerConstructProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_image_populate_hierarchy_pairwise_central_process, "dbrecImgHierarchyPopulatePCProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_image_populate_sample_hierarchy_pairwise_central_process, "dbrecImgHierarchyPopulateSamplePCProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_image_populate_sample_indep_gaus_hierarchy_pairwise_central_process, "dbrecImgHierarchyPopulateSampleIGPCProcess");
  
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_image_train_composite_parts_process, "dbrecImgTrainCompositePartsProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_visualize_hierarchy_models_process, "dbrecHierarchyVisualizeModelsProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_hierarchy_get_type_id_process, "dbrecHierarchyGetTypeIDProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_sample_rot_inv_parts_process, "dbrecImgSampleRotInvPartsProcess");
  REG_PROCESS_FUNC_CONS_INIT(bprb_func_process, bprb_batch_process_manager, dbrec_image_parse_rot_inv_process, "dbrecImgParseRotInvPartsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_image_rot_inv_weibull_model_learner_init_process, "dbrecImgRotInvWeibullLearnerInitProcess");
  REG_PROCESS_FUNC_CONS_INIT(bprb_func_process, bprb_batch_process_manager, dbrec_image_rot_inv_weibull_model_update_process, "dbrecImgRotInvWeibullLearnerUpdateProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_image_rot_inv_weibull_model_learner_print_process, "dbrecImgRotInvWeibullLearnerPrintProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_context_factory_get_response_map_process, "dbrecContextFactoryGetResponseMapProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_context_factory_create_response_histogram_process, "dbrecContextFactoryCreateResponseHistProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_construct_bg_op_models_rot_inv_process, "dbrecConstructBGOpModelsRotInvProcess"); 
  REG_PROCESS_FUNC_CONS_INIT(bprb_func_process, bprb_batch_process_manager, dbrec_image_parse_rot_inv_with_fg_process, "dbrecImgParseRotInvPartsWithFGProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_image_collect_stats_rot_inv_process, "dbrecImgCollectStatsRotInvProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_image_train_rot_inv_composite_parts_process, "dbrecImgTrainRotInvCompositePartsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_image_populate_hierarchy_pairwise_discrete_process, "dbrecImgHierarchyPopulateRotInvPDProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, dbrec_image_entropy_selector_pairwise_discrete_process, "dbrecImgEntropySelectorProcess");
}
