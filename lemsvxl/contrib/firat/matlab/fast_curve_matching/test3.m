% This is /lemsvxl/contrib/firat/matlab/fast_curve_matching/test3.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Feb 7, 2011

clear
%close all
obj_contour = load_contours('/home/firat/Desktop/db/1000bottles/bottle1.txt');
query_contours = load_contours('/home/firat/Desktop/db/bottle_contours.txt');



query_curve = query_contours{6}; %6
query_image = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_originals/bottles_torbreck.jpg';
test_curve = obj_contour{1};
F = query_curve;
whos
max_num_samples = 80;

I = imread(query_image);
figure;
imshow(I); hold on;
plot(query_curve(:,1),query_curve(:,2), 'r-','LineWidth', 3); 
hold off;

tic;
%[cf_center, cf_len] = scale_invariant_match(test_curve, F, max_num_samples, 3);
[cf_center, cf_len] = scale_invariant_match2(test_curve, F, max_num_samples, 3, 30);
toc

figure;
for i = 1:length(cf_center)
    j = cf_center(i)/max_num_samples*size(test_curve,1);
    l = cf_len(i)/max_num_samples*size(test_curve,1);
    MF = mod((round(j-l/2):round(j+l/2))-1,   size(test_curve, 1)) + 1;    
    subplot(5,6,i)
    imshow(ones(1,1)); hold on
    plot(test_curve(:,1),test_curve(:,2)); axis off; axis image
    plot(test_curve(MF,1),test_curve(MF,2), 'g', 'LineWidth', 3)
    hold off;
end


