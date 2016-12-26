% this script load the output prediction file of libsvm and plot the error
% rate
% (c) Nhon Trinh
% Date: Oct 4, 2008



%% Input
input_folder = 'D:\vision\projects\symseg\xshock\ETHZ-shapes\training\giraffes-eid_8-nrm';
predict_file = 'libsvm_bhog.test2.predict1';
threshold = 0.5;

%% Process

data = dlmread(fullfile(input_folder, predict_file), ' ', 1, 0);

label = data(:, 1);
prob_neg = data(:, 2);
prob_pos = data(:, 3);
` 
predict_label = zeros(length(label), 1);
mask = find(prob_pos > threshold);
mask_comp = find(prob_pos <= threshold);
predict_label(mask) = label(mask);
predict_label(mask_comp) = -label(mask_comp);


correct = abs(predict_label - test_label) == 0;
num_correct = sum(correct)
