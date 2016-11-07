files = dir(['/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_originals/giraffes*']);
%files = dir(['/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_originals/*.jpg']);
confidences = cell(length(files),1);
tpfp = cell(length(files),1);
total_gt = 0;
for i = 1:length(files)
    image_name = files(i).name
    image_name = image_name(1:end-4);
    [confidences{i}, tpfp{i}, num_gt] = evaluate(image_name, '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_originals', 'jpg', '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_groundtruths_patched2', 'giraffes', {'/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/giraffehead_exp1_v1.03'}, {'xml_det_record+giraffes_green.xgraph.0+'}, {neck_transformation}, {neck_average_aspect_ratios}, {neck_mu}, {neck_sigma},0,'~/Desktop/neck');
    %[confidences{i}, tpfp{i}, num_gt] = evaluate(image_name, '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_originals', 'jpg', '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_groundtruths_patched2', 'giraffes',{'/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/firat-experiments/giraffeneck_exp2_v1.03'}, {'xml_det_record+giraffes_green.xgraph.0+'}, {neck_transformation}, {neck_average_aspect_ratios}, {neck_mu}, {neck_sigma},1,'~/Desktop/neck');
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
