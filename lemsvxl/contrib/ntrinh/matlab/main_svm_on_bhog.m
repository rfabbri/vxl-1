% This script trains boundary hog using svm
% (c) Nhon Trinh
% Oct 1, 2008

clear all;
close all;

data_folder = 'D:\vision\data\symseg\xshock\ETHZ-shapes\training';
pos_file = fullfile(data_folder, 'giraffes-eid_8-positive-sym_orient_alignment.txt');
neg_file = fullfile(data_folder, 'giraffes-eid_8-negative-sym_orient_alignment.txt');

%pos_feat = dlmread(pos_file, ' ', 0, 0);
%neg_feat = dlmread(neg_file, ' ', 0, 0);

load(fullfile(data_folder, 'pos.mat'));
load(fullfile(data_folder, 'neg.mat'));



% rescale data so that sum of the components in each = 1
pos_mag = sum(pos_feat, 2) + 1e-6;
pos_feat = pos_feat ./ repmat(pos_mag, [1, size(pos_feat, 2)]);

neg_mag = sum(neg_feat, 2) + 1e-6;
neg_feat = neg_feat ./ repmat(neg_mag, [1, size(neg_feat, 2)]);

pos_label = ones(size(pos_feat, 1), 1);
neg_label = ones(size(neg_feat, 1), 1) * (-1);

% form a training set by taking half of positive and half of negative
% samples
num_pos = size(pos_feat, 1);
num_neg = size(neg_feat, 1);
half_pos = round(num_pos / 2);
half_neg = round(num_neg / 2);

train_feat = [pos_feat(1:half_pos, :); neg_feat(1:half_neg, :)];
train_label = [pos_label(1:half_pos, :); neg_label(1:half_neg, :)];

test_feat = [pos_feat(half_pos+1:end, :); neg_feat(half_neg+1:end, :)];
test_label = [pos_label(half_pos+1:end, :); neg_label(half_neg+1:end, :)];


%model = svmtrain(train_label, train_feat, '-t 2 -c 1 -b 1');
%[predicted_label, accuracy, decision_values] = svmpredict(test_label, test_feat, model);

