files = dir(['/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_originals/giraffes*']);
for i = 1:length(files)
    image_name = files(i).name;
    image_name = image_name(1:end-4);
    predict_whole_bb(image_name, '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_originals', 'jpg', {'/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/giraffehead_exp1_v1.03','/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/giraffebody_exp1_v1.03'}, {'xml_det_record+giraffes_green.xgraph.0+','xml_det_record+giraffes_green.xgraph.0+'}, {neck_transformation, body_transformation}, {neck_average_aspect_ratios, body_average_aspect_ratios}, {neck_mu, body_mu}, {neck_sigma, body_sigma})
end
