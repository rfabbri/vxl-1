[neck_transformation, neck_average_aspect_ratios] = learn_part_whole_bb_transformation('/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/giraffes', '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/giraffe-heads', '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/giraffe-heads/giraffehead-gt-xgraph.txt', '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_originals', 'jpg', 3);

[body_transformation, body_average_aspect_ratios] = learn_part_whole_bb_transformation('/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/giraffes', '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/giraffe-body', '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/giraffe-body/giraffebody-gt-xgraph.txt', '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_originals', 'jpg', 1);

[neck_mu,neck_sigma] = learn_detection_confidence_distribution('/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/giraffe-heads/giraffehead-gt-xgraph.txt', '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/giraffehead_exp1_v1.03', 'xml_det_record+giraffes_green.xgraph.0+');

[body_mu,body_sigma] = learn_detection_confidence_distribution('/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/giraffe-body/giraffebody-gt-xgraph.txt', '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/giraffebody_exp1_v1.03', 'xml_det_record+giraffes_green.xgraph.0+');



[whole_transformation, whole_average_aspect_ratios] = learn_part_whole_bb_transformation('/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/giraffes', '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/giraffes', '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/giraffe-heads/giraffehead-gt-xgraph.txt', '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_originals', 'jpg', 1);

[whole_mu,whole_sigma] = learn_detection_confidence_distribution('/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/giraffe-body/giraffebody-gt-xgraph.txt', '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/giraffes_exp4_nc_v1.3_orient_45_dist_4', 'xml_det_record+giraffes_prototype_0+');

[whole_mu,whole_sigma] = learn_detection_confidence_distribution('/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/giraffe-body/giraffebody-gt-xgraph.txt', '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/giraffes_exp5_nc_v1.3_orient_30', 'xml_det_record+giraffes_prototype_0+');
