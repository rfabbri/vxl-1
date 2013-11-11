function [train_feat, train_label, test_feat, test_label] = main_generate_bhog_train_test()
% This script analyzes the shape fragment from a shape class
% (c) Nhon Trinh
% Date: Sep 22, 2008

%close all;
%clear all;

%% input
input_folder = 'D:\vision\projects\symseg\xshock\ETHZ-shapes\training\giraffes-eid_8-nrm';
pos_filename = 'bhog-giraffes-eid_8-normalized-each_1000.pos3';
neg_filename = 'bhog-giraffes-eid_8-normalized-each_9.neg3';

%% process data

% parse input data
pos_file = fullfile(input_folder, pos_filename);
pos_data = dlmread(pos_file, ' ', 0, 0);

neg_file = fullfile(input_folder, neg_filename);
neg_data = dlmread(neg_file, ' ', 0, 0);

%load pos_data;
%load neg_data;

start = 1;
nbins = 9;
num_segs = 4;

% plot_bhog(neg_data, nbins, num_segs);

% Construct training and testing data
num_pos = size(pos_data, 1);
num_neg = size(neg_data, 1);

num_train_pos = round(0.2* num_pos);
num_train_neg = round(0.2* num_neg);

num_test_pos = round(0.3* num_pos);
num_test_neg = round(0.3* num_neg);

% positive samples
train_pos_mask = unique(ceil(rand(num_train_pos, 1) * num_pos));
train_pos = pos_data(train_pos_mask, :);

pos_data(train_pos_mask, :) = []; % remove data already taken

test_pos_mask = unique(ceil(rand(num_test_pos, 1) * size(pos_data, 1)));
test_pos =  pos_data(test_pos_mask, :);

% negative samples
train_neg_mask = unique(ceil(rand(num_train_neg, 1) * num_neg));
train_neg = neg_data(train_neg_mask, :);

neg_data(train_neg_mask, :) = [];

test_neg_mask = unique(ceil(rand(num_test_neg, 1) * size(neg_data, 1)));
test_neg = neg_data(test_neg_mask, :);


% combine the two
train_feat = [train_pos; train_neg];
test_feat = [test_pos; test_neg];

train_label = [ones(size(train_pos, 1), 1); ones(size(train_neg, 1), 1)*2];
test_label = [ones(size(test_pos, 1), 1); ones(size(test_neg, 1), 1) * 2];


% saving train and test vector
write_libsvm_datafile(train_label, train_feat, 'libsvm_bhog.train3b');
write_libsvm_datafile(test_label, test_feat, 'libsvm_bhog.test3b');

save data3 train_feat train_label test_feat test_label;

disp('done');





%%
function a = plot_bhog(data, nbins, num_segs)
% Plot the boundary HOG of a fragment
% nbins: number of bins in the histogram
% num_segs: number of segments each boundary is divided to

start = 1;

for k = 1 : num_segs
  lhog{k} = data(:, start:start+nbins-1);
  start = start + nbins;
end;

for k = 1 : num_segs
  rhog{k} = data(:, start:start+nbins-1);
  start = start + nbins;
end;

for k = 1 : num_segs
  figure(k);
  hold on;
  plot(mean(lhog{k}), 'b-s');
  plot(std(lhog{k}), 'r-*');
  title(sprintf('lhog%d', k));
  xlabel('orientation bin');
  ylabel('normalized magnitude');
  legend('mean of hog around groundtruth fragments', ...
  'std of hog around groundtruth fragments');
  grid on;
  axis([1, 9, 0, 0.5]);
end;


for k = 1 : num_segs
  figure(k+num_segs);
  hold on;
  plot(mean(rhog{k}), 'b-s');
  plot(std(rhog{k}), 'r-*');
  title(sprintf('rhog%d', k));
  xlabel('orientation bin');
  ylabel('normalized magnitude');
  legend('mean of hog around groundtruth fragments', ...
  'std of hog around groundtruth fragments');
  grid on;
  axis([1, 9, 0, 0.5]);
end;

return;

