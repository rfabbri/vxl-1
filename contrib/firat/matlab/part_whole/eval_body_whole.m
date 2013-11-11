files = dir(['/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_originals/giraffes*']);
confidences = cell(length(files),1);
tpfp = cell(length(files),1);
total_gt = 0;
for i = 1:length(files)
    image_name = files(i).name
    image_name = image_name(1:end-4);
    [confidences{i}, tpfp{i}, num_gt] = evaluate(image_name, '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_originals', 'jpg', '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_groundtruths_patched2', {'/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/giraffebody_exp1_v1.03', '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/giraffes_exp5_nc_v1.3_orient_30'}, {'xml_det_record+giraffes_green.xgraph.0+', 'xml_det_record+giraffes_prototype_0+'}, {body_transformation, whole_transformation}, {body_average_aspect_ratios, whole_average_aspect_ratios}, {body_mu, whole_mu}, {body_sigma, whole_sigma}, 0, '~/Desktop/whole');
    total_gt = total_gt + num_gt;
end
confidences = cell2mat(confidences);
tpfp = cell2mat(tpfp);
[sorted_confidences, sort_index] = sort(confidences, 'descend');
tpfp = tpfp(sort_index);
recall = cumsum(tpfp) / total_gt;
fp = 1 - tpfp;
fppi = cumsum(fp) / length(files);
plot(fppi, recall, 'r-')
axis([0 1.5 0 1])
tp_index = find(tpfp);
recall_end = recall(end)
recall_index = recall(tp_index(end))
fppi_index = fppi(tp_index(end))

